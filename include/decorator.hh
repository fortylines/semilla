/* Copyright (c) 2009-2012, Fortylines LLC
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of fortylines nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY Fortylines LLC ''AS IS'' AND ANY
   EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL Fortylines LLC BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#ifndef guarddecorator
#define guarddecorator

#include <ostream>
#include "session.hh"
#include "tokenize.hh"

/* Decorators are used to highjack the underlying buffer of an ostream
   in order to format the text sent to the ostream.

   Decorators can be attached and detached on the fly, first-in last-out,
   to an ostream. In order to pass a reference to a chain of decorators,
   a decoratorChain class is defined (i.e. composite pattern).


   Primary Author(s): Sebastien Mirolo <smirolo@fortylines.com>
*/

namespace tero {

// forward declaration
template<typename charT, typename traitsT = std::char_traits<charT> >
class basicDecoratorChain;


/** \brief decorate the text outputed through a basic_ostream

	This is the base class for basic_ostream decorators. 
	Derived classes tokenize a stream of raw text and forwards 
	a stream of text decorated with tags to the underlying buffer.
 */
template<typename charT, typename traitsT = std::char_traits<charT> >
class basicDecorator : public std::basic_ostream<charT, traitsT> {
protected:
	friend class basicDecoratorChain<charT,traitsT>;

	typedef std::basic_ostream<charT, traitsT> super;

    std::basic_streambuf<charT, traitsT>* nextBuf;
    std::basic_ostream<charT, traitsT> *next;

	bool pre;

public:
    explicit basicDecorator( std::basic_streambuf<charT,traitsT> *sb, 
			     bool formated = false )
	: super(sb), nextBuf(sb), next(NULL), pre(formated) {
    }
    
    virtual ~basicDecorator() {}
    
    /** \brief Attach the decorator to a basic_ostream
	
	After the call returns, all text sent to the basic_ostream will appear
	decorated in the final output.
    */
    virtual void attach(  std::basic_ostream<charT, traitsT>& o ) = 0;
    
    /** Detach the decorator from a basic_ostream
	
	After the call returns, the decorator will no longer preprocess 
	the text sent to the basic_ostream.
    */
    virtual void detach() = 0;
    
    /** True when the decorator formats the underlying stream layout.
	
	When *formated* is True, an HTML processor will emit <pre> and </pre>
	tags around the decorated text. When *formated* is false, an HTML 
	processor will assume that spaces and carriage returns are not 
	formatters.
    */
    bool formated() const { return pre; }
    
};


/** \brief Composite for attaching multiple decorators as a single decorator.
 */
template<typename charT, typename traitsT>
class basicDecoratorChain : public basicDecorator<charT, traitsT> {
protected:
	typedef basicDecorator<charT, traitsT> super;

	super *first;
	super *last;

public:
	basicDecoratorChain() : super(NULL), first(NULL), last(NULL) {}

	virtual ~basicDecoratorChain();

	virtual void attach(  std::basic_ostream<charT, traitsT>& o );

	virtual void detach();

	void push_back( basicDecorator<charT, traitsT>& );

};


/** \brief Base class to decorate streams of tokens.
 */
template<typename tokenizerT, typename charT, 
		 typename traitsT = std::char_traits<charT> >
class basicHighLight : public basicDecorator<charT, traitsT> {
protected:
	typedef basicDecorator<charT, traitsT> super;

private:
    class buffer : public std::basic_stringbuf<charT, traitsT> {
    public:
	using std::basic_stringbuf<charT, traitsT>::gbump;
	using std::basic_stringbuf<charT, traitsT>::gptr;
	using std::basic_stringbuf<charT, traitsT>::pptr;
	
	explicit buffer( basicHighLight& d ) 
	    : std::basic_stringbuf<charT, traitsT>(),
	      decorator(d) {}
	
	int sync() { return decorator.sync(); }
	
	basicHighLight& decorator;
    };

   buffer buf;

protected:
    tokenizerT tokenizer;
    
    void scan();
    
public:
    explicit basicHighLight( bool formated );
    
    explicit basicHighLight( std::basic_ostream<charT,traitsT>& o, 
			     bool formated = false ); 

    ~basicHighLight();

    virtual void attach( std::ostream& o );

    void detach();

    int sync();

};


/** \brief Escape symbols that would otherwise be interpretated as HTML tags.
 */
template<typename charT, typename traitsT = std::char_traits<charT> >
class basicHtmlEscaper : public basicHighLight<xmlEscTokenizer, charT, traitsT>,
			 public xmlEscTokListener {
protected:
    typedef basicHighLight<xmlEscTokenizer, charT, traitsT> super;

public:
    basicHtmlEscaper() 
		: super(true) { 
		super::tokenizer.attach(*this);
    }
    
    explicit basicHtmlEscaper(  std::basic_ostream<charT,traitsT>& o )
	: super(o,true) { super::tokenizer.attach(*this); }
    
    void newline( const char *line, int first, int last ) {
		super::nextBuf->sputc('\n');
    }
    
    void token( xmlEscToken token, const char *line, 
		int first, int last, bool fragment );
    
};


/** \brief Decorate links with load, create and external. 
 */
template<typename charT, typename traitsT = std::char_traits<charT> >
class basicLinkLight : public basicHighLight<xmlTokenizer, charT, traitsT>,
		       public xmlTokListener {
protected:
    typedef basicHighLight<xmlTokenizer, charT, traitsT> super;

    enum {
	linkStartState,
	linkWaitAttState
    } state;

    /* We classify links between the ones which match a local file,
       the ones that are generated by a local script and the remote
       links. The presentation of local files is cached. */
    enum linkClass {
	localFileExists,
	localLinkGenerated,
	remoteLink
    };
    
    session* context;
    boost::filesystem::path base;

    /** returns true if the orginal text needs to be copied to the output
	stream and false if the method replaced the text already. */
    virtual bool decorate( const url& u );

public:
    typedef std::set<url> linkSet;
    static linkSet allLinks;
    static linkSet currs;
    static linkSet nexts;

    static linkSet::const_iterator begin() { return currs.begin(); }

    static linkSet::const_iterator end() { return currs.end(); }

    static bool empty() { return nexts.empty(); }

    static void clear() {	
	allLinks.insert(currs.begin(),currs.end());
	currs = nexts;
	nexts.clear();
    }

    linkClass add( const url& u );

public:
    explicit basicLinkLight( session& s, const boost::filesystem::path& r )
        : super(false), context(&s), base(r) {
	super::tokenizer.attach(*this);
    }
    
    basicLinkLight(  session& s, const boost::filesystem::path& root,
        std::basic_ostream<charT,traitsT>& o )
        : super(o,false), context(&s), base(root) {
        super::tokenizer.attach(*this);
    }
    
    void newline( const char *line, int first, int last ) {
	super::nextBuf->sputc('\n');
    }
    
    void token( xmlToken token, const char *line, 
		int first, int last, bool fragment );
    
};


/** \brief Transforms all <a href="..."> links to absolute urls.
 */
template<typename charT, typename traitsT = std::char_traits<charT> >
class absUrlDecoratorBase : public basicLinkLight<charT, traitsT> {
protected:
    typedef basicLinkLight<charT, traitsT> super;

    virtual bool decorate( const url& u );

public:
    absUrlDecoratorBase( const boost::filesystem::path& b, session& s ) 
        : super(s,b) {}
    
    absUrlDecoratorBase( const boost::filesystem::path& b, 
		     session& s, std::basic_ostream<charT,traitsT>& o )
        : super(s,b,o) {}
};


/** For each link <a href="...">, if the file exists it will rewrite
    the link to load a cached version (.html) of the file.
 */
template<typename charT, typename traitsT = std::char_traits<charT> >
class cachedUrlBase : public basicLinkLight<charT, traitsT> {
protected:
    typedef basicLinkLight<charT, traitsT> super;

    virtual bool decorate( const url& u );

public:
    explicit cachedUrlBase( session& s, const boost::filesystem::path& r )
        : super(s,r) {}
    
    cachedUrlBase( session& s, const boost::filesystem::path& r,
        std::basic_ostream<charT,traitsT>& o )
        : super(s,r,o) {}
};


/** \brief Decorate a text with href links to pathnames. 
 */
template<typename charT, typename traitsT = std::char_traits<charT> >
class basicHrefLight : public basicHighLight<hrefTokenizer, charT, traitsT>,
		       public hrefTokListener {
protected:
    typedef basicHighLight<hrefTokenizer, charT, traitsT> super;
    
    session* context;
    
public:
    explicit basicHrefLight( session& s ) 
	: super(false), context(&s) { 
	super::tokenizer.attach(*this); 
    }
    
    explicit basicHrefLight(  session& s, std::basic_ostream<charT,traitsT>& o )
	: super(o,false), context(&s) { super::tokenizer.attach(*this); }
    
    void newline( const char *line, int first, int last ) {
	super::nextBuf->sputc('\n');
    }
    
    void token( hrefToken token, const char *line, 
		int first, int last, bool fragment );
    
};


/** \brief Decorate C++ code with token tags.
 */
template<typename charT, typename traitsT = std::char_traits<charT> >
class basicCppLight : public basicHighLight<cppTokenizer,charT,traitsT>,
                       public cppTokListener {
protected:
    bool preprocessing;
    bool virtualLineBreak;
    typedef basicHighLight<cppTokenizer,charT, traitsT>  super;
    
public:
    basicCppLight() 
	: super(true), preprocessing(false), virtualLineBreak(false) { 
	super::tokenizer.attach(*this); 
    }
    
    explicit basicCppLight( std::basic_ostream<charT,traitsT>& o )
	: super(o,true), preprocessing(false), virtualLineBreak(false) { 
	super::tokenizer.attach(*this); 
    }

    void newline(const char *line, int first, int last );
    
    void token( cppToken token, const char *line, 
		int first, int last, bool fragment );
};


/** Base Abstract class to add annotations on each line of a source file.

	Implementation Note: we only need a EOL tokenizer to do that. 
*/
template<typename charT, typename traitsT = std::char_traits<charT> >
class basicAnnotate  : public basicHighLight<xmlTokenizer, charT, traitsT>,
		       public xmlTokListener {
protected:
    typedef basicHighLight<xmlTokenizer, charT, traitsT> super;
	int nbLines;

public:
    basicAnnotate() 
		: super(false), nbLines(1) { 
		super::tokenizer.attach(*this); 
    }
    
    basicAnnotate( std::basic_ostream<charT,traitsT>& o )
		: super(o,false), nbLines(1) {
		super::tokenizer.attach(*this); 
	}

    void newline(const char *line, int first, int last );
    
    void token( xmlToken token, const char *line, 
		int first, int last, bool fragment );
};

/** Annotate source with static checks information
 */
template<typename charT, typename traitsT = std::char_traits<charT> >
class noteAnnotate  : public basicAnnotate<charT,traitsT> {
protected:
    typedef basicAnnotate<charT, traitsT> super;
	typedef std::map<int,std::string> annotationsType;
	annotationsType annotations;

public:
	noteAnnotate() 
		: super() {}

    noteAnnotate( std::basic_ostream<charT,traitsT>& o )
		: super(o) {}

   void newline(const char *line, int first, int last );
 
};


/* Annotate source with code coverage information
 */
template<typename charT, typename traitsT = std::char_traits<charT> >
class rangeAnnotate  : public basicAnnotate<charT,traitsT> {
public:
	typedef std::vector<int> rangesType;

protected:
    typedef basicAnnotate<charT,traitsT> super;
	rangesType ranges;

public:
	rangeAnnotate()
		: super() {}
    
    rangeAnnotate( std::basic_ostream<charT,traitsT>& o )
		: super(o) {
	}

   void newline(const char *line, int first, int last );
};

typedef basicDecorator<char> decorator;
typedef basicDecoratorChain<char> decoratorChain;
typedef basicHtmlEscaper<char> htmlEscaper;
typedef basicLinkLight<char> linkLight;
typedef absUrlDecoratorBase<char> absUrlDecorator;
typedef cachedUrlBase<char> cachedUrlDecorator;
typedef basicHrefLight<char> hrefLight;
typedef basicCppLight<char> cppLight;
typedef noteAnnotate<char> noteDecorator;
typedef rangeAnnotate<char> rangeDecorator;

}

#include "decorator.tcc"


#endif

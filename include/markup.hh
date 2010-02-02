/* Copyright (c) 2009, Sebastien Mirolo
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

   THIS SOFTWARE IS PROVIDED BY Sebastien Mirolo ''AS IS'' AND ANY
   EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL Sebastien Mirolo BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#ifndef guardmarkup
#define guardmarkup

#include <ostream>

namespace detail {

    class nodeEnd {
    public:
	const char* name;

	explicit nodeEnd( const char* n ) : name(n) {}

	template<typename ch, typename tr>
	friend std::basic_ostream<ch, tr>&
	operator<<(std::basic_ostream<ch, tr>& ostr, const nodeEnd& v ) {
	    ostr << "</" << v.name << '>'
		 << std::endl;
	    return ostr;
	}
    };

    class attribute {
    public:
	bool valid;
	const char* name;
	std::string value;

	explicit attribute( const char* n ) 
	    : valid(false), name(n) {}

	attribute( const char* n, const std::string& v ) 
	    : valid(true), name(n), value(v) {}
	
	template<typename ch, typename tr>
	friend std::basic_ostream<ch, tr>&
	operator<<(std::basic_ostream<ch, tr>& ostr, const attribute& v ) {
	    if( v.valid ) {
		ostr << ' ' << v.name << "=\"" << v.value << '\"';
	    }
	    return ostr;
	}
    };
    
    class markup {
    protected:
	bool newline;
	const char *name;

	const size_t attrArrayLength;
	const char **attrNames;
	std::string *attrValues;

   public:		
	markup( const char *n, 
		const char **attrNs,
		std::string *attrVs,
		const size_t attrNbs,
		bool nl = false ) : newline(nl), name(n), 
				    attrNames(attrNs), attrValues(attrVs), 
				    attrArrayLength(attrNbs) {}
	
	template<typename ch, typename tr>
	friend std::basic_ostream<ch, tr>&
	operator<<(std::basic_ostream<ch, tr>& ostr, const markup& v ) {
	    ostr << "<" << v.name;
	    if( v.attrNames != NULL & v.attrValues != NULL ) {
		for( size_t i = 0; i < v.attrArrayLength; ++i ) {
		    if( !v.attrValues[i].empty() ) {
			ostr << ' ' << v.attrNames[i] << "=\"" << v.attrValues[i] << '\"';
		    }
		}
	    }
	    ostr << '>';
	    if( v.newline ) ostr << std::endl;
	    return ostr;
	}
    };

} // detail

namespace html {

    /** HTML a (href) markup
     */
    class a : public detail::markup {    
    protected:
	enum attributes {
	    hrefAttr,
	    titleAttr
	};

	static const size_t attrLength = 2;

	static const char *attrNames[];
	std::string attrValues[attrLength];


    public:  
	static const char* name;
	static const detail::nodeEnd end;
    
	a() : markup(name,attrNames,attrValues,attrLength) {}

	a& href( const std::string& v ) {
	    attrValues[hrefAttr] = v;
	    return *this;
	}

	a& title( const std::string& v ) {
	    attrValues[titleAttr] = v;
	    return *this;
	}
    };


    /** HTML div markup
     */
    class div : public detail::markup {    
    protected:  
	enum attributes {
	    classAttr,
	};

	static const size_t attrLength = 1;

	static const char *attrNames[];
	std::string attrValues[attrLength];

	
    public:  
	static const char* name;
	static const detail::nodeEnd end;
    
	div() : markup(name,attrNames,attrValues,attrLength,true) {}

	div& classref( const char *v ) {
	    attrValues[classAttr] = v;
	    return *this;
	}
    };

    /** HTML h{1,2,3,...} markup
     */
    class h : public detail::markup {    
    protected:
	static const char* names[];

	int num;

    public:
	/* \todo assert than n < 5. */
	explicit h( int n ) : markup(names[n],NULL,NULL,0), num(n) {}

	detail::nodeEnd end() const {
	    return detail::nodeEnd(names[num]);
	}

    };

    /** HTML p markup
     */
    class p : public detail::markup {    
    public:  
	static const char* name;
	static const detail::nodeEnd end;
    
	p() : markup(name,NULL,NULL,0,true) {}
    };

} // namespace html

/** RSS author markup
 */
class author : public detail::markup {    
public:  
    static const char* name;
    static const detail::nodeEnd end;
    
    author() : markup(name,NULL,NULL,0) {}
};


/** channel markup
 */
class channel : public detail::markup {    
public:  
    static const char* name;
    static const detail::nodeEnd end;
    
    channel() : markup(name,NULL,NULL,0,true) {}
};


/** description markup
 */
class description : public detail::markup {    
public:  
    static const char* name;
    static const detail::nodeEnd end;
    
    description() : markup(name,NULL,NULL,0,true) {}
};


/** guid into a rss feed
 */
class guid : public detail::markup {    
public:  
    static const char* name;
    static const detail::nodeEnd end;
    
    guid() : markup(name,NULL,NULL,0) {}
};


/** item into a rss feed
 */
class item : public detail::markup {    
public:  
    static const char* name;
    static const detail::nodeEnd end;
    
    item() : markup(name,NULL,NULL,0,true) {}
};


/** link into a rss feed
 */
class rsslink : public detail::markup {    
public:  
    static const char* name;
    static const detail::nodeEnd end;
    
    rsslink() : markup(name,NULL,NULL,0) {}
};


/** pre markup
 */
class pre : public detail::markup {    
public:  
    static const char* name;
    static const detail::nodeEnd end;
    
    pre() : markup(name,NULL,NULL,0,true) {}
};


/** a source code listing markup
 */
class code : public pre {
public:  
    code() {}
    
    template<typename ch, typename tr>
    friend std::basic_ostream<ch, tr>&
    operator<<(std::basic_ostream<ch, tr>& ostr, const code& v ) {
	ostr << "<pre class=\"code\">" << std::endl;
	return ostr;
    }
};


/** RSS pubDate markup
 */
class pubDate : public detail::markup {    
public:  
    static const char* name;
    static const detail::nodeEnd end;
    
    pubDate() : markup(name,NULL,NULL,0) {}
};


/** rss markup
 */
class rss : public detail::markup {    
protected:
    enum attributes {
	versionAttr,
    };
    
    static const size_t attrLength = 1;
    
    static const char *attrNames[];
    std::string attrValues[attrLength];
    

public:  
    static const char* name;
    static const detail::nodeEnd end;
    
    rss() : markup(name,attrNames,attrValues,attrLength,true) {}

    rss& version( const char *v ) {
	attrValues[versionAttr] = v;
	return *this;
    }
};


/** title markup
 */
class title : public detail::markup {    
public:  
    static const char* name;
    static const detail::nodeEnd end;
    
    title() : markup(name,NULL,NULL,0) {}
};

#endif
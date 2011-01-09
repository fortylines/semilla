/* Copyright (c) 2009, Fortylines LLC
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

#ifndef guardchangelist
#define guardchangelist

#include "composer.hh"
#include "post.hh"

/* Different displays of changes to an underlying source control repository.

   Primary Author(s): Sebastien Mirolo <smirolo@fortylines.com>
*/

class historyref {
public:
    historyref() {}

    virtual url asUrl( const boost::filesystem::path& doc, 
		       const std::string& rev ) const = 0;
};

class diffref : public historyref {
public:
    diffref() {}

    virtual url asUrl( const boost::filesystem::path& doc, 
		       const std::string& rev ) const;
};

class checkinref : public historyref {
public:
    checkinref() {}

    virtual url asUrl( const boost::filesystem::path& doc, 
		       const std::string& rev ) const;
};

class checkin : public post {
public:
    typedef std::list<boost::filesystem::path> fileSet;

public:
    fileSet files;

    checkin() {}

    void addFile( const boost::filesystem::path& pathname ) {
	files.push_back(pathname);
    }

};


/** History of checkins on a project
 */
class history {
public:
    typedef std::list<checkin> checkinSet;
    
public:
    checkinSet checkins;

    checkin* add() { 
	checkin ci;
	checkins.push_back(ci);
	return &checkins.back();
    }
};


/** Base class definition for interacting with a source code control system.    
 */
class revisionsys {
public:
    typedef std::vector<revisionsys*> revsSet;
    static revsSet revs;

    boost::filesystem::path rootpath;

    /** Directory name in which revision control meta information 
	is stored (example: .git). 
    */
    const char* metadir;

public:
    explicit revisionsys( const char* m ) 
	: metadir(m) { revs.push_back(this); }

    /** Create a new repository from directory *pathname*. When *group*
	is true, all users in the same group as the creator have permissions
	to push changes into the repository otherwise only the creator
	can push changes.
     */
    virtual void create( const boost::filesystem::path& pathname,
			 bool group = false ) = 0;

    /** Add *pathname* to the changelist to be committed. 
     */
    virtual void add( const boost::filesystem::path& pathname ) = 0;

    /** Commit the default changelist.
     */
    virtual void commit( const std::string& msg ) = 0;

    virtual void diff( std::ostream& ostr, 
		       const std::string& leftCommit, 
		       const std::string& rightCommit, 
		       const boost::filesystem::path& pathname ) = 0;
    
    virtual void history( std::ostream& ostr,
			  const session& s, 
			  const boost::filesystem::path& pathname,
			  historyref& r ) = 0;
    
    virtual void checkins( ::history& hist,
			   const session& s, 
			   const boost::filesystem::path& pathname ) = 0;
    
    /** returns the revision system associated with a pathname 
     */
    static revisionsys*
    findRev( session& s, const boost::filesystem::path& pathname );

};


/** interaction with a git repository.
 */
class gitcmd : public revisionsys {
protected:
    boost::filesystem::path executable;

    /** Execute a command line catching stdout such that apache does
	not end-up with malformed headers and throwing an exception
	when the command fails.
     */
    void shellcmd( const std::string& cmdline );

public:
    gitcmd( const boost::filesystem::path& exec ) 
	: revisionsys(".git"), executable(exec) {}
   
    void create( const boost::filesystem::path& pathname,
		 bool group = false );

    void add( const boost::filesystem::path& pathname );

    void commit( const std::string& msg );

    void checkins( ::history& hist,
		   const session& s,
		   const boost::filesystem::path& pathname );

    void diff( std::ostream& ostr, 
	       const std::string& leftCommit, 
	       const std::string& rightCommit, 
	       const boost::filesystem::path& pathname );
    
    void history( std::ostream& ostr, 
		  const session& s, 
		  const boost::filesystem::path& pathname,
		  historyref& r );	
}; 


/** Command to cancel web edits
*/
class cancel : public document {
public:
    explicit cancel( std::ostream& o ) : document(o) {}

    virtual void fetch( session& s, const boost::filesystem::path& pathname );
};


/* Add a changed file to the default change list 
*/
class change : public document {
public:
    explicit change( std::ostream& o ) : document(o) {}

    static void 
    addSessionVars( boost::program_options::options_description& opts );

    virtual void fetch( session& s, const boost::filesystem::path& pathname );
};


/** Diff between two revisions of a file under revision control
 */
class changediff : public composer {
protected:
	
    /** Embed the content of a variable into a page. 
     */
    virtual void embed( session& s, const std::string& value );

public:
    changediff( std::ostream& o, const boost::filesystem::path& f ) 
	: composer(o,f,error) {}

};


/** Base class for commands displaying changelists.
 */
class changelist : public document {
protected:
    
public:
    explicit changelist( std::ostream& o )
	: document(o) {}


};

/** Detailed description of a single changelist
 */
class changedescr : public changelist {
protected:

public:
    explicit changedescr( std::ostream& o ) 
	: changelist(o) {}

    virtual void fetch(	session& s, const boost::filesystem::path& pathname );
};


/** Short history of changes to a project under revision control. 

    This will generate the same output as changehistory but with hyperlinks
    to an expended description of the checkin instead of a difference between
    the current file and the revision.
 */
class changecheckin : public changelist {
public:
    explicit changecheckin( std::ostream& o ) : changelist(o) {}

    virtual void fetch( session& s, const boost::filesystem::path& pathname );	
};


/** History of changes to a file under revision control
 */
class changehistory : public changelist {
public:
    explicit changehistory( std::ostream& o ) : changelist(o) {}

    virtual void fetch( session& s, const boost::filesystem::path& pathname );	
};


/** RSS of changes to a file under revision control
 */
class changerss : public changelist {
public:
    explicit changerss( std::ostream& o ) : changelist(o) {}

    virtual void fetch(	session& s, const boost::filesystem::path& pathname );	
};


#endif

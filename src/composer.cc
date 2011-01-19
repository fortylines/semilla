/* Copyright (c) 2009-2011, Fortylines LLC
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

#include <iostream>
#include <boost/regex.hpp>
#include <boost/filesystem/fstream.hpp>
#include "composer.hh"

/** Document composition

    Primary Author(s): Sebastien Mirolo <smirolo@fortylines.com>
*/

void 
composer::addSessionVars( boost::program_options::options_description& opts )
{
    using namespace boost::program_options;
    
    options_description vars("composer");
    vars.add_options()
	("document",value<std::string>(),"document")
	("themeDir",value<std::string>(),"path to user interface elements");
    opts.add(vars);
}


void composer::embed( session& s, const std::string& value ) const {
    using namespace boost::filesystem;
    try {
	dispatchDoc::instance->fetch(s,value);
    } catch( const basic_filesystem_error<path>& e ) {
#if 0
	/* \todo s.out not restored correctly. */
	s.out() << "<p>" << e.what() << "</p>" << std::endl;
#else
	std::cerr << "[embed of '" << value << "']: " 
		  << e.what() << std::endl;	
#endif	
    }
}


void composer::fetch( session& s, const boost::filesystem::path& pathname ) const
{
    using namespace boost;
    using namespace boost::system;
    using namespace boost::filesystem;

    static const boost::regex tmplname("<!-- tmpl_name '(\\S+)' -->");
    static const boost::regex tmplvar("<!-- tmpl_var name='(\\S+)' -->");
    static const boost::regex tmplinc("<!-- tmpl_include name='(\\S+)' -->");
    
    ifstream strm;
    open(strm,fixed.empty() ? pathname : fixed);

    skipOverTags(s,strm);
    while( !strm.eof() ) {
	smatch m;
	std::string line;
	bool found = false;
	std::getline(strm,line);
	if( regex_search(line,m,tmplname) ) {
	    std::string varname = m.str(1);
	    session::variables::const_iterator v = s.vars.find(varname);
	    s.out() << m.prefix();
	    if( v != s.vars.end() ) {		
		s.out() << v->second.value;		
	    } else {
		s.out() << varname << " not found!";
	    }
	    s.out() << m.suffix() << std::endl;
	    found = true;
	}

	if( regex_search(line,m,tmplvar) ) {
	    found = true;
	    s.out() << m.prefix();
	    std::string varname = m.str(1);
	    /* Hack! duplicate code from dispatchDoc::fetch() - see below. */
	    session::variables::const_iterator v = s.vars.find(varname);
	    if( v == s.vars.end() ) {    
		s.vars[varname] = s.valueOf("document");
	    }
	    const document* doc 
		= dispatchDoc::instance->select(varname,s.valueOf(varname));
	    if( doc != NULL ) {
		boost::filesystem::path docname = s.valueAsPath(varname);
		/* \todo code could be:
		           doc->fetch(s,docname);  
			 but that would skip over the override 
			 of changediff::embed(). */
		embed(s,varname);					    
	    } else {
		s.out() << s.valueOf(varname);
	    }
	    s.out() << m.suffix() << std::endl;
	} else if( regex_search(line,m,tmplinc) ) {
	    /* \todo fetch another template. This code should
	     really call to the dispatcher once we can sort
	     out varnames and pathnames... */
	    path incpath((fixed.empty() ? pathname.parent_path() 
			  : fixed.parent_path()) / m.str(1));
	    const document* doc = dispatchDoc::instance->select("document",incpath.string());
	    if( doc != NULL ) {
		doc->fetch(s,incpath);
	    }
	    found = true;
	}
	if( !found ) {
	    s.out() << line << std::endl;
	}
    }
    strm.close();
}

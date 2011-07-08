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

#include <unistd.h>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "auth.hh"
#include "feeds.hh"
#include "changelist.hh"
#include "composer.hh"
#include "docbook.hh"
#include "project.hh"
#include "logview.hh"
#include "checkstyle.hh"
#include "calendar.hh"
#include "comments.hh"
#include "contrib.hh"
#include "todo.hh"
#include "blog.hh"
#include "webserve.hh"
#include "payment.hh"
#include "cppfiles.hh"
#include "shfiles.hh"

/** Main executable

    Primary Author(s): Sebastien Mirolo <smirolo@fortylines.com>
*/


#ifndef CONFIG_FILE
#error "CONFIG_FILE should be defined when compiling this file"
#endif
#ifndef SESSION_DIR
#error "SESSION_DIR should be defined when compiling this file"
#endif
#ifndef VERSION
#error "VERSION should be defined when compiling this file"
#endif

const char* session::configFile = CONFIG_FILE;
const char* session::sessionDir = SESSION_DIR;

char except[] = "exception";
extern dispatchDoc semDocs;

int main( int argc, char *argv[] )
{
    using namespace std;
    using namespace boost::program_options;
    using namespace boost::filesystem;

    std::stringstream mainout;
    session s("semillaId",mainout);
    s.privileged(false);

    try {
	/* parse command line arguments */
	options_description opts;
	options_description visible;
	options_description genOptions("general");
	genOptions.add_options()
	    ("help","produce help message");	
	opts.add(genOptions);
	visible.add(genOptions);
	session::addSessionVars(opts,visible);
	authAddSessionVars(opts,visible);
	changelistAddSessionVars(opts,visible);
	composerAddSessionVars(opts,visible);
	postAddSessionVars(opts,visible);
	projectAddSessionVars(opts,visible);
	calendarAddSessionVars(opts,visible);
	commentAddSessionVars(opts,visible);

	s.restore(argc,argv,opts);
	
	if( !s.runAsCGI() ) {
	    bool printHelp = false;
	    if( argc <= 1 ) {
		printHelp = true;
	    } else {
		for( int i = 1; i < argc; ++i ) {
		    if( strncmp(argv[i],"--help",6) == 0 ) {
			printHelp = true;
		    }
		}
	    }
	    if( printHelp ) {
		boost::filesystem::path binname 
		    = boost::filesystem::basename(argv[0]);
		cout << binname << "[options] pathname" << endl << endl
		     << "Version" << endl
		     << "  " << binname << " version " << VERSION << endl << endl;
		cout << "Options" << endl
		     << opts << endl;
		cout << "Further Documentation" << endl
		     << "Semilla relies on session variables (ex. *siteTop*)"
		     << " to find relevent pieces of information to build"
		     << " a page."
		     << " Session variables can be defined as:\n"
		     << "   * Arguments on the command-line\n"
		     << "   * Name=Value pairs in a global configuration file\n"
		     << "   * Parameters passed through CGI environment "
		     << "variables\n"
		     << "   * Name=Value pairs in a unique session file\n"
		     << "When a variable is defined in more than one set, "
		 << "command-line arguments have precedence over the global "
		     << "configuration file which has precedence over"
		     << " environment variables which in turn as precedence"
		     << " over the session file." << endl;
		return 0;
	    }
	}

	/* by default bring the index page */
	if( document.value(s).string() == "/"
	    && boost::filesystem::exists(siteTop.value(s) 
					 / std::string("index.html")) ) {
	    cout << httpHeaders.location(url("index.html"));		       
	    
	} else {	  
	    semDocs.fetch(s,document.name,document.value(s));
	    if( s.runAsCGI() ) {
		cout << httpHeaders
		    .contentType()
		    .status(s.errors() ? 404 : 0);
	    }
	    cout << mainout.str();
	}

    } catch( exception& e ) {
	try {
	    std::cerr << e.what() << std::endl;
	    s.insert("exception",e.what());
	    compose<except>(s,document.name);
	} catch( exception& e ) {
	    /* Something went really wrong if we either get here. */
	    cout << httpHeaders.contentType().status(404);
	    cout << "<html>" << endl;
	    cout << html::head() << endl
		 << "<title>It is really bad news...</title>" << endl
		 << html::head::end << endl;
	    cout << html::body() << endl << html::p() << endl
		 << "caught exception: " << e.what() << endl
		 << html::p::end << endl << html::body::end << endl;
	    cout << "</html>" << endl;
	}
	++s.nErrs;
    }

    return s.errors();
}

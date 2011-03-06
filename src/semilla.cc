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
#include "projfiles.hh"
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

const char* session::configFile = CONFIG_FILE;
const char* session::sessionDir = SESSION_DIR;


struct callEntry {
    const char *name;
    boost::regex pat;
    callFetchType behavior;
    callFetchFunc callback;
};

char todoExt[] = "todo";
char corpExt[] = "corp";
char rssExt[] = "rss";
char blogExt[] = "blog";
char project[] = "project";
char document[] = "document";
char todos[] = "todos";
char blogPat[] = ".*\\.blog";
char source[] = "source";
char genericDoc[] = "document";
char title[] = "title";
char feed[] = "feed";
char indexPage[] = "index";
char buildView[] = "Build View";

std::string active("contrib/todos/active/");


/* The pattern need to be inserted in more specific to more 
   generic order since the matcher will apply each the first
   one that yields a positive match. */
callEntry entries[] = {
#if 0
    { "view", boost::regex("/cancel"), always, cancelFetch },
    { "view", boost::regex("/edit"), always, composerFetch<"edit.ui"> },
    { "view", boost::regex("/login"),always, loginFetch },
    { "view", boost::regex("/logout"),always, logoutFetch },
    { "view", boost::regex("/save"),always, changeFetch },
#endif

    /* Login and Logout pages generates HTML to be displayed
       in a web browser. It is very often convinient to quickly
       start and stop recording worked hours from the command line.
       In that case, "work" and "rest" can be used as substitute. */
    { "view", boost::regex("work"), always, authFetch },
    { "view", boost::regex("rest"), always, deauthFetch },
    
    /* If a template file matching the document's extension
       is present in the theme directory, let's use it
       as a composer. */
    { "view", boost::regex(".*\\.todo"), always, composerFetch<todoExt> },
    { "view", boost::regex(".*\\.corp"), always, composerFetch<corpExt> },
    { "view", boost::regex(".*\\.rss"), always, composerFetch<rssExt> },
    { "view", boost::regex(".*\\.blog"), always, composerFetch<blogExt> },

    /* The build "document" gives an overview of the set 
       of all projects at a glance and can be used to assess 
       the stability of the whole as a release candidate. */
    { "document", boost::regex(".*/log/"), always, logviewFetch },

    /* A project dws.xml "document" file show a description,
       commits and unit test status of a single project through 
       a project "view". */
    { "regressions", boost::regex(".*regression\\.log"), always, regressionsFetch },

    { "check", boost::regex(".*\\.c"), whenFileExist, checkfileFetch<cppChecker> },
    { "check", boost::regex(".*\\.h"), whenFileExist, checkfileFetch<cppChecker> },
    { "check", boost::regex(".*\\.cc"), whenFileExist, checkfileFetch<cppChecker> },

    { "check", boost::regex(".*\\.hh"), whenFileExist, checkfileFetch<cppChecker> },
    { "check", boost::regex(".*\\.tcc"), whenFileExist, checkfileFetch<cppChecker> },

    { "document", boost::regex(".*\\.c"), whenFileExist, cppFetch },
    { "document", boost::regex(".*\\.h"), whenFileExist, cppFetch },
    { "document", boost::regex(".*\\.cc"), whenFileExist, cppFetch },
    { "document", boost::regex(".*\\.hh"), whenFileExist, cppFetch },
    { "document", boost::regex(".*\\.tcc"), whenFileExist, cppFetch },

    { "document", boost::regex(".*\\.c/diff"), always, cppDiff },
    { "document", boost::regex(".*\\.h/diff"), always, cppDiff },
    { "document", boost::regex(".*\\.cc/diff"), always, cppDiff },
    { "document", boost::regex(".*\\.hh/diff"), always, cppDiff },
    { "document", boost::regex(".*\\.tcc/diff"), always, cppDiff },

    { "view", boost::regex(".*\\.c"), whenFileExist, composerFetch<source> },
    { "view", boost::regex(".*\\.h"), whenFileExist, composerFetch<source> },
    { "view", boost::regex(".*\\.cc"), whenFileExist, composerFetch<source> },
    { "view", boost::regex(".*\\.hh"), whenFileExist, composerFetch<source> },
    { "view", boost::regex(".*\\.tcc"), whenFileExist, composerFetch<source> },

    { "check", boost::regex(".*\\.mk"), whenFileExist, checkfileFetch<shChecker> },
    { "check", boost::regex(".*\\.py"), whenFileExist, checkfileFetch<shChecker> },
    { "check", boost::regex(".*Makefile"), whenFileExist, checkfileFetch<shChecker> },

    { "document", boost::regex(".*\\.mk"), whenFileExist, shFetch },
    { "document", boost::regex(".*\\.py"), whenFileExist, shFetch },
    { "document", boost::regex(".*Makefile"), whenFileExist, shFetch },

    { "document", boost::regex(".*\\.mk/diff"), always, shDiff },
    { "document", boost::regex(".*\\.py/diff"), always, shDiff },
    { "document", boost::regex(".*Makefile/diff"), always, shDiff },

    { "view", boost::regex(".*\\.mk"), whenFileExist, composerFetch<source> },
    { "view", boost::regex(".*\\.py"), whenFileExist, composerFetch<source> },
    { "view", boost::regex(".*Makefile"), whenFileExist, composerFetch<source> },

    /* Command to create a new project */
    { "view", boost::regex(".*/reps/.*/create"), always, projCreateFetch },
    { "history", boost::regex(".*dws\\.xml"), always, feedRepository<htmlwriter> },
    { "document", boost::regex(".*dws\\.xml"),always, projindexFetch },

    /* Composer for a project view */
    { "view", boost::regex(".*dws\\.xml"), always, composerFetch<project> },

    /* Widget to generate a rss feed. Attention: it needs 
       to be declared before any of the todoFilter::viewPat 
       (i.e. todos/.+) since an rss feed exists for todo items
       as well. */	    
    { "document", boost::regex(".*\\.git/index\\.rss"), always,
      feedRepository<rsswriter> },
    { "document", boost::regex(".*/index\\.rss"), always,
      feedAggregateFetch<rsswriter,document> },

    /* Composer and document for the todos index view */
    { "view", boost::regex(".*todos/.+"), always, composerFetch<todos> },

    { "document", boost::regex(std::string(".*") + active), always,
      todoIndexWriteHtmlFetch },

    { "document", boost::regex("/todos/create"), always, todoCreateFetch },
#if 0
    { "view", boost::regex(".*\\.todo/comment"), always, todocomment },
    { "document", boost::regex(".*\\.todo/comment"),always, todocomment },
    { "document", boost::regex(".*\\.todo/voteAbandon"), always, todoVoteAbandonFetch },
    { "document", boost::regex(".*\\.todo/voteSuccess"), always, todoVoteSuccess },
#endif
    /* comments */
    { "view", boost::regex(std::string("/comments/create.*")), always, commentPageFetch },

    /* blog presentation */ 
    { "view", boost::regex(".*/blog/.*"), always, composerFetch<blogExt> },
    { "document", boost::regex(".*/blog/tags/.*"), always, blogByIntervalTags },
    { "document", boost::regex(".*/blog/archive/.*"), always, blogByIntervalDate },
    { "document", boost::regex(".*/blog/.*"), always, blogByIntervalDate },
    { "dates", boost::regex(".*/blog/.*"), always, blogDateLinks<blogPat> },
    { "tags", boost::regex(".*/blog/.*"), always, blogTagLinks<blogPat> },

    /* contribution */
    { "document", boost::regex(".*contrib/"), always, contribIdxFetch },
    { "document", boost::regex(".*contrib/create"), always, contribCreateFetch },
    
    { "document", boost::regex(".*\\.ics"), whenFileExist, calendarFetch },
    
    /* Source code "document" files are syntax-highlighted 
       and presented inside a source.template "view" */    
    { "view", boost::regex(".*/diff"), always, composerFetch<source> },
    
    { "document", boost::regex(".*\\.eml"), always, mailParserFetch },
    { "document", boost::regex(".*\\.todo"), always, todoWriteHtmlFetch },

    /* We transform docbook formatted text into HTML for .book 
       and .corp "document" files and interpret all other unknown 
       extension files as raw text. In all cases we use a default
       document.template interface "view" to present those files. */ 
    { "document", boost::regex(".*\\.book"), whenFileExist, docbookFetch },
    { "document", boost::regex(".*\\.corp"), whenFileExist, docbookFetch },

    /* \todo !!! Hack for current tmpl_include implementation */
    { "document", boost::regex(".*\\.template"), whenFileExist, formattedFetch },

    { "document", boost::regex(".*"), whenFileExist, textFetch },

    /* Load title from the meta tags in a text file. */
    { "title",boost::regex(".*/log/"),whenFileExist,consMeta<buildView> },
    { "title", boost::regex(".*\\.book"), whenFileExist, docbookMeta },
    { "title", boost::regex(".*\\.corp"), whenFileExist, docbookMeta },
    { "title", boost::regex(".*\\.todo"), whenFileExist, todoMeta },
    { "title", boost::regex(".*\\.template"), whenFileExist, textMeta<title> },
    { "title", boost::regex(".*"), whenFileExist,metaFetch<title> },

    /* homepage */
    { "feed", boost::regex(".*\\.git/index\\.feed"), always, feedRepositoryPopulate },
    { "feed", boost::regex("^/index\\.feed"),always, htmlSiteAggregate<feed> },
    { "view", boost::regex("/"),always, composerFetch<indexPage> },

    /* default catch-all */
    { "view",boost::regex(".*"), always, composerFetch<genericDoc> },

    /* Widget to display status of static analysis of a project 
       source files in the absence of a more restrictive pattern. */
    { "checkstyle", boost::regex(".*"), always, checkstyleFetch },

    /* Widget to display the history of a file under revision control
       in the absence of a more restrictive pattern. */	   
    { "history", boost::regex(".*"), always, changehistoryFetch },

    /* Widget to display a list of files which are part of a project.
       This widget is used through different "view"s to browse 
       the source repository. */
    { "projfiles", boost::regex(".*"), always, projfilesFetch },

    /* button to Amazon payment */    
    { "payproc", boost::regex(".*"), always, paymentFetch },
    
};




int main( int argc, char *argv[] )
{
    using namespace std;
    using namespace boost::program_options;
    using namespace boost::filesystem;

    std::stringstream mainout;
    session s("view","semillaId",mainout);
    s.privileged(false);

    try {
	/* parse command line arguments */
	options_description opts;
	options_description genOptions("general");
	genOptions.add_options()
	    ("help","produce help message")
	    ("binDir",value<std::string>(),"path to outside executables");
	
	opts.add(genOptions);
	auth::addSessionVars(opts);
	change::addSessionVars(opts);
	composer::addSessionVars(opts);
	post::addSessionVars(opts);
	logview::addSessionVars(opts);
	projindex::addSessionVars(opts);
	calendar::addSessionVars(opts);
	commentPage::addSessionVars(opts);

#if 0
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
	    cout << opts << endl;
	    cout << "1. Environment variables\n"
		 << "2. Command-line arguments\n"
		 << "3. Session database\n"
		 << "4. Configuration file\n";
	    return 0;
	}
#endif

	s.restore(argc,argv,opts);

	/* If no document is present, set document 
	   as view in order to catch default dispatch 
	   clause. */
	session::variables::const_iterator doc = s.vars.find("document");
	if( doc == s.vars.end() ) {
	    s.insert("document",s.valueOf("view"));
	}
 
	/* by default bring the index page */
	if( (s.valueOf("view").empty() || s.valueOf("view") == "/") 
	    && boost::filesystem::exists(s.valueOf("siteTop") 
					 + std::string("/index.html")) ) {
	    cout << httpHeaders.location(url("index.html"));		       
	    
	} else {	    		       
	    dispatchDoc docs;

	    boost::filesystem::path regressname
		= s.valueOf("siteTop") 
		/ boost::filesystem::path("log/tests")
		/ (boost::filesystem::path(s.valueOf("document")).parent_path().filename() 
		   + std::string("-test/regression.log"));
	    s.vars["regressions"] = regressname.string();
#if 0
	    s.vars["dates"] = s.valueOf("document");
	    s.vars["tags"] = s.valueOf("document");
	    s.vars["checkstyle"] = s.valueOf("document");
	    s.vars["history"] = s.valueOf("document");
	    s.vars["projfiles"] = s.valueOf("document");
	    s.vars["payproc"] = s.valueOf("document");
#endif     
	    /* homepage */
	    s.vars["feed"] = session::valT("/index.feed");

	    for( callEntry *e = entries; 
		 e != &entries[sizeof(entries)/sizeof(entries[0])];
		 ++e ) {
		docs.add(e->name,e->pat,e->callback,e->behavior);
	    }
     	    
	    docs.fetch(s,"view");
	    if( s.runAsCGI() ) {
		std::cout << httpHeaders
		    .contentType()
		    .status(s.errors() ? 404 : 0);
	    }
	    std::cout << mainout.str();
	}
#if 0
	/* find a way to get .template in main() top scope. */
    } catch( exception& e ) {

	try {
	    std::cerr << "caught exception: " << e.what() << std::endl;
	    s.vars["exception"] = e.what();
	    char except[] = "exception";
	    composerFetch<except>(s,s.valueOf("document"));
#endif
	} catch( exception& e ) {
	    /* Something went really wrong if we either get here. */
	    cout << httpHeaders.contentType().status(404);
	    cout << "<html>" << endl;
	    cout << "<head>" << endl;
	    cout << "<title>It is really bad news...</title>" << endl;
	    cout << "</head>" << endl;
	    cout << "<body>" << endl;
	    cout << "<p>" << endl;
	    cout << "caught exception: " << e.what() << endl;
	    cout << "</p>" << endl;
	    cout << "</body>" << endl;
	    cout << "</html>" << endl;
#if 0
	}
#endif
	++s.nErrs;
    }

    return s.errors();
}

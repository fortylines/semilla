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


#include <sys/types.h>
#include <unistd.h>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/regex.hpp>
#include "session.hh"
#include <uriparser/Uri.h>
#include "markup.hh"

/** Session manager

    Primary Author(s): Sebastien Mirolo <smirolo@fortylines.com>
*/

pathVariable document("view","document to be displayed");

pathVariable siteTop("siteTop","root directory that contains the web site");

/* \todo Unless we communicate a callback to another site,
         we should be clean of requiring domainName... 
	 not true: We need domain name to display fully qualified src 
	 repository urls.
*/
urlVariable domainName("domainName","domain name of the web server");

timeVariable startTime("startTime","start time for the session");


undefVariableError::undefVariableError( const std::string& varname ) 
    : std::runtime_error(std::string("undefined variable in session ") 
			 + varname) {}


boost::shared_ptr<boost::program_options::option_description> 
sessionVariable::option() { 
    return boost::shared_ptr<boost::program_options::option_description> 
	(new boost::program_options::option_description(opt)); 
}


std::string sessionVariable::value( const session& s ) const
{
    return s.valueOf(name);
}


int intVariable::value( const session& s ) const
{
    return atoi(s.valueOf(name).c_str());
}


boost::filesystem::path pathVariable::value( const session& s ) const
{
    using namespace boost::filesystem;

#if 0
    path absolute = s.abspath(sessionVariable::value(s));
#if 0
    /* if we do that, we cannot generate dynamic pages. On the other
     hand using sessionVariable instead of pathVariable for *document*
     will not check it is an actual pathname. */
    if( !boost::filesystem::exists(absolute) ) {
	using namespace boost::system::errc;
	boost::throw_exception(			       
	  basic_filesystem_error<path>(std::string("path not found"),
				absolute, 
			       	make_error_code(no_such_file_or_directory)));
    }
#endif
    return absolute;
#else
    /* We cannot return an absolute path here, else it will mess up 
       the dispatch pattern matcher. */
    return sessionVariable::value(s);
#endif
}

boost::posix_time::ptime timeVariable::value( const session& s ) const 
{
    std::stringstream is(sessionVariable::value(s));
    boost::posix_time::ptime t;
    is >> t;
    return t;
}


url urlVariable::value( const session& s ) const 
{
    return url(sessionVariable::value(s));
}


std::string session::sessionName;


void 
session::addSessionVars( boost::program_options::options_description& all,
			 boost::program_options::options_description& visible ) 
{
    using namespace boost::program_options;

    options_description localOptions("session");
    localOptions.add_options()
	("config",value<std::string>(),(std::string("path to the configuration file (defaults to ") + configFile + std::string(")")).c_str())
	("sessionDir",value<std::string>(),(std::string("directory where session files are stored (defaults to ") + sessionDir + std::string(")")).c_str())
	(sessionName.c_str(),value<std::string>(),"name of the session id variable (or cookie)");
    localOptions.add(domainName.option());
    localOptions.add(siteTop.option());
    all.add(localOptions);
    visible.add(localOptions);

    options_description hiddenOptions("hidden");
    hiddenOptions.add(startTime.option());
    hiddenOptions.add(document.option());
    all.add(hiddenOptions);
}


void session::load( const boost::program_options::options_description& opts,
		    const boost::filesystem::path& p, sourceType st )
{
    using namespace boost;
    using namespace boost::system;
    using namespace boost::filesystem;
    using namespace boost::program_options;

    if( boost::filesystem::exists(p) ) {
	variables_map params;
	ifstream istr(p);
	if( istr.fail() ) {
	    boost::throw_exception(
				   system_error(error_code(),
						std::string("file not found")+ p.string()));
	}

	boost::program_options::store(parse_config_file(istr,opts,true),
				      params);
	
	for( variables_map::const_iterator param = params.begin(); 
	     param != params.end(); ++param ) {
	    if( vars.find(param->first) == vars.end() ) {
		insert(param->first,param->second.as<std::string>(),st);
	    }
	}
    }
}


void session::check( const boost::filesystem::path& pathname ) const
{
    if( !boost::filesystem::exists(pathname)
	|| !is_regular_file(pathname) ) {
	using namespace boost::system::errc;
	using namespace boost::filesystem;
 
	boost::throw_exception(boost::system::system_error(
	       make_error_code(no_such_file_or_directory),pathname.string()));	
    }
}

void session::appendfile( boost::filesystem::ofstream& strm, 
			  const boost::filesystem::path& pathname ) {
    using namespace boost::system;
    using namespace boost::filesystem;

    if( !boost::filesystem::exists(pathname) ) {
	createfile(strm,pathname);
    } else {
	using namespace boost::system::errc;

	strm.open(pathname,std::ios_base::out | std::ios_base::app);
	if( strm.fail() ) {
	    boost::throw_exception(boost::system::system_error(
	       make_error_code(no_such_file_or_directory),pathname.string()));	
	}
    }
}


void session::createfile( boost::filesystem::ofstream& strm, 
		 const boost::filesystem::path& pathname ) {
    using namespace boost::system;
    using namespace boost::filesystem;

    create_directories(pathname.parent_path());
    strm.open(pathname,std::ios_base::out | std::ios_base::trunc);
    if( strm.fail() ) {
	using namespace boost::system::errc;
	boost::throw_exception(boost::system::system_error(
	       make_error_code(no_such_file_or_directory),pathname.string()));	
    }
}


void session::openfile( boost::filesystem::ifstream& strm, 
		     const boost::filesystem::path& pathname ) {
    using namespace boost::system::errc;
    using namespace boost::filesystem;

    check(pathname);

    strm.open(pathname);
    if( !strm.fail() ) return;
    
    /* \todo figure out how to pass iostream error code in exception. */
    boost::throw_exception(boost::system::system_error(
	       make_error_code(no_such_file_or_directory),pathname.string()));	
}


slice<char> session::loadtext( const boost::filesystem::path& p )
{
    using namespace boost::filesystem;
    using namespace boost::system::errc;

    textMap::const_iterator found = texts.find(p);
    if( found != texts.end() ) {
	return found->second;
    }

    if( is_regular_file(p) ) {
	size_t fileSize = file_size(p);
	char *text = new char[ fileSize + 1 ];
	ifstream file;
	openfile(file,p);
	file.read(text,fileSize);
	text[fileSize] = '\0';
	file.close();
	/* +1 for zero but it would arbitrarly augment text -
	   does not work for tokenizers. */
	texts[p] = slice<char>(text,&text[fileSize]); 
	return texts[p];
    }

    return slice<char>();
}


RAPIDXML::xml_document<>*
session::loadxml( const boost::filesystem::path& p )
{
    xmlMap::const_iterator found = xmls.find(p);
    if( found != xmls.end() ) {
	return found->second;
    }
    slice<char> buffer = loadtext(p);
#if 0
    std::cerr << "loadxml(" << p << "):" << std::endl;
    std::cerr << "\"" << buffer.begin() << "\"" << std::endl;
#endif
    RAPIDXML::xml_document<> *doc = new RAPIDXML::xml_document<>();
    try {
	doc->parse<0>(buffer.begin());
	xmls[p] = doc;
    } catch( std::exception& e ) {
	delete doc;
	doc = NULL;
	/* We would rather return NULL here instead of rethrowing the exception
	   because part of the callback fetch that requested the XML must 
	   write an alternate message. */
    }
    return doc; 
}


boost::filesystem::path session::stateDir() const {
    std::string s = valueOf("sessionDir");
    if( s.empty() ) {
	s = sessionDir;
    }
    return boost::filesystem::path(s);
}


boost::filesystem::path session::stateFilePath() const
{
    return stateDir() / (sessionId + ".session");
}


url session::asUrl( const boost::filesystem::path& p ) const
{
    boost::filesystem::path name = p;
    boost::filesystem::path siteTop = valueOf("siteTop");
    if( prefix(siteTop,p) ) {
	name = boost::filesystem::path("/") / subdirpart(siteTop,p);
    }
    return url("","",name);
}


url session::asAbsUrl( const url& u, const boost::filesystem::path& base ) const
{
    url result = u;
    if( u.protocol.empty() ) {
	result.protocol = "http";
    }
    if( u.host.empty() ) {
	result.host = domainName.value(*this).string();
    }
    if( !u.pathname.is_complete() ) {
	result.pathname = base / u.pathname;
    }
    return result;
}


void session::filters( variables& results, sourceType source ) const
{
    for( variables::const_iterator p = vars.begin();
	 p != vars.end(); ++p ) {
	if( p->second.source == source ) {
	    results[p->first] = p->second;
	}
    }
}


void session::insert( const std::string& name, const std::string& value,
		      sourceType source ) 
{
    variables::const_iterator look = vars.find(name);
    if( look == vars.end() || source <= look->second.source ) { 
	vars[name] = valT(value,source);
    }
}


void session::state( const std::string& name, const std::string& value ) 
{    
    vars[name] = valT(value,sessionfile);
    if( !exists() ) {
	using namespace boost::posix_time;

	std::stringstream s;
	s << boost::uuids::random_generator()();
	sessionId = s.str();

	s.str("");
	s << second_clock::local_time();
	vars["startTime"] = valT(s.str(),sessionfile);
    }
}


bool session::prefix( const boost::filesystem::path& left, 
		      const boost::filesystem::path& right ) const 
{
    return right.string().compare(0,left.string().size(),left.string()) == 0;
}


void session::privileged( bool v ) {
    /* change uid. */
    /* \todo Setting the setuid flag. 
       The first digit selects attributes
           set user ID (4)
           set group ID (2) 
           sticky (1) 

       chmod 4755 semilla
       sudo chown root semilla
*/
    uid_t realId = getuid();
#if 0
    uid_t effectiveId = geteuid();
    std::cerr << "!!! real_uid=" << realId << ", effective_uid="
	      << effectiveId << std::endl;
#endif
    uid_t newId = v ? 0 : realId;
    if( setuid(newId) < 0 ) {
	std::cerr << "error: setuid to zero: " 
		  << ((errno == EINVAL) ? "invalid" : "eperm") << std::endl;	
    }
}


static std::string nullString("");


const std::string& session::valueOf( const std::string& name ) const {
    variables::const_iterator iter = vars.find(name);
    if( iter == vars.end() ) {
	return nullString;
    }
    return iter->second.value;
}


boost::filesystem::path 
session::abspath( const boost::filesystem::path& relative ) const {
    using namespace boost::filesystem;

    /* In CGI mode, we interpret paths relative to siteTop,
       otherwise in command-line mode, we use the usual convention
       of paths relative to current directory. */
    if( !runAsCGI() ) {
	return relative.is_complete() ? relative : (current_path() / relative);
    }

    path fromSiteTop(valueOf("siteTop"));
    if( fromSiteTop.empty() ) {
	using namespace boost::system::errc;
	boost::throw_exception(boost::system::system_error(
	    make_error_code(no_such_file_or_directory),
	    std::string("siteTop not found") + fromSiteTop.string()));
    }
    if( relative.string().compare(0,fromSiteTop.string().size(),
				  fromSiteTop.string()) == 0 ) {
	fromSiteTop = relative;
    } else {
	/* avoid to keep prepending siteTop in case the file does not exist.
	 */
	fromSiteTop /= relative;
    }
    
    /* We used to throw an exception at this point. That does
       not sit very well with dispatch::fetch() because
       the value of a "document" might not be an actual file.
       Since the relative path of "document" will initialy 
       be derived from the web server request uri, it is 
       the most appropriate to return the path from siteTop
       in case the document could not be found.
       \todo We have to return from srcTop because that is how
       the website is configured for rss feeds. */
    return fromSiteTop;
}


void session::restore( int argc, char *argv[],
		       const boost::program_options::options_description& opts )
{
    using namespace boost;
    using namespace boost::system;
    using namespace boost::filesystem;
    using namespace boost::program_options;

    /* We assume that if SCRIPT_FILENAME is defined
       we are running as a cgi. */
    ascgi = ( getenv("SCRIPT_FILENAME") != NULL );

    remoteAddr = "unknown";
    char *_remoteAddr = getenv("REMOTE_ADDR");
    if( _remoteAddr ) {
	boost::smatch m;
	if( regex_match(std::string(_remoteAddr),m,
			       boost::regex("\\d+\\.\\d+\\.\\d+\\.\\d+")) ) {
	    remoteAddr = _remoteAddr;
	}
    }

    positional_options_description pd;     
    pd.add(document.name, 1);

    /* 1. The command-line arguments are added to the session. */
    {
	variables_map params;
	command_line_parser parser(argc, argv);
	parser.options(opts);
	parser.positional(pd);    
	boost::program_options::store(parser.run(),params);  
	for( variables_map::const_iterator param = params.begin(); 
	     param != params.end(); ++param ) {	    
	    /* Without the strip(), there is a ' ' appended to the command
	       line on Linux apache2. */
	    std::string s = strip(param->second.as<std::string>());
	    if( !s.empty() ) {
		insert(param->first,s,cmdline);
	    }
	}
    }

    /* 2. If a "config" file name does not exist at this point, a (config,
       filename) pair compiled into the binary executable is added. */
    std::string config(configFile);
    variables::const_iterator cfg = vars.find("config");
    if( cfg != vars.end() ) {
	config = cfg->second.value;
    }

    /* 3. If the config file exists, the (name,value) pairs in that config file
       are added to the session. */
    load(opts,config,configfile);

    /* 4. Parameters passed in environment variables through the CGI invokation
       are then parsed. */
    variables_map cgiParams;
    cgi_parser parser;
    parser.options(opts);
    parser.positional(pd);
    boost::program_options::store(parser.run(),cgiParams);

    /* 5. If a "sessionName" and a derived file exists, the (name,value) pairs 
       in that session file are added to the session. */
    cgi_parser::querySet::const_iterator sid 
	= parser.query.find(sessionName);
    if( sid != parser.query.end() ) {
	sessionId = sid->second;
    }
    if( exists() ) {
	load(opts,stateFilePath(),sessionfile);
    }

    /* 6. The parsed CGI parameters are added to the session. */
    for( std::map<std::string,std::string>::const_iterator 
	     p = parser.query.begin(); p != parser.query.end(); ++p ) {    
	insert(p->first,p->second,queryenv);
    }
    
    /* set the username to the value of LOGNAME in case no information
       can be retrieved for the session. It helps with keeping track
       of time spent with a shell command line. */
    session::variables::const_iterator v = vars.find("username");
    if( v == vars.end() ) {
	char *logName = getenv("LOGNAME");
	if( logName != NULL ) {
	    insert("username",logName);
	}
    }

    /* Append a trailing '/' if the document is a directory
       to match Apache's rewrite rules. */    
    std::string docname = document.value(*this).string();
    if( boost::filesystem::is_directory(docname) 
	&& (docname.size() == 0 
	    || docname[docname.size() - 1] != '/') ) {
	sourceType source = unknown;
	variables::const_iterator iter = vars.find(document.name);
	if( iter != vars.end() ) {
	    source = iter->second.source;
	}
	insert(document.name,docname + '/',source);
    }
}


boost::filesystem::path 
session::root( const boost::filesystem::path& leaf,
	       const boost::filesystem::path& trigger ) const
{
    using namespace boost::filesystem;
    std::string srcTop = valueOf("srcTop");
    path dirname = leaf;
    if( !is_directory(dirname) ) {
	dirname = dirname.parent_path();
    }
    bool foundProject = boost::filesystem::exists(dirname.string() / trigger);
    while( !foundProject && (dirname.string() != srcTop) ) {
	dirname.remove_leaf();
	if( dirname.string().empty() ) {
	    using namespace boost::system::errc;
	    boost::throw_exception(boost::system::system_error(
		make_error_code(no_such_file_or_directory),
		leaf.string())); 
	}
	foundProject = boost::filesystem::exists(dirname.string() / trigger);
    }
    return foundProject ? dirname : path("");
}


void session::show( std::ostream& ostr ) {

    static const char *sourceTypeNames[] = {
	"unknown",
	"queryenv",
	"cmdline",
	"sessionfile",
	"configfile"
    };

    if( exists() ) {
	ostr << "session " << sessionId << std::endl;
    } else {
	ostr << "no session id" << std::endl;
    }
    for( variables::const_iterator var = vars.begin(); 
	 var != vars.end(); ++var ) {
	ostr << var->first << ": " << var->second.value 
	     << " [" << sourceTypeNames[var->second.source] << "]" << std::endl;
    }
}


void session::store() {
    using namespace boost::system;
    using namespace boost::filesystem;

    /* 1. open session file based on session id. */
    ofstream sessions(stateFilePath());
    if( sessions.fail() ) {
	using namespace boost::system::errc;
	boost::throw_exception(boost::system::system_error(
	    make_error_code(no_such_file_or_directory),
	    stateFilePath().string()));
    }
    /* 2. write session information */
    for( variables::const_iterator v = vars.begin(); v != vars.end(); ++v ) {
	if( v->second.source == sessionfile ) {
	    sessions << v->first << '=' << v->second.value << std::endl; 
	}
    }
    sessions.close();
}


void session::remove() {
    boost::filesystem::remove(stateFilePath());
    sessionId = "";
}


boost::filesystem::path 
session::subdirpart( const boost::filesystem::path& root,
		     const boost::filesystem::path& leaf ) const {
    if( leaf.string().compare(0,root.string().size(),root.string()) != 0 ) {
	boost::throw_exception(std::runtime_error(root.string() 
		+ " is not a prefix of " + leaf.string()));
    }
    return leaf.string().substr(std::min(leaf.string().size(),
	root.string().size() 
        + (root.string()[root.string().size() - 1] != '/' ? 1 : 0)));
}


boost::filesystem::path 
session::valueAsPath( const std::string& name ) const {
    variables::const_iterator iter = vars.find(name);
    if( iter == vars.end() ) {
	boost::throw_exception(undefVariableError(name));
    }
    boost::filesystem::path absolute 
	= abspath(boost::filesystem::path(iter->second.value));
    return absolute;
}

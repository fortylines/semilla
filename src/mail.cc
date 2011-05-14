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

#include <string>
#include <locale>
#include <boost/date_time/local_time/local_time.hpp>
#include "mail.hh"
#include "markup.hh"

/** Parser for mailboxes

    references: 
      http://en.wikipedia.org/wiki/Mbox
      see also rfc2822tok.cc

    Primary Author(s): Sebastien Mirolo <smirolo@fortylines.com>
*/


void mailthread::filters( const post& p ) {
    static const boost::regex titlePat("(^.*:)(.*)");

    boost::smatch m;
    if( boost::regex_match(p.title,m,titlePat) ) {    
	indexMap::iterator found = indexes.find(m.str(2));
	if( found != indexes.end() ) {
	    ++found->second;
	} else {
	    indexes[m.str(2)] = 0;
	}
    }
}


void mailthread::flush() {
    for( indexMap::iterator idx = indexes.begin(); 
	 idx != indexes.end(); ++idx ) {
	*ostr << idx->first << "(" << idx->second << ")" << std::endl;
    }
}


void mailAsPost::newline(const char *line, int first, int last )
{
}


void mailAsPost::token( rfc2822Token token, const char *line, 
			int first, int last, bool fragment )
{
    switch( token ) {
    case rfc2822FieldName:
	field = rfc2822Err;
	if( strncmp(&line[first],"Date",last - first) == 0 ) {
	    field = rfc2822Time;
	} else if( strncmp(&line[first],"From",last - first) == 0 ) {
	    field = rfc2822AuthorEmail;
	} else if( strncmp(&line[first],"Subject",last - first) == 0 ) {
	    field = rfc2822Title;
	} else {
	    name = std::string(&line[first],last - first);
	}
	break;
    case rfc2822FieldBody: {
	std::string value = strip(std::string(&line[first],last - first));
	switch( field ) {
	case rfc2822Time:
	    try {
		constructed.time = from_mbox_string(value);
	    } catch( std::exception& e ) {
		std::cerr << "unable to reconstruct time from " << value << std::endl;
	    }
	    break;
	case rfc2822AuthorEmail:
	    constructed.author = extractName(value);
	    constructed.authorEmail = extractEmailAddress(value);	    
	    break;
	case rfc2822Title:
	    constructed.title = value;
	    break;
	default: {
	    post::headersMap::iterator header 
		= constructed.moreHeaders.find(name);
	    if( header != constructed.moreHeaders.end() ) {
		header->second += std::string(",") + value;
	    } else {
		constructed.moreHeaders[name] = value;
	    }
	} break;
	}
    } break;
    case rfc2822MessageBody:
	constructed.content = std::string(&line[first],&line[last]);
	break;
    default:
	/* to stop gcc from complaining */
	break;
    }    
}


void mailParser::walk( session& s, std::istream& ins, const std::string& name ) const
{
    using namespace boost::gregorian;
    using namespace boost::posix_time;
    using namespace boost::local_time;
    using namespace std;

    typedef std::set<std::string> tagSet;

    /** tags associated to a post. 
     */
    size_t lineCount = 0;
    mailAsPost listener;
    rfc2822Tokenizer tok(listener);
    std::stringstream mailtext;
    while( !ins.eof() ) {
	std::string line;
	std::getline(ins,line);
	++lineCount;

	if( line.compare(0,5,"From ") == 0 ) {
	    /* Beginning of new message
	       http://en.wikipedia.org/wiki/Mbox */
	    if( !mailtext.str().empty() ) {
		/* First line of file introduces first e-mail. */
		if( stopOnFirst ) break;	    
		tok.tokenize(mailtext.str().c_str(),mailtext.str().size());
		filter->filters(listener.unserialized());
		mailtext.str("");
	    }
	} else {
	    mailtext << line << std::endl;
	}
    }
    tok.tokenize(mailtext.str().c_str(),mailtext.str().size());
    filter->filters(listener.unserialized());
    mailtext.str("");    
}


void mailParserFetch( session& s, const boost::filesystem::path& pathname )
{
    mailthread mt(s.out());
    mailParser mp(mt);
    mp.fetch(s,pathname);
#if 0
    /* \todo Cannot flush here if we want to support feedContent calls blogEntry.
       This call will generate non-intended intermediate flushes. It might be
       useful for other parts of the system though...
     */
    filter->flush();
#endif
}

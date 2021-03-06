/* Copyright (c) 2009-2013, Fortylines LLC
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

/* <a href="http://www.w3.org/TR/html5/">HTML5 Reference</a> */

#include <cassert>
#include "tokenize.hh"

/** XML escape tokenizer

    Primary Author(s): Sebastien Mirolo <smirolo@fortylines.com>
*/

namespace tero {

const char *xmlEscTokenTitles[] = {
    "escErr",
    "escAmpEscape",
    "escData",
    "escGtEscape",
    "escLtEscape",
    "escQuotEscape"
};


#define advance(state) { trans = &&state; goto advancePointer; }


size_t xmlEscTokenizer::tokenize( const char *line, size_t n )
{
    const char *p = line;
    size_t last = 0;
    size_t first = 0;

    if( n == 0 ) return 0;
    if( trans != NULL ) goto *trans; else goto token;

advancePointer:
	last = (size_t)std::distance(line,p);
	if( last >= n ) {
		if( last > first && listener != NULL ) {
			listener->token(tok,line,first,last,true);
		}
		return last;
	}
    switch( *p ) {
    case '\\':
		savedtrans = trans;	
		trans = &&esceol;
		goto eolAdvancePointer;
    case '\r':
		/* windows and internet protocols */
		savedtrans = trans;
		trans = &&eolmacwin;
		goto eolAdvancePointer;
    case '\n':  
		/* unix-like */
		savedtrans = trans;
		trans = &&eol;
		goto eolAdvancePointer;
    case '\0':
		goto eolAdvancePointer;
    }
	++p;
    goto *trans;

 eolAdvancePointer:
	last = (size_t)std::distance(line,p);
	if( last >= n || *p == '\0' ) {
		if( last > first && listener != NULL ) {
			listener->token(tok,line,first,last,true);
		}
		return last;
	}
	++p;
    goto *trans;
	
 esceol:
	/* Escaped EOL character */
	switch( *p ) {
	case '\r':
		/* windows and internet protocols */
		trans = &&eolmacwin;
		goto eolAdvancePointer;
	case '\n':
		/* unix-like */
		trans = &&eol;
		goto eolAdvancePointer;
	}
	/* not an escaped newline */
	trans = savedtrans;
	savedtrans = NULL;
	goto *trans;

 eolmacwin:
	switch( *p ) {
	case '\n':
		trans = &&eol;
		goto eolAdvancePointer;
	}
	goto eol;
	
 eol:
	if( last > first && listener != NULL ) {
		listener->token(tok,line,first,last,false);
	}
	first = last;
	last = (size_t)std::distance(line,p);
	if( last > first && listener != NULL ) {
		listener->newline(line,first,last);
	}
	first = last;
	trans = savedtrans;
	savedtrans = NULL;
	goto token;

	/* specialized tokenizer */
       
 data:
    switch( *p ) {
    case '<':
    case '>':
    case '&':
    case '"':
	goto token;
    }
    advance(data);
    
 token:
    last = std::distance(line,p);
    if( last > first && listener != NULL ) {
		listener->token(tok,line,first,last,false);
		trans = NULL;
    }
    tok = escErr;
    first = last;
    switch( *p ) {
    case '<':
	tok = escLtEscape;
	advance(token);
	break;
    case '>':
	tok = escGtEscape;
	advance(token);
	break;
    case '&':
	tok = escAmpEscape;
	advance(token);
	break;	
    case '"':
	tok = escQuotEscape;
	advance(token);
	break;	
    }
    tok = escData;
    advance(data);
}

}

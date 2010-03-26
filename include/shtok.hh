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

#ifndef guardshtok
#define guardshtok

#include <iterator>

enum shToken {
    shErr,
    shComment,
    shCode
};

extern const char *shTokenTitles[];

template<typename ch, typename tr>
inline std::basic_ostream<ch, tr>&
operator<<( std::basic_ostream<ch, tr>& ostr, shToken v ) {
    return ostr << shTokenTitles[v];
}


/** Interface for callbacks from the shTokenizer
 */
class shTokListener {
public:
    shTokListener() {}
    
    virtual void newline(const char *line, 
			  int first, int last ) = 0;
    
    virtual void token( shToken token, const char *line, 
			int first, int last, bool fragment ) = 0;
};


/** Tokenizer for shell-based (sh, python, Makefile) source files
 */
class shTokenizer {
protected:
	void *state;
	shToken tok;
	shTokListener *listener;

public:
    shTokenizer() 
	: state(NULL), tok(shErr), listener(NULL) {}
	
    explicit shTokenizer( shTokListener& l ) 
	: state(NULL), tok(shErr), listener(&l) {}
    
    void attach( shTokListener& l ) { listener = &l; }
    
    size_t tokenize( const char *line, size_t n );
};

#endif

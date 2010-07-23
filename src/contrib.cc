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

#include "contrib.hh"
#include "markup.hh"

void contribCreate::fetch( session& s, const boost::filesystem::path& pathname )
{
}


void contribIdx::meta( session& s, const boost::filesystem::path& pathname ) 
{
    s.vars["title"] = "A last word";
    document::meta(s,pathname);
}


void contribIdx::fetch( session& s, const boost::filesystem::path& pathname )
{
    using namespace std;
    using namespace boost::filesystem;

    cout << htmlContent;
    
#if 0
    std::cout << html::table();    
#endif
    if( is_directory(pathname) ) {
	for( directory_iterator entry = directory_iterator(pathname); 
	     entry != directory_iterator(); ++entry ) {
	    if( is_directory(*entry) ) {
		boost::filesystem::path 
		    profilePathname = path(*entry) / "profile.blog";	
		if( boost::filesystem::is_regular_file(profilePathname) ) {
#if 0
		    cout << html::tr() 
			 << html::td() 
			 << "<img src=\"/resources/contrib/" 
			 << entry->filename() << "/profile.jpg\">"
			 << html::td::end
			 << html::td();
#endif		    
		    text t;
		    t.fetch(s,profilePathname);
#if 0		    
		    cout << html::td::end
			 << html::tr::end;
#endif
		}
	    }
	}
    }
#if 0
    std::cout << html::table::end;
#endif
}


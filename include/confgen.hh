/* Copyright (c) 2010, Fortylines LLC
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

#ifndef guardconfgen
#define guardconfgen

#include "document.hh"
#include <boost/program_options.hpp>

class confgenCheckout : public document {
protected:
    const char *nextPathname;
    
public:
    confgenCheckout( std::ostream& o, const char* np ) 
	: document(o), nextPathname(np) {}

    static void 
    addSessionVars( boost::program_options::options_description& opts );

    void fetch( session& s, const boost::filesystem::path& pathname );    

    void meta( session& s, const boost::filesystem::path& pathname );
};


class confgenDeliver : public document {
protected:
    const char *thisPathname;

    std::string adminLogin;
    std::string domainName;
    boost::filesystem::path packagePath;

public:
    confgenDeliver( std::ostream& o, const char* tp ) 
	: document(o), thisPathname(tp) {}
    
    void meta( session& s, const boost::filesystem::path& pathname );

    void fetch( session& s, const boost::filesystem::path& pathname );    
};


class forceDownload : public document {
protected:

public:
    explicit forceDownload( std::ostream& o ) 
	: document(o) {}

    void fetch( session& s, const boost::filesystem::path& pathname );    
};


#endif

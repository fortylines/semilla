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

#ifndef guardblog
#define guardblog

#include "post.hh"
#include "feeds.hh"

/** Pages related to blog posts.

    Primary Author(s): Sebastien Mirolo <smirolo@fortylines.com>
*/


/** Present blog entries in a specific interval [first,last[
    where first and last are based on *cmp*.
*/
template<typename cmp>
class blogInterval : public feedOrdered<cmp> {
protected:
    const boost::filesystem::path pathname;

public:
    typedef feedOrdered<cmp> super;

public:
    explicit blogInterval( postFilter* n, const boost::filesystem::path& p ) 
	: super(n), pathname(p) {}

    void provide();
};


template<typename defaultWriter, const char* varname, const char* filePat, 
	 typename cmp>
void blogByInterval( session& s, const boost::filesystem::path& pathname );

void blogByIntervalDate( session& s, const boost::filesystem::path& pathname );
void blogByIntervalTags( session& s, const boost::filesystem::path& pathname );


template<typename cmp>
class bySet : public retainedFilter {
protected:
    /* store the number blog entries with a specific key. */
    typedef std::map<typename cmp::keyType,uint32_t> linkSet;

    linkSet links;
    std::ostream *ostr;
    boost::filesystem::path root;

public:
    bySet( std::ostream& o, const boost::filesystem::path& r ) 
	: ostr(&o), root(r) {}

    virtual void filters( const post& p );
    
    virtual void flush(); 
};
 
/** Links to sets of blog posts sharing a specific key (ie. month, tag, etc.).
class blogSetLinks : public feedWriter 
filePat is a regex.
*/
template<typename cmp, const char *filePat>
void blogSetLinksFetch( session& s, const boost::filesystem::path& pathname );

template<const char *filePat>
void blogDateLinks( session& s, const boost::filesystem::path& pathname );

template<const char *filePat>
void blogTagLinks( session& s, const boost::filesystem::path& pathname );


/**
class blogEntry : public feedWriter {
*/
void blogEntryFetch( session& s, const boost::filesystem::path& pathname );

#include "blog.tcc"

#endif

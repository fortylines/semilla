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

#include "cppfiles.hh"
#include "changelist.hh"
#include "decorator.hh"

void cppFetch( session& s, const boost::filesystem::path& pathname ) 
{
    linkLight leftLinkStrm(s);
    linkLight rightLinkStrm(s);
    cppLight leftCppStrm;
    cppLight rightCppStrm;
    decoratorChain leftChain;
    decoratorChain rightChain;
    leftChain.push_back(leftLinkStrm);
    leftChain.push_back(leftCppStrm);
    rightChain.push_back(rightLinkStrm);
    rightChain.push_back(rightCppStrm);
    
    text cpp(leftChain,rightChain);
    cpp.fetch(s,pathname);
}


void cppDiff( session& s, const boost::filesystem::path& pathname ) 
{
    linkLight leftLinkStrm(s);
    linkLight rightLinkStrm(s);
    cppLight leftCppStrm;
    cppLight rightCppStrm;
    decoratorChain leftChain;
    decoratorChain rightChain;
    leftChain.push_back(leftLinkStrm);
    leftChain.push_back(leftCppStrm);
    rightChain.push_back(rightLinkStrm);
    rightChain.push_back(rightCppStrm);

    changediff(s,pathname,&leftChain,&rightChain);
}

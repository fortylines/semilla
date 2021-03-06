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

#include <boost/filesystem/fstream.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "project.hh"
#include "revsys.hh"
#include "decorator.hh"
#include "contrib.hh"

/** Pages related to projects

    Primary Author(s): Sebastien Mirolo <smirolo@fortylines.com>
*/

namespace tero {

pathVariable srcTop("srcTop","path to document top");


void
projectAddSessionVars( boost::program_options::options_description& all,
    boost::program_options::options_description& visible )
{
    using namespace boost::program_options;

    options_description localOpts("project");
    localOpts.add(srcTop.option());
    all.add(localOpts);
    visible.add(localOpts);
}


projhref::projhref( const session& s, const std::string& n )
    : name(n), base(boost::filesystem::path("/")
        / s.subdirpart(siteTop.value(s),
            srcTop.value(s)))
{
}


std::string
projectName( const session& s, const boost::filesystem::path& p ) {
    boost::filesystem::path base(p);
    while( !base.string().empty() && !is_directory(base) ) {
        base.remove_leaf();
    }
    /* \todo use .git as marker in place of srcTop for project? */
    boost::filesystem::path
        projRelativePath = s.subdirpart(srcTop.value(s),base);

    std::string projname;
    if( projRelativePath.begin() != projRelativePath.end() ) {
        projname = (*projRelativePath.begin()).string();
    }
    if( projname[projname.size() - 1] == '/' ) {
        projname = projname.substr(0,projname.size() - 1);
    }
    return projname;
}

void projectTitle( session& s, const url& name )
{
    s.insert("title",projectName(s,s.abspath(name)));
    session::variables::const_iterator look = s.find("title");
    if( s.found(look) ) {
        s.out() << look->second.value;
    } else {
        s.out() << name;
    }
}


void projindexFetch( session& s, const slice<char>& text, const url& name )
{
    using namespace RAPIDXML;
    using namespace boost::filesystem;

    path pathname = s.abspath(name);
    path projdir = pathname.parent_path().filename();
    xml_document<> doc;    // character type defaults to char
    doc.parse<0>((char*)text.begin());     // 0 means default parse flags

    xml_node<> *root = doc.first_node();
    if( root != NULL ) {
        xml_attribute<> *projname = NULL;
        for( xml_node<> *project = root->first_node("project");
             project != NULL; project = project->next_sibling("project") ) {

            projname = project->first_attribute("name");

            /* Description of the project */
            hrefLight dec(s);
            dec.attach(s.out());
            for( xml_node<> *descr
                     = project->first_node("description"); descr != NULL;
                 descr = descr->next_sibling("description") ) {
                s.out() << html::p()
#if 0
                        << descr->value();
#endif
                ;
                for( xml_node<> *child
                         = descr->first_node(); child != NULL;
                     child = child->next_sibling() ) {
                    s.out() << child->value();
                }
                s.out() << html::p::end;
            }
            dec.detach();

            /* Information about the maintainer */
            xml_node<> *maintainer = project->first_node("maintainer");
            if( maintainer ) {
                xml_node<> *name = maintainer->first_node("personname");
                xml_node<> *email = maintainer->first_node("email");
                if( email ) {
                    s.out() << html::p() << "maintained ";
                    s.out() << by(contrib::find(email->value(),name->value()));
                    s.out() << html::p::end;
                }
            }

            /* Shows the archives that can be downloaded. */
            const char *dists[] = {
                "resources/Darwin",
                "resources/Fedora",
                "resources/Ubuntu",
                "resources/srcs"
            };
            typedef std::vector<path> candidateSet;
            candidateSet candidates;
            for( const char **d = dists;
                 d != &dists[sizeof(dists)/sizeof(char*)]; ++d ) {
                path dirname(siteTop.value(s) / std::string(*d));
                path prefix(dirname / std::string(projname->value()));
                if( boost::filesystem::exists(dirname) ) {
                    for( directory_iterator entry = directory_iterator(dirname); 
                         entry != directory_iterator(); ++entry ) {
                        path p(*entry);
                        if( p.string().compare(0,prefix.string().size(),
                                prefix.string()) == 0 ) {
                            candidates.push_back(path("/") / path(*d) / *entry);
                        }
                    }
                }
            }
            s.out() << html::p();
            if( candidates.empty() ) {
                s.out() << "There are no prepackaged archive available for download.";
            } else {
                for( candidateSet::const_iterator c = candidates.begin();
                     c != candidates.end(); ++c ) {
                    s.out() << html::a().href(c->string())
                            << c->filename() << html::a::end
                            << "<br />" << std::endl;
                }
            }
            s.out() << html::p::end;

            /* Dependencies to install the project from a source compilation. */
            xml_node<> *repository = project->first_node("repository");
            if( repository == NULL ) repository = project->first_node("patch");
            if( repository ) {
                const char *sep = "";
                s.out() << html::p() << "The repository is available at "
                        << html::p::end;
                s.out() << html::pre()
                        << "http://" << domainName.value(s)
                        << "/reps/" << projhref(s,projdir.string()) << "/.git"
                        << html::pre::end;
                s.out() << html::p() << "The following prerequisites are "
                    "necessary to build the project from source: ";
                for( xml_node<> *dep = repository->first_node("dep");
                     dep != NULL; dep = dep->next_sibling() ) {
                    xml_attribute<> *name = dep->first_attribute("name");
                    if( name != NULL ) {
                        if( boost::filesystem::exists(srcTop.value(s)
                                / name->value()) ) {
                            s.out() << sep 
                                    << projhref(s,name->value());
                        } else {
                            s.out() << sep << name->value();
                        }
                        sep = ", ";
                    }
                }
                s.out() << '.';
                s.out() << " All the necessary steps to build and install from source"
                        << " can also be executed in a single "
                        << html::a().href("/log/") << "build" << html::a::end
                        << " command.";
                s.out() << html::p::end;
                s.out() << html::p();
                s.out() << "You can then later update the local copy"
                    " of the source tree, re-build the prerequisites re-make"
                    " and re-install the binaries with the following commands:";
                s.out() << html::p::end;
                s.out() << html::pre();
                s.out() << "cd *buildTop*/" << projname->value() << std::endl;
                s.out() << "dws make recurse" << std::endl;
                s.out() << "dws make install" << std::endl;
                s.out() << html::pre::end;
            }
        }
    }
}


void
projfiles::addDir( session& s, const boost::filesystem::path& dir ) const {
    switch( state ) {
    case start:
        s.out() << html::div().classref("MenuWidget");
        break;
    case toplevelFiles:
    case direntryFiles:
        s.out() << html::p::end;
        break;
    }
    state = direntryFiles;

    url href = s.asUrl(dir);
    std::string dirname = dir.filename().string();
    if( dirname.compare(".") == 0 ) {
        dirname = dir.parent_path().filename().string();
    }
    s.out() << html::h(2) << dirname << html::h(2).end();
    s.out() << html::p();
}


void
projfiles::addFile( session& s, const boost::filesystem::path& file ) const {
    if( state == start ) {
        s.out() << html::div().classref("MenuWidget");
        s.out() << html::p();
        state = toplevelFiles;
    }
    s.out() << html::a().href(s.asUrl(file).string())
            << file.leaf().string()
            << html::a::end << "<br />" << std::endl;
}


void projfiles::flush( session& s ) const
{
    switch( state ) {
    case toplevelFiles:
    case direntryFiles:
        s.out() << html::p::end;
        s.out() << html::div::end;
        break;
    default:
        /* Nothing to do excepts shutup gcc warnings. */
        break;
    }
}


void projfiles::fetch( session& s, const boost::filesystem::path& pathname )
{
    using namespace std;
    using namespace boost::system;

    state = dirwalker::start;
    revisionsys *rev = revisionsys::findRev(s, pathname);
    boost::filesystem::path projdir
        = rev ? rev->rootpath : s.root(pathname, "dws.xml");
    recurse(s, rev, projdir);
    flush(s);
}


void projfilesFetch( session& s, const url& name )
{
    projfiles p;
    p.fetch(s,s.abspath(name));
}

}

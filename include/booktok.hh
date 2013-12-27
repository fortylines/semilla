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

#ifndef guardbooktok
#define guardbooktok

#include <list>
#include "tokenize.hh"
#include "slice.hh"

/** Definition of tokens for docbook documents. This file was generated
    by the ll(1) parser generator and further modified to fit the required
    API by the docbook to HTML presentation engine.

   Primary Author(s): Sebastien Mirolo <smirolo@fortylines.com>
*/

namespace tero {

enum docbookToken {
    bookEof,
    abbrevEnd,
    abbrevStart,
    abstractEnd,
    abstractStart,
    accelEnd,
    accelStart,
    acknowledgementsEnd,
    acknowledgementsStart,
    acronymEnd,
    acronymStart,
    addressEnd,
    addressStart,
    affiliationEnd,
    affiliationStart,
    altEnd,
    altStart,
    anchorEnd,
    anchorStart,
    annotationEnd,
    annotationStart,
    answerEnd,
    answerStart,
    appendixEnd,
    appendixStart,
    applicationEnd,
    applicationStart,
    arcEnd,
    arcStart,
    areaEnd,
    areaStart,
    areasetEnd,
    areasetStart,
    areaspecEnd,
    areaspecStart,
    argEnd,
    argStart,
    articleEnd,
    articleStart,
    artpagenumsEnd,
    artpagenumsStart,
    attributionEnd,
    attributionStart,
    audiodataEnd,
    audiodataStart,
    audioobjectEnd,
    audioobjectStart,
    authorEnd,
    authorStart,
    authorgroupEnd,
    authorgroupStart,
    authorinitialsEnd,
    authorinitialsStart,
    bibliocoverageEnd,
    bibliocoverageStart,
    bibliodivEnd,
    bibliodivStart,
    biblioentryEnd,
    biblioentryStart,
    bibliographyEnd,
    bibliographyStart,
    biblioidEnd,
    biblioidStart,
    bibliolistEnd,
    bibliolistStart,
    bibliomiscEnd,
    bibliomiscStart,
    bibliomixedEnd,
    bibliomixedStart,
    bibliomsetEnd,
    bibliomsetStart,
    bibliorefEnd,
    bibliorefStart,
    bibliorelationEnd,
    bibliorelationStart,
    bibliosetEnd,
    bibliosetStart,
    bibliosourceEnd,
    bibliosourceStart,
	blockquoteEnd,
	blockquoteStart,
	bookEnd,
	bookStart,
	bridgeheadEnd,
	bridgeheadStart,
	calloutEnd,
	calloutStart,
	calloutlistEnd,
	calloutlistStart,
	captionEnd,
	captionStart,
	cautionEnd,
	cautionStart,
	chapterEnd,
	chapterStart,
	citationEnd,
	citationStart,
	citebiblioidEnd,
	citebiblioidStart,
	citerefentryEnd,
	citerefentryStart,
	citetitleEnd,
	citetitleStart,
	cityEnd,
	cityStart,
	classnameEnd,
	classnameStart,
	classsynopsisEnd,
	classsynopsisStart,
	classsynopsisinfoEnd,
	classsynopsisinfoStart,
	cmdsynopsisEnd,
	cmdsynopsisStart,
	coEnd,
	coStart,
	codeEnd,
	codeStart,
	colEnd,
	colStart,
	colgroupEnd,
	colgroupStart,
	collabEnd,
	collabStart,
	colophonEnd,
	colophonStart,
	colspecEnd,
	colspecStart,
	commandEnd,
	commandStart,
	computeroutputEnd,
	computeroutputStart,
	confdatesEnd,
	confdatesStart,
	confgroupEnd,
	confgroupStart,
	confnumEnd,
	confnumStart,
	confsponsorEnd,
	confsponsorStart,
	conftitleEnd,
	conftitleStart,
	constantEnd,
	constantStart,
	constraintEnd,
	constraintStart,
	constraintdefEnd,
	constraintdefStart,
	constructorsynopsisEnd,
	constructorsynopsisStart,
	contractnumEnd,
	contractnumStart,
	contractsponsorEnd,
	contractsponsorStart,
	contribEnd,
	contribStart,
	copyrightEnd,
	copyrightStart,
	corefEnd,
	corefStart,
	countryEnd,
	countryStart,
	coverEnd,
	coverStart,
	databaseEnd,
	databaseStart,
	dateEnd,
	dateStart,
	dedicationEnd,
	dedicationStart,
	destructorsynopsisEnd,
	destructorsynopsisStart,
	editionEnd,
	editionStart,
	editorEnd,
	editorStart,
	emailEnd,
	emailStart,
	emphasisEnd,
	emphasisStart,
	entryEnd,
	entryStart,
	entrytblEnd,
	entrytblStart,
	envarEnd,
	envarStart,
	epigraphEnd,
	epigraphStart,
	equationEnd,
	equationStart,
	errorcodeEnd,
	errorcodeStart,
	errornameEnd,
	errornameStart,
	errortextEnd,
	errortextStart,
	errortypeEnd,
	errortypeStart,
	exampleEnd,
	exampleStart,
	exceptionnameEnd,
	exceptionnameStart,
	extendedlinkEnd,
	extendedlinkStart,
	faxEnd,
	faxStart,
	fieldsynopsisEnd,
	fieldsynopsisStart,
	figureEnd,
	figureStart,
	filenameEnd,
	filenameStart,
	firstnameEnd,
	firstnameStart,
	firsttermEnd,
	firsttermStart,
	footnoteEnd,
	footnoteStart,
	footnoterefEnd,
	footnoterefStart,
	foreignphraseEnd,
	foreignphraseStart,
	formalparaEnd,
	formalparaStart,
	funcdefEnd,
	funcdefStart,
	funcparamsEnd,
	funcparamsStart,
	funcprototypeEnd,
	funcprototypeStart,
	funcsynopsisEnd,
	funcsynopsisStart,
	funcsynopsisinfoEnd,
	funcsynopsisinfoStart,
	functionEnd,
	functionStart,
	glossaryEnd,
	glossaryStart,
	glossdefEnd,
	glossdefStart,
	glossdivEnd,
	glossdivStart,
	glossentryEnd,
	glossentryStart,
	glosslistEnd,
	glosslistStart,
	glossseeEnd,
	glossseeStart,
	glossseealsoEnd,
	glossseealsoStart,
	glosstermEnd,
	glosstermStart,
	groupEnd,
	groupStart,
	guibuttonEnd,
	guibuttonStart,
	guiiconEnd,
	guiiconStart,
	guilabelEnd,
	guilabelStart,
	guimenuEnd,
	guimenuStart,
	guimenuitemEnd,
	guimenuitemStart,
	guisubmenuEnd,
	guisubmenuStart,
	hardwareEnd,
	hardwareStart,
	holderEnd,
	holderStart,
	honorificEnd,
	honorificStart,
	imagedataEnd,
	imagedataStart,
	imageobjectEnd,
	imageobjectStart,
	imageobjectcoEnd,
	imageobjectcoStart,
	importantEnd,
	importantStart,
	indexEnd,
	indexStart,
	indexdivEnd,
	indexdivStart,
	indexentryEnd,
	indexentryStart,
	indextermEnd,
	indextermStart,
	infoEnd,
	infoStart,
	informalequationEnd,
	informalequationStart,
	informalexampleEnd,
	informalexampleStart,
	informalfigureEnd,
	informalfigureStart,
	informaltableEnd,
	informaltableStart,
	initializerEnd,
	initializerStart,
	inlineequationEnd,
	inlineequationStart,
	inlinemediaobjectEnd,
	inlinemediaobjectStart,
	interfacenameEnd,
	interfacenameStart,
	issuenumEnd,
	issuenumStart,
	itemizedlistEnd,
	itemizedlistStart,
	itermsetEnd,
	itermsetStart,
	jobtitleEnd,
	jobtitleStart,
	keycapEnd,
	keycapStart,
	keycodeEnd,
	keycodeStart,
	keycomboEnd,
	keycomboStart,
	keysymEnd,
	keysymStart,
	keywordEnd,
	keywordStart,
	keywordsetEnd,
	keywordsetStart,
	labelEnd,
	labelStart,
	legalnoticeEnd,
	legalnoticeStart,
	lhsEnd,
	lhsStart,
	lineageEnd,
	lineageStart,
	lineannotationEnd,
	lineannotationStart,
	linkEnd,
	linkStart,
	listitemEnd,
	listitemStart,
	literalEnd,
	literalStart,
	literallayoutEnd,
	literallayoutStart,
	locatorEnd,
	locatorStart,
	manvolnumEnd,
	manvolnumStart,
	markupEnd,
	markupStart,
	mathphraseEnd,
	mathphraseStart,
	mediaobjectEnd,
	mediaobjectStart,
	memberEnd,
	memberStart,
	menuchoiceEnd,
	menuchoiceStart,
	methodnameEnd,
	methodnameStart,
	methodparamEnd,
	methodparamStart,
	methodsynopsisEnd,
	methodsynopsisStart,
	modifierEnd,
	modifierStart,
	mousebuttonEnd,
	mousebuttonStart,
	msgEnd,
	msgStart,
	msgaudEnd,
	msgaudStart,
	msgentryEnd,
	msgentryStart,
	msgexplanEnd,
	msgexplanStart,
	msginfoEnd,
	msginfoStart,
	msglevelEnd,
	msglevelStart,
	msgmainEnd,
	msgmainStart,
	msgorigEnd,
	msgorigStart,
	msgrelEnd,
	msgrelStart,
	msgsetEnd,
	msgsetStart,
	msgsubEnd,
	msgsubStart,
	msgtextEnd,
	msgtextStart,
	nonterminalEnd,
	nonterminalStart,
	noteEnd,
	noteStart,
	olinkEnd,
	olinkStart,
	ooclassEnd,
	ooclassStart,
	ooexceptionEnd,
	ooexceptionStart,
	oointerfaceEnd,
	oointerfaceStart,
	optionEnd,
	optionStart,
	optionalEnd,
	optionalStart,
	orderedlistEnd,
	orderedlistStart,
	orgEnd,
	orgStart,
	orgdivEnd,
	orgdivStart,
	orgnameEnd,
	orgnameStart,
	otheraddrEnd,
	otheraddrStart,
	othercreditEnd,
	othercreditStart,
	othernameEnd,
	othernameStart,
	packageEnd,
	packageStart,
	pagenumsEnd,
	pagenumsStart,
	paraEnd,
	paraStart,
	paramdefEnd,
	paramdefStart,
	parameterEnd,
	parameterStart,
	partEnd,
	partStart,
	partintroEnd,
	partintroStart,
	personEnd,
	personStart,
	personblurbEnd,
	personblurbStart,
	personnameEnd,
	personnameStart,
	phoneEnd,
	phoneStart,
	phraseEnd,
	phraseStart,
	pobEnd,
	pobStart,
	postcodeEnd,
	postcodeStart,
	prefaceEnd,
	prefaceStart,
	primaryEnd,
	primaryStart,
	primaryieEnd,
	primaryieStart,
	printhistoryEnd,
	printhistoryStart,
	procedureEnd,
	procedureStart,
	productionEnd,
	productionStart,
	productionrecapEnd,
	productionrecapStart,
	productionsetEnd,
	productionsetStart,
	productnameEnd,
	productnameStart,
	productnumberEnd,
	productnumberStart,
	programlistingEnd,
	programlistingStart,
	programlistingcoEnd,
	programlistingcoStart,
	promptEnd,
	promptStart,
	propertyEnd,
	propertyStart,
	pubdateEnd,
	pubdateStart,
	publisherEnd,
	publisherStart,
	publishernameEnd,
	publishernameStart,
	qandadivEnd,
	qandadivStart,
	qandaentryEnd,
	qandaentryStart,
	qandasetEnd,
	qandasetStart,
	questionEnd,
	questionStart,
	quoteEnd,
	quoteStart,
	refclassEnd,
	refclassStart,
	refdescriptorEnd,
	refdescriptorStart,
	refentryEnd,
	refentryStart,
	refentrytitleEnd,
	refentrytitleStart,
	referenceEnd,
	referenceStart,
	refmetaEnd,
	refmetaStart,
	refmiscinfoEnd,
	refmiscinfoStart,
	refnameEnd,
	refnameStart,
	refnamedivEnd,
	refnamedivStart,
	refpurposeEnd,
	refpurposeStart,
	refsect1End,
	refsect1Start,
	refsect2End,
	refsect2Start,
	refsect3End,
	refsect3Start,
	refsectionEnd,
	refsectionStart,
	refsynopsisdivEnd,
	refsynopsisdivStart,
	releaseinfoEnd,
	releaseinfoStart,
	remarkEnd,
	remarkStart,
	replaceableEnd,
	replaceableStart,
	returnvalueEnd,
	returnvalueStart,
	revdescriptionEnd,
	revdescriptionStart,
	revhistoryEnd,
	revhistoryStart,
	revisionEnd,
	revisionStart,
	revnumberEnd,
	revnumberStart,
	revremarkEnd,
	revremarkStart,
	rhsEnd,
	rhsStart,
	rowEnd,
	rowStart,
	sbrEnd,
	sbrStart,
	screenEnd,
	screenStart,
	screencoEnd,
	screencoStart,
	screenshotEnd,
	screenshotStart,
	secondaryEnd,
	secondaryStart,
	secondaryieEnd,
	secondaryieStart,
	sect1End,
	sect1Start,
	sect2End,
	sect2Start,
	sect3End,
	sect3Start,
	sect4End,
	sect4Start,
	sect5End,
	sect5Start,
	sectionEnd,
	sectionStart,
	seeEnd,
	seeStart,
	seealsoEnd,
	seealsoStart,
	seealsoieEnd,
	seealsoieStart,
	seeieEnd,
	seeieStart,
	segEnd,
	segStart,
	seglistitemEnd,
	seglistitemStart,
	segmentedlistEnd,
	segmentedlistStart,
	segtitleEnd,
	segtitleStart,
	seriesvolnumsEnd,
	seriesvolnumsStart,
	setEnd,
	setStart,
	setindexEnd,
	setindexStart,
	shortaffilEnd,
	shortaffilStart,
	shortcutEnd,
	shortcutStart,
	sidebarEnd,
	sidebarStart,
	simparaEnd,
	simparaStart,
	simplelistEnd,
	simplelistStart,
	simplemsgentryEnd,
	simplemsgentryStart,
	simplesectEnd,
	simplesectStart,
	spanspecEnd,
	spanspecStart,
	stateEnd,
	stateStart,
	stepEnd,
	stepStart,
	stepalternativesEnd,
	stepalternativesStart,
	streetEnd,
	streetStart,
	subjectEnd,
	subjectStart,
	subjectsetEnd,
	subjectsetStart,
	subjecttermEnd,
	subjecttermStart,
	subscriptEnd,
	subscriptStart,
	substepsEnd,
	substepsStart,
	subtitleEnd,
	subtitleStart,
	superscriptEnd,
	superscriptStart,
	surnameEnd,
	surnameStart,
	symbolEnd,
	symbolStart,
	synopfragmentEnd,
	synopfragmentStart,
	synopfragmentrefEnd,
	synopfragmentrefStart,
	synopsisEnd,
	synopsisStart,
	systemitemEnd,
	systemitemStart,
	tableEnd,
	tableStart,
	tagEnd,
	tagStart,
	taskEnd,
	taskStart,
	taskprerequisitesEnd,
	taskprerequisitesStart,
	taskrelatedEnd,
	taskrelatedStart,
	tasksummaryEnd,
	tasksummaryStart,
	tbodyEnd,
	tbodyStart,
	tdEnd,
	tdStart,
	termEnd,
	termStart,
	termdefEnd,
	termdefStart,
	tertiaryEnd,
	tertiaryStart,
	tertiaryieEnd,
	tertiaryieStart,
	textdataEnd,
	textdataStart,
	textobjectEnd,
	textobjectStart,
	tfootEnd,
	tfootStart,
	tgroupEnd,
	tgroupStart,
	thEnd,
	thStart,
	theadEnd,
	theadStart,
	tipEnd,
	tipStart,
	titleEnd,
	titleStart,
	titleabbrevEnd,
	titleabbrevStart,
	tocEnd,
	tocStart,
	tocdivEnd,
	tocdivStart,
	tocentryEnd,
	tocentryStart,
	tokenEnd,
	tokenStart,
	trEnd,
	trStart,
	trademarkEnd,
	trademarkStart,
	typeEnd,
	typeStart,
	uriEnd,
	uriStart,
	userinputEnd,
	userinputStart,
	varargsEnd,
	varargsStart,
	variablelistEnd,
	variablelistStart,
	varlistentryEnd,
	varlistentryStart,
	varnameEnd,
	varnameStart,
	videodataEnd,
	videodataStart,
	videoobjectEnd,
	videoobjectStart,
	voidEnd,
	voidStart,
	volumenumEnd,
	volumenumStart,
	warningEnd,
	warningStart,
	wordaswordEnd,
	wordaswordStart,
	xrefEnd,
	xrefStart,
	yearEnd,
	yearStart
};

extern const char* docbookKeywords[];

template<typename ch, typename tr>
inline std::basic_ostream<ch, tr>&
operator<<( std::basic_ostream<ch, tr>& ostr, docbookToken v ) {
    if( v == 0 ) ostr << "eof";
    else ostr << docbookKeywords[(v-1)/2] << ( ((v-1) % 2) ? "Start" : "End");
    return ostr;
}


/** *docbookScanner* implements the lexical scanner used
    by the LL(1) parser used to drive the transformation
    of documents with docbook markups into HTML.
*/
class docbookScanner : public xmlTokListener {
public:
    typedef slice<const char> textSlice;

protected:
    bool elementStart;
    std::list<docbookToken> tokens;
    xmlTokenizer tok;

    virtual void appendText( const char* first, const char *last ) {}

    virtual void clearText() {}

 public:

    docbookScanner() : tok(*this) {
	tokens.push_back(bookEof);
    }

    virtual ~docbookScanner() {}

    void newline( const char *line, int first, int last ) {}
    
    void token( xmlToken token, const char *line, 
		int first, int last, bool fragment );

    bool eof() const { return tokens.empty(); }

    virtual docbookToken read() = 0;

    docbookToken current() const { 
	return eof() ? bookEof : *tokens.begin();
    }
};


/** Generates tokens out of a text buffer in memory. 
 */
class docbookBufferTokenizer : public docbookScanner {
protected:
    char *buffer;
    size_t length;
    size_t curr;    
    textSlice text;

    void appendText( const char* first, const char *last );
    void clearText();

public:
    docbookBufferTokenizer( char *buffer, size_t length );
    virtual ~docbookBufferTokenizer() {}

    void newline( const char *line, int first, int last );
    
    docbookToken read();
};


/** Generates tokens out of a input stream of characters.
 */
class docbookStreamTokenizer : public docbookScanner {
protected:
    std::istream *istr;

public:
    explicit docbookStreamTokenizer( std::istream& is );
    virtual ~docbookStreamTokenizer() {}

    docbookToken read();
};

}

#endif

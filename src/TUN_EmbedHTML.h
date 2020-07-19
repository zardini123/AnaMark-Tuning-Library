// TUN_EmbedHTML.h: Interface of class/functions for EmbedHTML TUN
//
// (C)opyright in 2009 by Mark Henning, Germany
//
// Contact: See contact form at www.mark-henning.de
//
// This file provides some functionality for searching/replacing
// tokens in strings and to embed TUN data sets in other text file
// formats.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TUN_EmbedHTML_H__D813FFD6_BF2A_4F98_9470_3D5C96644688__INCLUDED_)
#define AFX_TUN_EmbedHTML_H__D813FFD6_BF2A_4F98_9470_3D5C96644688__INCLUDED_





#pragma warning( disable : 4786 )

#include <cassert>
#include <string>
#include <list>

#include "TUN_Scale.h"





namespace TUN
{





class CTokenPos
{
public:
	CTokenPos(CSingleScale::eSection section, CSingleScale::eKey key);
	virtual ~CTokenPos() {}


	// Builds the token from section and key
	std::string GetToken() const;


	// Searches the token in str beginning at pos.
	// returns true, if the token is found and sets m_pos
	// otherwise returns false and sets m_pos to std::string::npos
	bool FindNext(const std::string & str, std::string::size_type pos);


	// Ensures that the found tokens can be sorted by descending order
	bool operator< (const CTokenPos & rhs) const;


	// Replaces the token in str at m_pos by its value from ss
	void ReplaceToken(std::string & str, CSingleScale & ss,
					  long lVersionFrom = 0, long lVersionTo = 200,
					  std::string strListItemSeparator = std::string("<br />")) const;


	// The variables
	CSingleScale::eSection	m_section;	// Section related to the token
	CSingleScale::eKey		m_key;		// Key related to the token
	std::string::size_type	m_pos;		// Position of the token in the string
}; // class CTokenPos





// Builds a list which contains all tokens supported
void BuildListOfSupportedTokens(std::list<CTokenPos> & ltpTokens);
// Writes a list of the supported tokens to a text file:
void WriteListOfSupportedTokens(std::string strFile);


// Builds a descending sorted list which contains all tokens found in str
void BuildListOfFoundTokens(const std::string & str,
							std::list<CTokenPos> & ltpTokens,
							std::list<CTokenPos> & ltpPositions);


// Replaces all Tokens found in str
void ReplaceListOfFoundTokens(std::string & str,
							  CSingleScale & ss,
							  const std::list<CTokenPos> & ltpPositions,
							  long lVersionFrom = 0, long lVersionTo = 200);


// Concatenates the strings of a string list and places
// strSeparator between the entries
std::string StringList2String(const std::list<std::string> & lstr,
							  std::string strListItemSeparator);


// Replaces some important chars by their HTML entities
std::string HTMLEntities(const std::string & str);





class CEmbedHTML
{
public:
	// strTemplate can be a file path or the template content itself
	CEmbedHTML(std::string strTemplate, bool bTemplateIsFile = true);
	virtual ~CEmbedHTML();


	// Error handling
	const CErr &	Err() const { return m_err; }
private:
	CErr	m_err;
public:



	// Embeds single TUN data set into the template and writes the file
	bool	WriteFile(std::string strScaleFile, std::string strEmbeddedFile,
					  long lVersionFrom = 0, long lVersionTo = 200);
	bool	WriteFile(CSingleScale & ssScale, std::string strEmbeddedFile,
					  long lVersionFrom = 0, long lVersionTo = 200);

private:
	std::string				m_strTemplate; // Here, the template content is stored
	std::list<CTokenPos>	m_ltpTokens;
	std::list<CTokenPos>	m_ltpPositions;
};





} // namespace TUN





#endif // !defined(AFX_TUN_EmbedHTML_H__D813FFD6_BF2A_4F98_9470_3D5C96644688__INCLUDED_)

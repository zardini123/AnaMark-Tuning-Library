// TUN_StringTools.h: Interface of string tool functions and class CStringParser.
//
// (C)opyright in 2009 by Mark Henning, Germany
//
// Contact: See contact form at www.mark-henning.de
//
// This file provides some string extensions for std::string
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TUN_STRINGTOOLS_H__D813FFD6_BF2A_4F98_9470_3D5C96644688__INCLUDED_)
#define AFX_TUN_STRINGTOOLS_H__D813FFD6_BF2A_4F98_9470_3D5C96644688__INCLUDED_





#pragma warning( disable : 4786 )

#include <cassert>
#include <cctype>
#include <cstring>
#include <string>
#include <list>
#include <iostream>





namespace TUN
{





namespace strx
{
//////////////////////////////////////////////////////////////////////
// String tool functions
//////////////////////////////////////////////////////////////////////



// List of characters which count as white spaces
const char * WhiteSpaceChars();

// Convert to lower chars
std::string & ToLower(std::string & str);
std::string	GetAsLower(const std::string & str);

// Remove leading/trailing white spaces
std::string & Trim(std::string & str);

// Remove white spaces within
std::string & RemoveSpaces(std::string & str);

// Escape string according to C-like syntax, e.g. tabulator -> "\t"
std::string & Escape(std::string & str);

// Unescape string according to C-like syntax, e.g. "\t" -> tabulator
std::string & Unescape(std::string & str);



//////////////////////////////////////////////////////////////////////
// String evaluation functions
//////////////////////////////////////////////////////////////////////



// Helper functions to retrieve double- and long-values from strings
// with error checking
bool Eval(const std::string & str, std::string::size_type & pos, double & dblResult);
bool Eval(const std::string & str, std::string::size_type & pos, long & lResult);

// Split *trimmed* string "key = value" into key and value and trim
// the results. Additionally, key is converted to lower chars.
// returns false if there is no '=' or key value does neither start
// with a letter nor with an underscore.
bool EvalKeyAndValue(std::string & str, std::string & strKey, std::string & strValue);

// If string is like "[sectionname]", '[' and ']' are removed, the string
// is trimmed and converted to lower. The return value is true.
// Otherwise, the string keeps unchanged and the return value is false.
bool EvalSection(std::string & str);

// If string is like "(Param1, Param2)", '(' and ')' are removed.
// The return value is true.
// Otherwise, the string keeps unchanged and the return value is false.
bool EvalFunctionParam(std::string & str);

// If string is like "\"an escaped string\"", the double quotes are
// removed, the string is unescaped and true is returned.
// Otherwise, the string keeps unchanged and the return value is false.
bool EvalString(std::string & str);

// Splits a string at the chSeparator into a list of strings
// bTrimItems = true: Items are trimmed
// bIgnoreEmptyItems = true: Empty entries are ignored
void Split(std::string & str, char chSeparator, std::list<std::string> & lstrResult,
		   bool bTrimItems, bool bIgnoreEmptyItems);



//////////////////////////////////////////////////////////////////////
// String construction functions
//////////////////////////////////////////////////////////////////////



// Convert double- and long-values to string
std::string ltostr(long lValue);
std::string dtostr(double dblValue);

// Adds '[' and ']' to the begin and the end, respectively
std::string	GetAsSection(const std::string & str);

// Escapes string and adds '\"' to the begin and the end
std::string	GetAsString(const std::string & str);





} // namespace strx





class CStringParser
{
public:
	CStringParser() {}
	virtual ~CStringParser() {}





	//////////////////////////////////////////////////////////////////////
	// Tool functions for working with streams
	//////////////////////////////////////////////////////////////////////


	// Call this before start reading from the stream
	void	InitStreamReading()
	{
		m_chEOL = '@';
		m_lLineCount = -1;
	}


	// Retrieve number of last read line or -1 if no line was read
	// since initialization
	long	GetLineCount() const { return m_lLineCount; }


	// Retrieves next line from stream and trim the result
	bool	GetLineAndTrim(std::istream & istr, long & lCurrLineCount)
	{
		m_strLine = "";
		m_strLine.reserve(1000); // Should be enough in most cases

		if ( !istr )
		{
			lCurrLineCount = m_lLineCount;
			return false; // an error occurred or EOF
		}

		// Read from stream, until '\n', '\r', '\0' or EOF is reached
		while ( istr )
		{
			char	ch = '\0';
			istr.read(&ch, 1);
			if ( (ch == '\0') || (ch == '\r') || (ch == '\n') )
			{
				// Remember first found EOL char to trigger line count correctly
				if ( m_chEOL == '@' )
					m_chEOL = ch;
				// Increase line count if trigger character is found
				if ( ch == m_chEOL )
					lCurrLineCount = ++m_lLineCount;
				break; // Done. Line end character reached
			}
			else
				m_strLine.append(1, ch);
		}

		strx::Trim(m_strLine);

		return true;
	}


	std::string & str() { return m_strLine; }
	const std::string & str() const { return m_strLine; }

	// Private variables for stream handling
private:
	char		m_chEOL;
	long		m_lLineCount;
	std::string	m_strLine;
}; // class CStringParser





} // namespace TUN





#endif // !defined(AFX_TUN_STRINGTOOLS_H__D813FFD6_BF2A_4F98_9470_3D5C96644688__INCLUDED_)

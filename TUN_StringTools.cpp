// TUN_StringTools.h: Implementation of string tool functions
//
// (C)opyright in 2009 by Mark Henning, Germany
//
// Contact: See contact form at www.mark-henning.de
//
// You may use this code for free. If you find an error or make some
// interesting changes, please let me know.
//
//////////////////////////////////////////////////////////////////////

#include "TUN_StringTools.h"





namespace TUN
{





namespace strx
{
//////////////////////////////////////////////////////////////////////
// String tool functions
//////////////////////////////////////////////////////////////////////





const char * WhiteSpaceChars()
{
	return "\x09\x0a\x0b\x0c\x0d\x20";
}



std::string & ToLower(std::string & str)
{
	for ( std::string::size_type l = 0 ; l < str.size() ; ++l )
		str.at(l) = static_cast<char>( tolower(str.at(l)) );
	return str;
}

std::string	GetAsLower(const std::string & str)
{
	return ToLower(std::string(str));
}



std::string & Trim(std::string & str)
{
	std::string::size_type	posFirst = str.find_first_not_of(WhiteSpaceChars());
	if ( posFirst == std::string::npos )
		str = "";
	else
		str = str.substr(posFirst, str.find_last_not_of(WhiteSpaceChars()) - posFirst + 1);
	return str;
}



std::string & RemoveSpaces(std::string & str)
{
	// We're doing in-place conversion here:
	std::string::size_type	posR = 0;
	std::string::size_type	posW = 0;

	while ( posR < str.size() )
	{
		char	ch = str.at(posR++);
		if ( !isspace(ch) )
			str.at(posW++) = ch;
	} // while ( posR < size() )

	// Remove the remaining chars of the original string
	str.erase(posW);

	return str;
}



std::string & Escape(std::string & str)
{
	std::string	strEsc;
	strEsc.reserve(str.size() * 2 + 1); // Avoids reallocation in most cases

	for ( std::string::size_type l = 0 ; l < str.size() ; ++l )
	{
		switch ( str.at(l) )
		{
		case '\0': strEsc += "\\0"; break; // Nullbyte
		case '\a': strEsc += "\\a"; break; // Bell (alert) 
		case '\b': strEsc += "\\b"; break; // Backspace 
		case '\f': strEsc += "\\f"; break; // Formfeed 
		case '\n': strEsc += "\\n"; break; // New line 
		case '\r': strEsc += "\\r"; break; // Carriage return 
		case '\t': strEsc += "\\t"; break; // Horizontal tab 
		case '\v': strEsc += "\\v"; break; // Vertical tab 
		case '\'': strEsc += "\\\'"; break; // Single quotation mark 
		case '\"': strEsc += "\\\""; break; // Double quotation mark 
		case '\\': strEsc += "\\\\"; break; // Backslash 
		case '\?': strEsc += "\\?"; break; // Literal question mark 
		default:
			if ( (static_cast<unsigned char>(str.at(l)) < 0x20) ||
				 (static_cast<unsigned char>(str.at(l)) == 0xff) )
			{
				char	szHex[3] = "00";
				strEsc += "\\x0";
				strEsc += ltoa(static_cast<unsigned char>(str.at(l)), szHex, 16);
			}
			else
				strEsc += str.at(l);
		}
	}

	str = strEsc;

	return str;
}



std::string & Unescape(std::string & str)
{
	// We're doing in-place conversion here:
	std::string::size_type	posR = 0;
	std::string::size_type	posW = 0;

	while ( posR < str.size() )
	{
		char	ch = str.at(posR++);

		if ( (ch == '\\') && (posR < str.size()) )
			switch ( ch = str.at(posR++) )
			{
				case '0': ch = '\0'; break; // Nullbyte
				case 'a': ch = '\a'; break; // Bell (alert) 
				case 'b': ch = '\b'; break; // Backspace 
				case 'f': ch = '\f'; break; // Formfeed 
				case 'n': ch = '\n'; break; // New line 
				case 'r': ch = '\r'; break; // Carriage return 
				case 't': ch = '\t'; break; // Horizontal tab 
				case 'v': ch = '\v'; break; // Vertical tab 
				case '\'': ch = '\''; break; // Single quotation mark 
				case '\"': ch = '\"'; break; // Double quotation mark 
				case '\\': ch = '\\'; break; // Backslash 
				case '?': ch = '\?'; break; // Literal question mark 
				case 'x': // Hex representation
					ch = strtol(("0x0" + str.substr(posR, 3)).c_str(), NULL, 16);
					posR += 3;
					break;
			} // switch ( at(posR) )

		str.at(posW++) = ch;
	} // while ( posR < size() )

	// Remove the remaining chars of the original string
	str.erase(posW);

	return str;
}





//////////////////////////////////////////////////////////////////////
// String evaluation functions
//////////////////////////////////////////////////////////////////////





bool Eval(const std::string & str, std::string::size_type & pos, double & dblResult)
{
	const char	* szBeginPtr = str.c_str() + pos;
	char		* szEndPtr;
	dblResult = strtod(szBeginPtr, &szEndPtr); // conversion
	pos += szEndPtr - szBeginPtr; // points to the next char
	return (szBeginPtr != szEndPtr); // return false if an error occurred
}



bool Eval(const std::string & str, std::string::size_type & pos, long & lResult)
{
	const char	* szBeginPtr = str.c_str() + pos;
	char		* szEndPtr;
	lResult = strtol(szBeginPtr, &szEndPtr, 10); // conversion
	pos += szEndPtr - szBeginPtr; // points to the next char
	return (szBeginPtr != szEndPtr); // return false if an error occurred
}



bool EvalKeyAndValue(std::string & str, std::string & strKey, std::string & strValue)
{
	std::string::size_type	pos = str.find('=');
	if ( (pos == std::string::npos) || (!__iscsymf(str.at(0))) )
		return false; // error: no '=' or first char of key is invalid

	strKey = ToLower(Trim(str.substr(0, pos)));
	strValue = Trim(str.substr(pos+1));
	return true;
}



bool EvalSection(std::string & str)
{
	if ( (str.size() < 2) || (str.at(0) != '[') || (str.at(str.size()-1) != ']') )
		return false;
	str = ToLower(Trim(str.substr(1, str.size()-2)));
	return true;
}



bool EvalFunctionParam(std::string & str)
{
	if ( (str.size() < 2) || (str.at(0) != '(') || (str.at(str.size()-1) != ')') )
		return false;
	str = str.substr(1, str.size()-2);
	return true;
}



bool EvalString(std::string & str)
{
	if ( (str.size() < 2) || (str.at(0) != '\"') || (str.at(str.size()-1) != '\"') )
		return false;
	str = Unescape(str.substr(1, str.size()-2));
	return true;
}



void Split(std::string & str, char chSeparator, std::list<std::string> & lstrResult,
		   bool bTrimItems, bool bIgnoreEmptyItems)
{
	// Initialize list
	lstrResult.clear();

	// Split string
	std::string::size_type	posCurr = 0;
	std::string::size_type	posSep = 0;

	while ( true )
	{
		if ( posSep == std::string::npos )
			return;

		// Find the next separator and extract the item
		posSep = str.find(chSeparator, posCurr);
		std::string	strCurr;
		if ( posSep == std::string::npos )
			strCurr = str.substr(posCurr);
		else
			strCurr = str.substr(posCurr, posSep - posCurr);
		posCurr = posSep + 1;

		// Process the item
		if ( bTrimItems )
			Trim(strCurr);
		if ( bIgnoreEmptyItems && strCurr.empty() )
			continue;
		lstrResult.push_back(strCurr);
	}
}





//////////////////////////////////////////////////////////////////////
// String construction functions
//////////////////////////////////////////////////////////////////////





// Convert double- and long-values to string
std::string ltostr(long lValue)
{
	char	sz[11]; // Enough for 32-Bit long
	return std::string(ltoa(lValue, sz, 10));
}



std::string dtostr(double dblValue)
{
	char	sz[30]; // Enough for 64-Bit double
	return std::string(_gcvt(dblValue, 20, sz));
}



std::string	GetAsSection(const std::string & str)
{
	return "[" + str + "]";
}



std::string	GetAsString(const std::string & str)
{
	return "\"" + Escape(std::string(str)) + "\"";
}





} // namespace strx
} // namespace TUN

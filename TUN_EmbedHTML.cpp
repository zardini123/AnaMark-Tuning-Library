// TUN_EmbedHTML.cpp: Implementation of class/functions for EmbedHTML TUN
//
// (C)opyright in 2009 by Mark Henning, Germany
//
// Contact: See contact form at www.mark-henning.de
//
// You may use this code for free. If you find an error or make some
// interesting changes, please let me know.
//
//////////////////////////////////////////////////////////////////////

#include <sstream>

#include "TUN_EmbedHTML.h"





namespace TUN
{





//////////////////////////////////////////////////////////////////////
// class CTokenPos
//////////////////////////////////////////////////////////////////////





CTokenPos::CTokenPos(CSingleScale::eSection section,
					 CSingleScale::eKey key) :
	m_section(section),
	m_key(key),
	m_pos(0)
{
}



std::string CTokenPos::GetToken() const
{
	return "@@@"
		+ CSingleScale::GetSections().at(m_section)
		+ ":"
		+ CSingleScale::GetKeys().at(m_key)
		+ "@@@";
}



bool CTokenPos::FindNext(const std::string & str, std::string::size_type pos)
{
	m_pos = str.find(GetToken(), pos);
	return ( m_pos != std::string::npos );
}



bool CTokenPos::operator< (const CTokenPos & rhs) const
{
	return m_pos > rhs.m_pos;
}



void CTokenPos::ReplaceToken(std::string & str,
							 CSingleScale & ss,
							 long lVersionFrom /* = 0 */,
							 long lVersionTo /* = 200 */,
							 std::string strListItemSeparator /* = std::string("<br />") */) const
{
	// Erase the token
	str.erase(m_pos, GetToken().size());

	// Retrieve the token value
	std::string	strValue;
	switch ( m_section )
	{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	case CSingleScale::SEC_ScaleBegin:
		switch ( m_key )
		{
		case CSingleScale::KEY_Format:
			strValue = ss.Format();
			break;
		case CSingleScale::KEY_FormatVersion:
			strValue = ss.FormatVersionAsStr();
			break;
		case CSingleScale::KEY_FormatSpecs:
			strValue = ss.FormatSpecs();
			break;
		default:
			assert(false); // key not implemented
		} // switch ( m_key )
		break;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	case CSingleScale::SEC_Info:
		switch ( m_key )
		{
		case CSingleScale::KEY_Name:
			strValue = HTMLEntities(ss.m_strName);
			break;
		case CSingleScale::KEY_ID:
			strValue = HTMLEntities(ss.m_strID);
			break;
		case CSingleScale::KEY_Filename:
			strValue = HTMLEntities(ss.m_strFilename);
			break;
		case CSingleScale::KEY_Author:
			strValue = HTMLEntities(ss.m_strAuthor);
			break;
		case CSingleScale::KEY_Location:
			strValue = HTMLEntities(ss.m_strLocation);
			break;
		case CSingleScale::KEY_Contact:
			strValue = HTMLEntities(ss.m_strContact);
			break;
		case CSingleScale::KEY_Date:
			strValue = HTMLEntities(ss.GetDate());
			break;
		case CSingleScale::KEY_Editor:
			strValue = HTMLEntities(ss.m_strEditor);
			break;
		case CSingleScale::KEY_EditorSpecs:
			strValue = HTMLEntities(ss.m_strEditorSpecs);
			break;
		case CSingleScale::KEY_Description:
			strValue = HTMLEntities(ss.m_strDescription);
			break;
		case CSingleScale::KEY_Keyword:
			strValue = StringList2String(ss.m_lstrKeywords, strListItemSeparator);
			break;
		case CSingleScale::KEY_History:
			strValue = HTMLEntities(ss.m_strHistory);
			break;
		case CSingleScale::KEY_Geography:
			strValue = HTMLEntities(ss.m_strGeography);
			break;
		case CSingleScale::KEY_Instrument:
			strValue = HTMLEntities(ss.m_strInstrument);
			break;
		case CSingleScale::KEY_Composition:
			strValue = StringList2String(ss.m_lstrCompositions, strListItemSeparator);
			break;
		case CSingleScale::KEY_Comments:
			strValue = HTMLEntities(ss.m_strComments);
			break;
		default:
			assert(false); // key not implemented
		} // switch ( m_key )
		break;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	case CSingleScale::SEC_DataSet:
		switch ( m_key )
		{
		case CSingleScale::KEY_AllData:
			{
				std::stringstream	strstr;
				ss.Write(strstr, lVersionFrom, lVersionTo);
				strValue = strstr.str() + "__FakeKey = ";
			}
			break;
		default:
			assert(false); // key not implemented
		} // switch ( m_key )
		break;

	default:
		assert(false); // section not implemented
	} // switch ( m_section )

	// Insert the token value
	str.insert(m_pos, strValue);
}





//////////////////////////////////////////////////////////////////////
// Tool functions
//////////////////////////////////////////////////////////////////////





void BuildListOfSupportedTokens(std::list<CTokenPos> & ltpTokens)
{
	ltpTokens.clear();
	ltpTokens.push_back(CTokenPos(CSingleScale::SEC_ScaleBegin, CSingleScale::KEY_Format));
	ltpTokens.push_back(CTokenPos(CSingleScale::SEC_ScaleBegin, CSingleScale::KEY_FormatVersion));
	ltpTokens.push_back(CTokenPos(CSingleScale::SEC_ScaleBegin, CSingleScale::KEY_FormatSpecs));
	ltpTokens.push_back(CTokenPos(CSingleScale::SEC_Info, CSingleScale::KEY_Name));
	ltpTokens.push_back(CTokenPos(CSingleScale::SEC_Info, CSingleScale::KEY_ID));
	ltpTokens.push_back(CTokenPos(CSingleScale::SEC_Info, CSingleScale::KEY_Filename));
	ltpTokens.push_back(CTokenPos(CSingleScale::SEC_Info, CSingleScale::KEY_Author));
	ltpTokens.push_back(CTokenPos(CSingleScale::SEC_Info, CSingleScale::KEY_Location));
	ltpTokens.push_back(CTokenPos(CSingleScale::SEC_Info, CSingleScale::KEY_Contact));
	ltpTokens.push_back(CTokenPos(CSingleScale::SEC_Info, CSingleScale::KEY_Date));
	ltpTokens.push_back(CTokenPos(CSingleScale::SEC_Info, CSingleScale::KEY_Editor));
	ltpTokens.push_back(CTokenPos(CSingleScale::SEC_Info, CSingleScale::KEY_EditorSpecs));
	ltpTokens.push_back(CTokenPos(CSingleScale::SEC_Info, CSingleScale::KEY_Description));
	ltpTokens.push_back(CTokenPos(CSingleScale::SEC_Info, CSingleScale::KEY_Keyword));
	ltpTokens.push_back(CTokenPos(CSingleScale::SEC_Info, CSingleScale::KEY_History));
	ltpTokens.push_back(CTokenPos(CSingleScale::SEC_Info, CSingleScale::KEY_Geography));
	ltpTokens.push_back(CTokenPos(CSingleScale::SEC_Info, CSingleScale::KEY_Instrument));
	ltpTokens.push_back(CTokenPos(CSingleScale::SEC_Info, CSingleScale::KEY_Composition));
	ltpTokens.push_back(CTokenPos(CSingleScale::SEC_Info, CSingleScale::KEY_Comments));
	ltpTokens.push_back(CTokenPos(CSingleScale::SEC_DataSet, CSingleScale::KEY_AllData));
}



void WriteListOfSupportedTokens(std::string strFile)
{
	std::list<CTokenPos>	ltpTokens;
	BuildListOfSupportedTokens(ltpTokens);

	std::ofstream					ofs(strFile.c_str(), std::ios_base::out | std::ios_base::trunc);
	std::list<CTokenPos>::iterator	itTokens;
	for ( itTokens = ltpTokens.begin() ; itTokens != ltpTokens.end() ; ++itTokens )
		ofs << itTokens->GetToken().c_str() << std::endl;
	ofs.close();
}



void BuildListOfFoundTokens(const std::string & str,
							std::list<CTokenPos> & ltpTokens,
							std::list<CTokenPos> & ltpPositions)
{
	std::list<CTokenPos>::iterator	itTokens;
	for ( itTokens = ltpTokens.begin() ; itTokens != ltpTokens.end() ; ++itTokens )
	{
		itTokens->m_pos = -1;
		while ( itTokens->FindNext(str, ++itTokens->m_pos) )
			ltpPositions.push_back(*itTokens);
	}
	ltpPositions.sort();
}



void ReplaceListOfFoundTokens(std::string & str,
							  CSingleScale & ss,
							  const std::list<CTokenPos> & ltpPositions,
							  long lVersionFrom /* = 0 */,
							  long lVersionTo /* = 200 */)
{
	std::list<CTokenPos>::const_iterator	itPos;
	for ( itPos = ltpPositions.begin() ; itPos != ltpPositions.end() ; ++itPos )
		itPos->ReplaceToken(str, ss, lVersionFrom, lVersionTo);
}



std::string StringList2String(const std::list<std::string> & lstr,
							  std::string strListItemSeparator)
{
	std::string								strResult;
	std::list<std::string>::const_iterator	it;
	for ( it = lstr.begin() ; it != lstr.end() ; ++it )
	{
		if ( !strResult.empty() )
			strResult += strListItemSeparator;
		strResult += HTMLEntities(*it);
	}
	return strResult;
}



std::string HTMLEntities(const std::string & str)
{
	std::string	strResult;
	strResult.reserve(str.size());

	for ( long l = 0 ; l < str.size() ; ++l )
		switch ( str.at(l) )
		{
		case '\0':	strResult += "<br />"; strResult += '\0'; break;
		case '\n':	strResult += "\n<br />"; break;
		case '\"':	strResult += "&quot;"; break;
		case '&':	strResult += "&amp;"; break;
		case '<':	strResult += "&lt;"; break;
		case '>':	strResult += "&gt;"; break;
		default:	strResult += str.at(l);
		}

	return strResult;
}





//////////////////////////////////////////////////////////////////////
// class CEmbedHTML
//////////////////////////////////////////////////////////////////////





CEmbedHTML::CEmbedHTML(std::string strTemplate, bool bTemplateIsFile /* = true */)
{
	// Retrieves Template content
	if ( bTemplateIsFile )
	{
		std::ifstream		ifstr(strTemplate.c_str(), std::ios_base::in | std::ios_base::binary);
		std::stringstream	strstr;
		strstr << ifstr.rdbuf();
		m_strTemplate = strstr.str(); // strstrstrstrstrstr... :-)
		ifstr.close();
	}
	else
		m_strTemplate = strTemplate;

	// Build the list of tokens
	BuildListOfSupportedTokens(m_ltpTokens);

	// Search for the tokens in the template
	BuildListOfFoundTokens(m_strTemplate, m_ltpTokens, m_ltpPositions);
}



CEmbedHTML::~CEmbedHTML()
{
}



bool CEmbedHTML::WriteFile(std::string strScaleFile, std::string strEmbeddedFile,
						   long lVersionFrom /* = 0 */, long lVersionTo /* = 200 */)
{
	// Load file
	CSingleScale	ss;
	if ( !ss.Read(strScaleFile.c_str()) )
	{
		m_err = ss.Err();
		return false;
	}

	return WriteFile(ss, strEmbeddedFile, lVersionFrom, lVersionTo);
}



bool CEmbedHTML::WriteFile(CSingleScale & ssScale, std::string strEmbeddedFile,
						   long lVersionFrom /* = 0 */, long lVersionTo /* = 200 */)
{
	// Embed data
	std::string	strEmbeddedData = m_strTemplate;
	ReplaceListOfFoundTokens(strEmbeddedData, ssScale, m_ltpPositions,
							 lVersionFrom, lVersionTo);

	// Write result
	std::ofstream	ofs(strEmbeddedFile.c_str(), std::ios_base::out | std::ios_base::trunc);
	ofs << strEmbeddedData;
	if ( !ofs )
		return m_err.SetError("Error writing the file.");
	ofs.close();

	return m_err.SetOK();
}





} // namespace TUN

// TUN_Scale.cpp: Implementation of the class CSingleScale.
//
// (C)opyright in 2003-2009 by Mark Henning, Germany
//
// Contact: See contact page at www.mark-henning.de
//
// You may use this code for free. If you find an error or make some
// interesting changes, please let me know.
//
// This class deals with the reading/writing of AnaMark tuning files
// according to specifications V2.00.
// Be carefull with changes of the functions
// Write/Read because this may lead to incompatibilities!
//
// I think, the source-code is rather self-explaining.
//
// The specifications of the AnaMark tuning files (V2.00) and also
// the specifications of version 1 (AnaMark / VAZ 1.5 Plus-compatible
// tuning file format) can be found at http://www.anamark.de
//
// IMPORTANT:
// Please note the version and naming history:
//
//		Version		Tuning file format name
//		   0		VAZ 1.5 Plus tuning files
//		   1		AnaMark / VAZ 1.5 Plus-compatible tuning files
//		   2		AnaMark tuning file V2.00
//
// Have fun!
//
//
//////////////////////////////////////////////////////////////////////
//
// Source code history:
// ====================
//
// 2009-02-14, V1.0:
// - First release
//
//////////////////////////////////////////////////////////////////////

#include <cctype>
#include <cassert>
#include <cstring>
#include <cmath>

#include "TUN_Scale.h"





namespace TUN
{





//////////////////////////////////////////////////////////////////////
// Constants / Tool functions
//////////////////////////////////////////////////////////////////////





const long		MaxNumOfNotes = 128;	// 0 - 127
const double	DefaultBaseFreqHz = 8.1757989156437073336; // refers to A=440Hz



double Hz2Cents(double dblHz, double dblBaseFreqHz)
{
	return Factor2Cents(dblHz/dblBaseFreqHz);
}

double Cents2Hz(double dblCents, double dblBaseFreqHz)
{
	return dblBaseFreqHz * Cents2Factor(dblCents);
}

double Cents2Factor(double dblCents)
{
	return pow(2, dblCents/1200);
}

double Factor2Cents(double dblFactor)
{
	return log(dblFactor) * (1200/log(2));
}

double MIDINote_DefaultHz(int nMIDINote)
{
	return Cents2Hz(MIDINote_DefaultCents(nMIDINote), DefaultBaseFreqHz);
}

double MIDINote_DefaultCents(int nMIDINote)
{
	if ( nMIDINote < 0 )
		nMIDINote = 0;
	if ( nMIDINote > 127 )
		nMIDINote = 127;
	return nMIDINote * 100;
}





//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////





std::vector<std::string>	CSingleScale::m_vstrSections;
std::vector<std::string>	CSingleScale::m_vstrKeys;



CSingleScale	ssTemporary___; // To ensure that the static vectors are initialized!



CSingleScale::CSingleScale()
{
	// If not yet done, initialize vectors with sections and keys
	if ( m_vstrSections.empty() )
	{
		m_vstrSections.resize(SEC_NumOfSections);
		m_vstrSections.at(SEC_ScaleBegin) = "Scale Begin";
		m_vstrSections.at(SEC_ScaleEnd) = "Scale End";

		m_vstrSections.at(SEC_Info) = "Info";
		m_vstrSections.at(SEC_EditorSpecifics) = "Editor Specifics";

		m_vstrSections.at(SEC_Tuning) = "Tuning";
		m_vstrSections.at(SEC_ExactTuning) = "Exact Tuning";
		m_vstrSections.at(SEC_FunctionalTuning) = "Functional Tuning";

		m_vstrSections.at(SEC_Mapping) = "Mapping";

		m_vstrSections.at(SEC_Assignment) = "Assignment";

		m_vstrSections.at(SEC_DataSet) = "_DataSet_";
	}
	if ( m_vstrKeys.empty() )
	{
		m_vstrKeys.resize(KEY_NumOfKeys);
		// Keys of section [Scale Begin]
		m_vstrKeys.at(KEY_Format) = "Format";
		m_vstrKeys.at(KEY_FormatVersion) = "FormatVersion";
		m_vstrKeys.at(KEY_FormatSpecs) = "FormatSpecs";
		// Keys of section [Info]
		m_vstrKeys.at(KEY_Name) = "Name";
		m_vstrKeys.at(KEY_ID) = "ID";
		m_vstrKeys.at(KEY_Filename) = "Filename";
		m_vstrKeys.at(KEY_Author) = "Author";
		m_vstrKeys.at(KEY_Location) = "Location";
		m_vstrKeys.at(KEY_Contact) = "Contact";
		m_vstrKeys.at(KEY_Date) = "Date";
		m_vstrKeys.at(KEY_Editor) = "Editor";
		m_vstrKeys.at(KEY_EditorSpecs) = "EditorSpecs";
		m_vstrKeys.at(KEY_Description) = "Description";
		m_vstrKeys.at(KEY_Keyword) = "Keyword";
		m_vstrKeys.at(KEY_History) = "History";
		m_vstrKeys.at(KEY_Geography) = "Geography";
		m_vstrKeys.at(KEY_Instrument) = "Instrument";
		m_vstrKeys.at(KEY_Composition) = "Composition";
		m_vstrKeys.at(KEY_Comments) = "Comments";
		// Keys of sections [Tuning] and [Exact Tuning]
		m_vstrKeys.at(KEY_Note) = "Note";
		m_vstrKeys.at(KEY_BaseFreq) = "BaseFreq";
		m_vstrKeys.at(KEY_InitEqual) = "InitEqual";
		// Keys of sections [Mapping]
		m_vstrKeys.at(KEY_LoopSize) = "LoopSize";
		m_vstrKeys.at(KEY_Keyboard) = "Keyboard";
		// Keys of sections [Assignment]
		m_vstrKeys.at(KEY_MIDIChannel) = "MIDIChannel";
		// Keys of sections [_DataSet_]
		m_vstrKeys.at(KEY_AllData) = "AllData";
	}
	// Provide a standard tuning
	Reset();
}



CSingleScale::~CSingleScale()
{
}





//////////////////////////////////////////////////////////////////////
// Initialize scale
//////////////////////////////////////////////////////////////////////





void CSingleScale::Reset()
{
	m_err.SetOK();

	// Keys of section [Scale Begin]
	m_strFormat = "";
	m_lFormatVersion = 0;
	m_strFormatSpecs = "";

	// Keys of sections [Assignment]
	m_lmcrChannels.clear();

	// Keys of section [Info]
	m_strName = "";
	m_strID = "";
	m_strFilename = "";
	m_strAuthor = "";
	m_strLocation = "";
	m_strContact = "";
	m_strDate = "";
	m_strEditor = "";
	m_strDescription = "";
	m_lstrKeywords.clear();
	m_strHistory = "";
	m_strGeography = "";
	m_strInstrument = "";
	m_lstrCompositions.clear();
	m_strComments = "";

	// Resize vectors for note frequencies
	m_vdblNoteFrequenciesHz.resize(MaxNumOfNotes);

	// Initialize scale frequencies
	InitEqual();

	// Initialize mapping
	ResetKeyboardMapping();
}



void CSingleScale::ResetKeyboardMapping()
{
	// Initialize mapping
	m_vlMapping.resize(MaxNumOfNotes);
	for ( int i = 0 ; i < MaxNumOfNotes ; ++i )
		m_vlMapping.at(i) = i;
	m_lMappingLoopSize = 0;
}



void CSingleScale::InitEqual(long lBaseNote /* = 69 */,
							 double dblBaseFreqHz /* = 440 */)
{
	m_lInitEqual_BaseNote = lBaseNote;
	m_dblInitEqual_BaseFreqHz = dblBaseFreqHz;

	for ( int i = 0 ; i < MaxNumOfNotes ; ++i )
		m_vdblNoteFrequenciesHz.at(i) =
			m_dblInitEqual_BaseFreqHz * pow(2, (i-lBaseNote) / 12.);

	// Clear formulas
	m_lformulas.clear();
}





//////////////////////////////////////////////////////////////////////
// Accessing the scale
//////////////////////////////////////////////////////////////////////





void CSingleScale::AddFormula(CFormula formula)
{
	formula.Apply(m_vdblNoteFrequenciesHz);
	m_lformulas.push_back(formula);
}



void CSingleScale::SetMappingLoopSize(long lMappingLoopSize)
{
	if ( lMappingLoopSize < 1 )
		lMappingLoopSize = 0;
	m_lMappingLoopSize = lMappingLoopSize;
}



long CSingleScale::MapMIDI2Scale(long lMIDINoteNumber) const
{
	if ( m_lMappingLoopSize <= 0 )
		return m_vlMapping.at(lMIDINoteNumber);
	else
	{
		long	lOctave = lMIDINoteNumber / m_lMappingLoopSize;
		long	lOffset = lMIDINoteNumber % m_lMappingLoopSize;
		long	lScaleNoteNumber = m_vlMapping.at(lOffset) + lOctave * m_lMappingLoopSize;
		if ( lScaleNoteNumber < 0 )
			lScaleNoteNumber = 0;
		if ( lScaleNoteNumber >= MaxNumOfNotes )
			lScaleNoteNumber = MaxNumOfNotes-1;
		return lScaleNoteNumber;
	}
}





//////////////////////////////////////////////////////////////////////
// Read/write files
//////////////////////////////////////////////////////////////////////





bool CSingleScale::Write(const char * szFilepath,
						 long lVersionFrom /* = 0 */,
						 long lVersionTo /* = 200 */,
						 bool bWriteHeaderComment /* = true */)
{
	std::ofstream	ofs(szFilepath, std::ios_base::out | std::ios_base::trunc);
	return Write(ofs, lVersionFrom, lVersionTo, bWriteHeaderComment);
}

bool CSingleScale::Write(std::ostream & os,
						 long lVersionFrom /* = 0 */,
						 long lVersionTo /* = 200 */,
						 bool bWriteHeaderComment /* = true */)
{
	// Evaluate which sections to write
	if ( lVersionFrom < 0 )
		lVersionFrom = 0;
	if ( lVersionTo > 200 )
		lVersionTo = 200;
	if ( lVersionFrom > lVersionTo )
		return m_err.SetError("Error in version settings - file not written."); // Versions to write mismatch

	bool	bV000 = ((lVersionFrom <= 0) && (lVersionTo >= 0));
	bool	bV100 = ((lVersionFrom <= 100) && (lVersionTo >= 100));
	bool	bV200 = ((lVersionFrom <= 200) && (lVersionTo >= 200));

	int				i;

	// Header comment
	if ( bV100 || bV200 )
	{
		os << ";" << std::endl;
		os << "; This is an AnaMark tuning map file V2.00" << std::endl;
		if ( !bV200 )
			os << "; written in V1.00 compatibility mode" << std::endl;
		os << ";" << std::endl;
		os << "; Free .TUN file handling source code (C)2009 by Mark Henning, Germany" << std::endl;
		os << ";" << std::endl;
		os << "; Specifications and free source code see:" << std::endl;
		os << ";         " << FormatSpecs() << std::endl;
		os << ";" << std::endl;
		os << std::endl;
		os << std::endl;
	}

	// Section [Scale Begin]
	if ( bV100 || bV200 )
	{
		os << ";" << std::endl;
		os << "; Begin of tuning file and format declaration" << std::endl;
		os << ";" << std::endl;
		WriteSection(os, SEC_ScaleBegin);
		WriteKey(os, KEY_Format, std::string(Format()));
		WriteKey(os, KEY_FormatVersion, long(FormatVersion()));
		WriteKey(os, KEY_FormatSpecs, std::string(FormatSpecs()));
		os << std::endl;
		os << std::endl;
	}

	// Section [Assignment]
	if ( bV200 && !m_lmcrChannels.empty() ) // Versions below 200 do not support Multi Scale Files!
	{
		os << ";" << std::endl;
		os << "; Assignment of scale dataset" << std::endl;
		os << "; Note: This might be ignored, if this is not part of a MSF-File!" << std::endl;
		os << "; See the documentation of the software you use for details." << std::endl;
		os << ";" << std::endl;
		WriteSection(os, SEC_Assignment);
		WriteKey(os, KEY_MIDIChannel, GetMIDIChannelsAssignment());
		os << std::endl;
		os << std::endl;
	}

	// Section [Info]
	if ( bV100 || bV200 )
	{
		os << ";" << std::endl;
		os << "; Scale informations" << std::endl;
		os << ";" << std::endl;
		WriteSection(os, SEC_Info);
		WriteKey(os, KEY_Name, m_strName);
		WriteKey(os, KEY_ID, m_strID);
		WriteKey(os, KEY_Filename, m_strFilename);
		WriteKey(os, KEY_Author, m_strAuthor);
		WriteKey(os, KEY_Location, m_strLocation);
		WriteKey(os, KEY_Contact, m_strContact);
		WriteKey(os, KEY_Date, m_strDate);
		WriteKey(os, KEY_Editor, m_strEditor);
		WriteKey(os, KEY_EditorSpecs, m_strEditorSpecs);
		WriteKey(os, KEY_Description, m_strDescription);
		WriteKey(os, KEY_Keyword, m_lstrKeywords);
		WriteKey(os, KEY_History, m_strHistory);
		WriteKey(os, KEY_Geography, m_strGeography);
		WriteKey(os, KEY_Instrument, m_strInstrument);
		WriteKey(os, KEY_Composition, m_lstrCompositions);
		WriteKey(os, KEY_Comments, m_strComments);
		os << std::endl;
		os << std::endl;
	}

	// You might write some editor specific data here...
	if ( bV100 || bV200 )
	{
		// Currently we don't have such specific data, so don't write the section
		if ( false )
		{
			os << ";" << std::endl;
			os << "; Editor specific data" << std::endl;
			os << ";" << std::endl;
			WriteSection(os, SEC_EditorSpecifics);
			os << std::endl;
			os << std::endl;
		}
	}

	// Section [Functional Tuning]
	if ( bV200 )
	{
		os << ";" << std::endl;
		os << "; Version 2:" << std::endl;
		os << "; Functional tunings" << std::endl;
		os << ";" << std::endl;
		WriteSection(os, SEC_FunctionalTuning);
		os.precision(10);
		os << m_vstrKeys.at(KEY_InitEqual).c_str()
		   << " = (" << m_lInitEqual_BaseNote
		   << ", " << m_dblInitEqual_BaseFreqHz
		   << ")" << std::endl;
		std::list<CFormula>::const_iterator	it;
		for ( it = m_lformulas.begin() ; it != m_lformulas.end() ; ++it )
			WriteKey(os, KEY_Note, it->GetAsStr(), it->GetMyIndex());
		os << std::endl;
		os << std::endl;
	}

	// Section [Exact Tuning]
	if ( bV100 )
	{
		double	dblET_BaseFreqHz = m_dblInitEqual_BaseFreqHz * pow(2, -m_lInitEqual_BaseNote / 12.);

		os << ";" << std::endl;
		os << "; Version 1:" << std::endl;
		os << "; AnaMark-specific section with exact tunings" << std::endl;
		os << ";" << std::endl;
		WriteSection(os, SEC_ExactTuning);
		os.precision(10);
		WriteKey(os, KEY_BaseFreq, dblET_BaseFreqHz);
		for ( i = 0 ; i < MaxNumOfNotes ; ++i )
			WriteKey(os, KEY_Note,
					 Hz2Cents(m_vdblNoteFrequenciesHz.at(MapMIDI2Scale(i)),
							  dblET_BaseFreqHz),
					 i);
		os << std::endl;
		os << std::endl;
	}

	// Section [Tuning]
	if ( bV000 )
	{
		os << ";" << std::endl;
		os << "; Version 0:" << std::endl;
		os << "; VAZ-section with quantized tunings" << std::endl;
		os << ";" << std::endl;
		WriteSection(os, SEC_Tuning);
		for ( i = 0 ; i < MaxNumOfNotes ; ++i )
			WriteKey(os, KEY_Note,
					 long(static_cast<long>(floor(
						Hz2Cents(m_vdblNoteFrequenciesHz.at(MapMIDI2Scale(i)),
								 DefaultBaseFreqHz) + 0.5
					 ))),
					 i);
		os << std::endl;
		os << std::endl;
	}

	// Section [Mapping]
	if ( bV200 )
	{
		// Write only if needed:
		bool	bNeedsMapping = false;
		long	lMapSize = ( (m_lMappingLoopSize <= 0) || (m_lMappingLoopSize >= MaxNumOfNotes) ? MaxNumOfNotes : m_lMappingLoopSize);
		for ( i = 0 ; i < lMapSize ; ++i )
			bNeedsMapping |= ( m_vlMapping.at(i) != i );

		if ( bNeedsMapping )
		{
			os << ";" << std::endl;
			os << "; Keyboard mapping: Keyboard note number -> scale note number" << std::endl;
			os << ";" << std::endl;
			WriteSection(os, SEC_Mapping);
			WriteKey(os, KEY_LoopSize, m_lMappingLoopSize);
			for ( i = 0 ; i < lMapSize ; ++i )
			{
				if ( m_vlMapping.at(i) != i )
					WriteKey(os, KEY_Keyboard, m_vlMapping.at(i), i);
			}
			os << std::endl;
			os << std::endl;
		}
	}
	else
	{
		if ( bV100 )
		{
			os << ";" << std::endl;
			os << "; In V1.00 compatibility mode, there is no explicit keyboard mapping" << std::endl;
			os << "; The order of frequencies above includes keyboard mapping settings." << std::endl;
			os << ";" << std::endl;
			os << std::endl;
			os << std::endl;
		}
	}

	// Section [Scale End]
	if ( bV100 || bV200 )
	{
		os << ";" << std::endl;
		os << "; End of tuning file" << std::endl;
		os << ";" << std::endl;
		WriteSection(os, SEC_ScaleEnd);
		os << std::endl;
		os << std::endl;
	}

	return m_err.SetOK();
}



void CSingleScale::WriteSection(std::ostream & os, eSection section) const
{
	if ( (section <= SEC_Unknown) || (section >= SEC_NumOfSections) )
		return;

	os << strx::GetAsSection(m_vstrSections.at(section)).c_str() << std::endl;
}



void CSingleScale::WriteKey(std::ostream & os, eKey key,
							const std::list<std::string> & lstrValues) const
{
	if ( (key <= KEY_Unknown) || (key >= KEY_NumOfKeys) || (lstrValues.empty()) )
		return;

	std::list<std::string>::const_iterator	it;
	for ( it = lstrValues.begin() ; it != lstrValues.end() ; ++it )
	{
		if ( it->empty() )
			continue;
		os << m_vstrKeys.at(key).c_str()
			<< " = " << strx::GetAsString(*it).c_str() << std::endl;
	}
}



void CSingleScale::WriteKey(std::ostream & os, eKey key,
							const std::string & strValue, long lKeyIndex /* = -1 */) const
{
	if ( (key <= KEY_Unknown) || (key >= KEY_NumOfKeys) || (strValue.empty()) )
		return;

	os << m_vstrKeys.at(key).c_str();
	if ( (key == KEY_Note) || (key == KEY_Keyboard) )
		os << " " << lKeyIndex;
	os << " = " << strx::GetAsString(strValue).c_str() << std::endl;
}



void CSingleScale::WriteKey(std::ostream & os, eKey key,
							const double & dblValue, long lKeyIndex /* = -1 */) const
{
	if ( (key <= KEY_Unknown) || (key >= KEY_NumOfKeys) )
		return;

	os << m_vstrKeys.at(key).c_str();
	if ( (key == KEY_Note) || (key == KEY_Keyboard) )
		os << " " << lKeyIndex;
	if ( fabs(dblValue) < 1e-8 ) // To avoid crude "near-zero" values due to numerical inaccuracies
		os << " = " << (double)0 << std::endl;
	else
		os << " = " << dblValue << std::endl;
}



void CSingleScale::WriteKey(std::ostream & os, eKey key,
							const long & lValue, long lKeyIndex /* = -1 */) const
{
	if ( (key <= KEY_Unknown) || (key >= KEY_NumOfKeys) )
		return;

	os << m_vstrKeys.at(key).c_str();
	if ( (key == KEY_Note) || (key == KEY_Keyboard) )
		os << " " << lKeyIndex;
	os << " = " << lValue << std::endl;
}



long CSingleScale::Read(const char * szFilepath)
{
	// Open the file
	std::ifstream	ifstr(szFilepath, std::ios_base::in | std::ios_base::binary);

	if ( !ifstr )
		return m_err.SetError("Error opening the file.");

	// String which will receive the current line from the file
	CStringParser	strparser;
	strparser.InitStreamReading();

	// Read the file
	long	lResult = Read(ifstr, strparser);

	// Close the file
	ifstr.close();

	return lResult;
}



long CSingleScale::Read(std::istream & istr, CStringParser & strparser)
{
	bool		bInScaleData = false; // Flag to determine whether we are within a scale dataset
	eSection	secCurr = SEC_Unknown; // Current section
	eSection	secPriorityTuning = SEC_Unknown; // Tuning section with highest priority found so far


	// Initialize data
	// Important, because notes not listed in the tuning file
	// should always have standard tuning.
	Reset();


	// We need some temporary variables with initialization here
	// Keys of section [Tuning]
	long		lT_TunesCents[MaxNumOfNotes]; // Refers to DefaultBaseFreqHz
	// Keys of section [Exact Tuning]
	double		dblET_BaseFreqHz = DefaultBaseFreqHz;
	double		dblET_TunesCents[MaxNumOfNotes]; // Refers to m_dblBaseFreqHz
	long		lET_LastNoteFound = -1; // For auto completion
	for ( int i = 0 ; i < MaxNumOfNotes ; ++i )
		dblET_TunesCents[i] = lT_TunesCents[i] = 100 * i;


	// Read scale dataset from stream
	while ( true )
	{
		// Get next line
		if ( !strparser.GetLineAndTrim(istr, m_lReadLineCount) )
		{
			// No scale dataset found
			if ( !bInScaleData )
			{
				m_err.SetError("No scale dataset found", m_lReadLineCount);
				return 0;
			}
			// Format version >= 200 requires [Scale End] at dataset end
			if ( m_lFormatVersion >= 200 )
			{
				m_err.SetError("[Scale End] missing at file end or read error.", m_lReadLineCount);
				return -1;
			}
			// Format version < 200 ends with eof
			break;
		}

		// Skip empty lines and comments
		if ( strparser.str().empty() || (strparser.str().at(0) == ';') )
			continue;

		// Check for new section
		if ( strx::EvalSection(strparser.str()) )
		{
			secCurr = FindSection(strparser.str());

			if ( secCurr != SEC_ScaleBegin )
			{
				if ( !bInScaleData )
				{
					// No [Scale Begin] found -> assume Version 1 file format
					m_strFormat = Format();
					m_lFormatVersion = 100;
					m_strFormatSpecs = FormatSpecs();
					// One might give some keys special default values here...
				}
				// Check for version compliance of section
				bool	bSectionOK = false;
				if ( m_lFormatVersion >= 200 )
				{
					bSectionOK |= (secCurr == SEC_ScaleBegin);
					bSectionOK |= (secCurr == SEC_ScaleEnd);
					bSectionOK |= (secCurr == SEC_Info);
					bSectionOK |= (secCurr == SEC_EditorSpecifics);
					bSectionOK |= (secCurr == SEC_FunctionalTuning);
					bSectionOK |= (secCurr == SEC_Mapping);
					bSectionOK |= (secCurr == SEC_Assignment);
				}
				if ( m_lFormatVersion >= 100 )
				{
					bSectionOK |= (secCurr == SEC_Tuning);
					bSectionOK |= (secCurr == SEC_ExactTuning);
				}
				bSectionOK |= (secCurr == SEC_Unknown); // Each version must be aware of unknown sections!
				if ( !bSectionOK )
				{
					m_err.SetError("Section not version compliant.", m_lReadLineCount);
					return -1;
				}
			} // if ( secCurr != SEC_ScaleBegin )
			bInScaleData = true;

			// Detection of [Scale End] stops file reading immediately:
			if ( secCurr == SEC_ScaleEnd )
				break;

			// ignore tuning section, if section of higher priority was
			// already processed. Otherwise initialize tuning data
			if ( (secCurr == SEC_Tuning) ||
				 (secCurr == SEC_ExactTuning) ||
				 (secCurr == SEC_FunctionalTuning) )
			{
				if ( secPriorityTuning > secCurr )
					secCurr = SEC_Unknown; // Ignore section content
				else
					// This section has a higher priority than the previously
					// found ones -> remember it and process it
					secPriorityTuning = secCurr;
			}
			continue; // Process the next line
		} // if ( strx::EvalSection(strparser.str()) )

		// Skip lines not in a known section
		if ( secCurr == SEC_Unknown )
			continue;

		// Here you might process you editor specific data
		if ( secCurr == SEC_EditorSpecifics )
			continue; // Currently just ignore editor specific data

		// Split line into key and value
		std::string	strKey, strValue;
		if ( !strx::EvalKeyAndValue(strparser.str(), strKey, strValue) )
		{
			m_err.SetError("Syntax error", m_lReadLineCount);
			return -1;
		}

		// Now process the key:
		long	lKeyIndex;
		eKey	key = FindKey(strKey, lKeyIndex);

		switch ( secCurr )
		{
		case SEC_ScaleBegin:
			switch ( key )
			{
			case KEY_Format:
				if ( !CheckType(strValue, m_strFormat) )
					return -1;
				if ( m_strFormat != Format() )
				{
					m_err.SetError("Format not supported.", m_lReadLineCount);
					return -1;
				}
				break;
			case KEY_FormatVersion:
				if ( !CheckType(strValue, m_lFormatVersion) )
					return -1;
				break;
			case KEY_FormatSpecs:
				if ( !CheckType(strValue, m_strFormatSpecs) )
					return -1;
				break;
			}
			break;

		case SEC_Info:
			switch ( key )
			{
			case KEY_Name:
				if ( !CheckType(strValue, m_strName) )
					return -1;
				break;
			case KEY_ID:
				if ( !CheckType(strValue, m_strID) )
					return -1;
				break;
			case KEY_Filename:
				if ( !CheckType(strValue, m_strFilename) )
					return -1;
				break;
			case KEY_Author:
				if ( !CheckType(strValue, m_strAuthor) )
					return -1;
				break;
			case KEY_Location:
				if ( !CheckType(strValue, m_strLocation) )
					return -1;
				break;
			case KEY_Contact:
				if ( !CheckType(strValue, m_strContact) )
					return -1;
				break;
			case KEY_Date:
				if ( !CheckType(strValue, m_strDate) )
					return -1;
				if ( !IsDateFormatOK(m_strDate) )
				{
					m_err.SetError("Date format mismatch. YYYY-MM-DD expected!", m_lReadLineCount);
					return -1;
				}
				break;
			case KEY_Editor:
				if ( !CheckType(strValue, m_strEditor) )
					return -1;
				break;
			case KEY_EditorSpecs:
				if ( !CheckType(strValue, m_strEditorSpecs) )
					return -1;
				break;
			case KEY_Description:
				if ( !CheckType(strValue, m_strDescription) )
					return -1;
				break;
			case KEY_Keyword:
				{
					std::string	strNewKeyword;
					if ( !CheckType(strValue, strNewKeyword) )
						return -1;
					if ( !strNewKeyword.empty() )
						m_lstrKeywords.push_back(strNewKeyword);
				}
				break;
			case KEY_History:
				if ( !CheckType(strValue, m_strHistory) )
					return -1;
				break;
			case KEY_Geography:
				if ( !CheckType(strValue, m_strGeography) )
					return -1;
				break;
			case KEY_Instrument:
				if ( !CheckType(strValue, m_strInstrument) )
					return -1;
				break;
			case KEY_Composition:
				{
					std::string	strNewComposition;
					if ( !CheckType(strValue, strNewComposition) )
						return -1;
					if ( !strNewComposition.empty() )
					{
						// Check format:
						// Musician or Band|Album|Title|Year|Misc
						long	lNumOfBars = 0;
						for ( long l = 0 ; l < strNewComposition.size() ; ++l )
							if ( strNewComposition.at(l) == '|' )
								++lNumOfBars;
						if ( lNumOfBars != 4 )
						{
							m_err.SetError("Composition format mismatch. \"Musician or Band|Album|Title|Year|Misc\" expected!", m_lReadLineCount);
							return -1;
						}
						m_lstrCompositions.push_back(strNewComposition);
					}
				}
				break;
			case KEY_Comments:
				if ( !CheckType(strValue, m_strComments) )
					return -1;
				break;
			}
			break;

		case SEC_Tuning:
			if ( key == KEY_Note )
				if ( !CheckType(strValue, lT_TunesCents[lKeyIndex]) )
					return -1;
			break;

		case SEC_ExactTuning:
			switch ( key )
			{
			case KEY_BaseFreq:
				if ( !CheckType(strValue, dblET_BaseFreqHz) )
					return -1;
				break;
			case KEY_Note:
				if ( !CheckType(strValue, dblET_TunesCents[lKeyIndex]) )
					return -1;

				// Originally used __max, a windows only function macro
				// 	#define __max(a,b) (((a) > (b)) ? (a) : (b))
				lET_LastNoteFound = std::max(lET_LastNoteFound, lKeyIndex);
				break;
			}
			break;

		case SEC_FunctionalTuning:
			switch ( key )
			{
			case KEY_InitEqual:
				{
					std::string	strParams = strValue;
					if ( !strx::EvalFunctionParam(strParams) )
					{
						m_err.SetError("Value type mismatch. Function parameter block expected!", m_lReadLineCount);
						return -1;
					}

					const char	* szBeginPtr = strParams.c_str();
					const char	* szEndPtr;
					m_lInitEqual_BaseNote = strtol(szBeginPtr, const_cast<char **>(&szEndPtr), 10);
					while ( isspace(*szEndPtr) )
						++szEndPtr;
					if ( *szEndPtr != ',' )
					{
						m_err.SetError("Coma after parameter 1 missing!", m_lReadLineCount);
						return -1;
					}
					szBeginPtr = szEndPtr +1;
					m_dblInitEqual_BaseFreqHz = strtod(szBeginPtr, const_cast<char **>(&szEndPtr));
					while ( isspace(*szEndPtr) )
						++szEndPtr;
					if ( *szEndPtr != '\0' )
					{
						m_err.SetError("No more data expected after parameter 2!", m_lReadLineCount);
						return -1;
					}
					InitEqual(m_lInitEqual_BaseNote, m_dblInitEqual_BaseFreqHz);
				}
				break;
			case KEY_Note:
				{
					std::string	strFormula;
					if ( !CheckType(strValue, strFormula) )
						return -1;
					CFormula	formula(lKeyIndex);
					if ( !formula.SetFromStr(strFormula) )
					{
						m_err.SetError("Formula syntax error or parameter refers to invalid note index!", m_lReadLineCount);
						return -1;
					}
					AddFormula(formula);
				}
				break;
			}
			break;

		case SEC_Mapping:
			if ( key == KEY_LoopSize )
				if ( !CheckType(strValue, m_lMappingLoopSize) )
					return -1;
			if ( key == KEY_Keyboard )
				if ( !CheckType(strValue, m_vlMapping.at(lKeyIndex)) )
					return -1;
			break;

		case SEC_Assignment:
			if ( key == KEY_MIDIChannel )
			{
				std::string	strMIDIChannels;
				if ( !CheckType(strValue, strMIDIChannels) )
					return -1;

				if ( !SetMIDIChannelsAssignment(strMIDIChannels) )
					return -1;
			}
			break;

		} // switch ( secCurr )
	} // while ( true )

	// Apply tuning data of priority section found / check for existence of tuning data
	switch ( secPriorityTuning )
	{
	case SEC_Unknown:
		m_err.SetError("No tuning data found!", m_lReadLineCount);
		return -1;

	case SEC_Tuning:
		// Ignore keyboard mapping
		ResetKeyboardMapping();
		// Transfer Values from [Tuning] to m_vdblNoteFrequenciesHz
		InitEqual(0, DefaultBaseFreqHz);
		for ( int i = 0 ; i < MaxNumOfNotes ; ++i )
			m_vdblNoteFrequenciesHz.at(i) = Cents2Hz(lT_TunesCents[i], DefaultBaseFreqHz);
		// Create formulas to represent values
		for ( int i = MaxNumOfNotes-1 ; i >= 0 ; --i )
		{
			CFormula	formula(i);
			formula.SetToCentsAbsRef(lT_TunesCents[i], 0);
			m_lformulas.push_back(formula);
		}
		break;

	case SEC_ExactTuning:
		// Ignore keyboard mapping
		ResetKeyboardMapping();
		// Do the "auto expand"
		if ( (lET_LastNoteFound >= 0) && (lET_LastNoteFound < MaxNumOfNotes-1) )
		{
			long	H = lET_LastNoteFound;	// Highest MIDI note number
			double	P = dblET_TunesCents[H];		// Period length
			for ( int i = H ; i < MaxNumOfNotes ; ++i )
				dblET_TunesCents[i] = dblET_TunesCents[i-H] + P;
		}
		// Transfer Values from [Exact Tuning] to m_vdblNoteFrequenciesHz
		InitEqual(0, dblET_BaseFreqHz);
		for ( int i = 0 ; i < MaxNumOfNotes ; ++i )
			m_vdblNoteFrequenciesHz.at(i) = Cents2Hz(dblET_TunesCents[i], dblET_BaseFreqHz);
		// Create formulas to represent values
		// (in reverse order to avoid special handling if note 0 != 0 cents)
		for ( int i = MaxNumOfNotes-1 ; i >= 0 ; --i )
		{
			CFormula	formula(i);
			formula.SetToCentsAbsRef(dblET_TunesCents[i], 0);
			m_lformulas.push_back(formula);
		}
		break;

	case SEC_FunctionalTuning:
		// Nothing to do here, as values are already in-place
		break;

	default:
		// Uups... should never happen!
		assert(false);
	}

	return 1; // Everything nice!
}



bool CSingleScale::CheckType(const std::string & strValue, std::string & strResult)
{
	strResult = strValue;
	if ( !strx::EvalString(strResult) )
		return m_err.SetError("Value type mismatch. String expected!", m_lReadLineCount);
	else
		return m_err.SetOK();
}



bool CSingleScale::CheckType(const std::string & strValue, double & dblResult)
{
	std::string::size_type	pos = 0;
	if ( strx::Eval(strValue, pos, dblResult) && (pos == strValue.size()) )
		return m_err.SetOK();
	else
		return m_err.SetError("Value type mismatch. Float expected!", m_lReadLineCount);
}



bool CSingleScale::CheckType(const std::string & strValue, long & lResult)
{
	std::string::size_type	pos = 0;
	if ( strx::Eval(strValue, pos, lResult) && (pos == strValue.size()) )
		return m_err.SetOK();
	else
		return m_err.SetError("Value type mismatch. Integer expected!", m_lReadLineCount);
}





//////////////////////////////////////////////////////////////////////
// Keys of section [Info]
//////////////////////////////////////////////////////////////////////





bool CSingleScale::IsDateFormatOK(const std::string & strDate)
{
	// Check date format YYYY-MM-DD (only a formal check!)
	return !( (strDate.size() != 10) ||
			  (!isdigit(strDate.at(0))) ||
			  (!isdigit(strDate.at(1))) ||
			  (!isdigit(strDate.at(2))) ||
			  (!isdigit(strDate.at(3))) ||
			  (strDate.at(4) != '-') ||
			  (!isdigit(strDate.at(5))) ||
			  (!isdigit(strDate.at(6))) ||
			  (strDate.at(7) != '-') ||
			  (!isdigit(strDate.at(8))) ||
			  (!isdigit(strDate.at(9))) );
}



bool CSingleScale::SetDate(std::string strDate)
{
	if ( IsDateFormatOK(strDate) )
	{
		m_strDate = strDate;
		return m_err.SetOK();
	}
	else
		return m_err.SetError("Date format mismatch. YYYY-MM-DD expected!");
}



bool CSingleScale::SetDate(long lYear, long lMonth, long lDay)
{
	// Do a rough check of Year, Month and Day
	if ( (lYear < 0) || (lYear > 9999) ||
		 (lMonth < 0) || (lMonth > 12) ||
		 (lDay < 0) || (lDay > 31) )
		return false;

	char	szDate[11] = "YYYY-MM-DD";
	sprintf(szDate, "%04li-%02li-%02li", lYear, lMonth, lDay);
	m_strDate = szDate;
	return true;
}





//////////////////////////////////////////////////////////////////////
// Keys of section [Assignment]
//////////////////////////////////////////////////////////////////////





std::string CSingleScale::GetMIDIChannelsAssignment() const
{
	std::string	strMIDIChannels;
	std::list<CMIDIChannelRange>::const_iterator	it;
	for ( it = m_lmcrChannels.begin() ; it != m_lmcrChannels.end() ; ++it )
	{
		if ( !strMIDIChannels.empty() )
			strMIDIChannels += ",";
		strMIDIChannels += it->GetAsStr();
	}
	return strMIDIChannels;
}



bool CSingleScale::SetMIDIChannelsAssignment(std::string strMIDIChannels)
{
	std::list<std::string>	lstrChannels;
	strx::Split(strMIDIChannels, ',', lstrChannels, true, true);

	std::list<std::string>::const_iterator	it;
	m_lmcrChannels.clear();
	for ( it = lstrChannels.begin() ; it != lstrChannels.end() ; ++it )
	{
		CMIDIChannelRange	mcr;
		if ( !mcr.SetFromStr(*it) )
		{
			m_err.SetError("Error in MIDI channel range: syntax error or values exceed the range 1-65535!", m_lReadLineCount);
			return false;
		}
		m_lmcrChannels.push_back(mcr);
	}

	return true;
}



bool CSingleScale::AppliesToChannel(long lMIDIChannel) const
{
	// If no MIDI Channel is specified, the scale applies to each channel
	if ( m_lmcrChannels.empty() )
		return true;

	std::list<CMIDIChannelRange>::const_iterator	it;
	for ( it = m_lmcrChannels.begin() ; it != m_lmcrChannels.end() ; ++it )
		if ( it->IsInside(lMIDIChannel) )
			return true;

	return false;
}





//////////////////////////////////////////////////////////////////////
// Private Functions: Known sections and keys
//////////////////////////////////////////////////////////////////////





CSingleScale::eSection CSingleScale::FindSection(const std::string & strSection)
{
	if ( !strSection.empty() )
		for ( long l = 0 ; l < m_vstrSections.size() ; ++l )
			if ( strSection == strx::GetAsLower(m_vstrSections.at(l)) )
				return static_cast<CSingleScale::eSection>(l);
	return SEC_Unknown;
}



CSingleScale::eKey CSingleScale::FindKey(const std::string & strKey, long & lKeyIndex)
{
	if ( !strKey.empty() )
	{
		for ( long l = 0 ; l < m_vstrKeys.size() ; ++l )
		{
			std::string	strCurr = strx::GetAsLower(m_vstrKeys.at(l));
			if ( strKey == strCurr )
			{
				// Identity
				if ( (l == KEY_Note) || (l == KEY_Keyboard) )
					return KEY_Unknown; // Those keys need an index following
				return static_cast<CSingleScale::eKey>(l);
			}

			if ( strKey.substr(0, strCurr.size()) == strCurr )
			{
				// Begin matches
				if ( (l == KEY_Note) || (l == KEY_Keyboard) )
				{
					// Evaluate note index
					lKeyIndex = atol(strKey.substr(strCurr.size()).c_str());
					if ( IsNoteIndexOK(lKeyIndex) )
						return static_cast<CSingleScale::eKey>(l);
				}
			}
		}
	}

	return KEY_Unknown;
}





//////////////////////////////////////////////////////////////////////
// Private static Functions: Misc functions
//////////////////////////////////////////////////////////////////////





bool CSingleScale::IsNoteIndexOK(int nIndex)
{
	return (nIndex >= 0) && (nIndex < MaxNumOfNotes);
}





} // namespace TUN

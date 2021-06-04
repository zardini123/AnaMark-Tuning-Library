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
#include <algorithm>

#include "TUN_Scale.h"





namespace TUN
{





//////////////////////////////////////////////////////////////////////
// Constants / Tool functions
//////////////////////////////////////////////////////////////////////





const long		MaxNumOfNotes = 128;
const double	DefaultBaseFreqHz = 8.1757989156437073336;



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

double DefaultMIDINoteToHz(int midiNote)
{
	return Cents2Hz(DefaultMIDINoteToCents(midiNote), DefaultBaseFreqHz);
}

double DefaultMIDINoteToCents(int midiNote)
{
	// Clamp midiNote to 0 to 127 range
	if ( midiNote < 0 ) {
		midiNote = 0;
	}
	if ( midiNote > 127 ) {
		midiNote = 127;
	}
	return midiNote * 100;
}





//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////





std::vector<std::string>	CSingleScale::m_vstrSections;
std::vector<std::string>	CSingleScale::m_vstrKeys;



CSingleScale	ssTemporary___; // To ensure that the static vectors are initialized!



CSingleScale::~CSingleScale() = default;





//////////////////////////////////////////////////////////////////////
// Initialize scale
//////////////////////////////////////////////////////////////////////





void CSingleScale::Reset()
{
	err.SetOK();

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





//////////////////////////////////////////////////////////////////////
// Keys of section [Assignment]
//////////////////////////////////////////////////////////////////////




// For MSF
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
			err.SetError("Error in MIDI channel range: syntax error or values exceed the range 1-65535!", m_lReadLineCount);
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
// Private static Functions: Misc functions
//////////////////////////////////////////////////////////////////////





bool CSingleScale::IsNoteIndexOK(int nIndex)
{
	return (nIndex >= 0) && (nIndex < MaxNumOfNotes);
}





} // namespace TUN

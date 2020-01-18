// SCL_Import.h: Schnittstelle für die Klasse CSCL_Import.
//
// (C)opyright in 2009 by Mark Henning, Germany
//
// Contact: See contact form at www.mark-henning.de
//
// This file provides import capabitilies for Scala .SCL and .KBM files
//
// For details, see SCL_Import.cpp
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCL_Import_H__343D6998_CC30_4408_A585_1266F47A0DB6__INCLUDED_)
#define AFX_SCL_Import_H__343D6998_CC30_4408_A585_1266F47A0DB6__INCLUDED_



#include <fstream>
#include <string>

#include "TUN_MultiScaleFile.h"





namespace TUN
{





class CSCL_Import  
{
public:
	CSCL_Import();
	virtual ~CSCL_Import();



	void	ResetTuning();
	void	ResetMapping();



	// Error handling
	const CErr &	Err() const { return m_err; }
private:
	CErr	m_err;
public:



	// Handling of Scala Keyboard Mapping files
	bool			ReadKBM(const char * szFilepath);
private:
	bool			ReadKBM(std::istream & istr, CStringParser & strparser);
public:



	// Handling of Scala tuning files
	bool			ReadSCL(const char * szFilepath);
private:
	bool			ReadSCL(std::istream & istr, CStringParser & strparser);
public:



	// Export to CSingleScale
	void			SetSingleScale(CSingleScale & SS);
private:
	void			ApplyMapping();
//	double			GetMIDINoteFreqHz(int nMIDINote) const;
public:


	// Handling tuning data
	std::string		GetTuningName() const { return m_strTuningName; }
	std::string		GetMappingName() const { return m_strMappingName; }

private:
	// ----------------------------------------------------------------------
	// Variables: Tuning
	// ----------------------------------------------------------------------

	std::string			m_strTuningName;
	long				m_lScaleSize;
	double				m_dblScaleCents[128];
	// After applying the keyboard mapping:
	double				m_dblMappedCents[128];	// [Cents]



	// ----------------------------------------------------------------------
	// Variables: Mapping
	// ----------------------------------------------------------------------

	std::string			m_strMappingName;

	// Size of Keyboard Map (The pattern repeats every so many keys)
	long				m_lKeybMapPatternSize;
	// First MIDI note number to retune
	long				m_lKeybMapFirstMIDINote;
	// Last MIDI note number to retune
	long				m_lKeybMapLastMIDINote;
	// Middle MIDI note where scale degree 0 is mapped to:
	long				m_lKeybMapMiddleMIDINote;
	// Reference MIDI note for which frequency is given
	long				m_lKeybMapReferenceMIDINote;
	// Frequency to tune the above note to (floating point e.g. 440.0)
	double				m_dblKeybMapReferenceFreq_Hz;
	// Scale degree to consider as formal octave (determines differences
	// in pitch	between adjacent mapping patterns)
	long				m_lKeybMapOctaveHalftones;
	// One octave mapping table.
	// Array Indices: In-octave MIDI note index
	//         Value: Corresponding in-octave scale note index
	//                -1 represents MIDI note not retuned
	long				m_lKeybMap[128];



	// ----------------------------------------------------------------------
	// Variables: Misc
	// ----------------------------------------------------------------------

	long			m_lReadLineCount;
};





} // namespace TUN





#endif // !defined(AFX_SCL_Import_H__343D6998_CC30_4408_A585_1266F47A0DB6__INCLUDED_)

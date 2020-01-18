// SCL_Import.cpp: Implementierung der Klasse CSCL_Import.
//
// (C)opyright in 2009 by Mark Henning, Germany
//
// Contact: See contact page at www.mark-henning.de
//
// You may use this code for free. If you find an error or make some
// interesting changes, please let me know.
//
//////////////////////////////////////////////////////////////////////





#include "SCL_Import.h"





namespace TUN
{





#define RestrictMinMax(value, min, max) 	\
	(										\
		(value) < (min)						\
		?									\
		(min)								\
		:									\
		((value) > (max) ? (max) : (value))	\
	)





//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////





CSCL_Import::CSCL_Import()
{
	ResetMapping();
	ResetTuning();
}



CSCL_Import::~CSCL_Import()
{

}



void CSCL_Import::ResetTuning()
{
	m_strTuningName = "Equal tempered (Default)";
	m_lScaleSize = 128;
	for ( int i = 0 ; i < 128 ; ++i )
		m_dblMappedCents[i] = 100 * i;
}



void CSCL_Import::ResetMapping()
{
	m_strMappingName = "MIDI (Default)";

	// Size of Keyboard Map (The pattern repeats every so many keys)
	m_lKeybMapPatternSize = 12;

	// First MIDI note number to retune
	m_lKeybMapFirstMIDINote = 0;

	// Last MIDI note number to retune
	m_lKeybMapLastMIDINote = 127;

	// Middle note where scale degree 0 is mapped to:
	m_lKeybMapMiddleMIDINote = 0;

	// Reference note for which frequency is given
	m_lKeybMapReferenceMIDINote = 69; // A

	// Frequency to tune the above note to (floating point e.g. 440.0)
	m_dblKeybMapReferenceFreq_Hz = 440;

	// Scale degree to consider as formal octave (determines differences
	// in pitch between adjacent mapping patterns)
	m_lKeybMapOctaveHalftones = 12;

	for ( int i = 0 ; i < 128 ; ++i )
		m_lKeybMap[i] = i;
}





//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////





bool CSCL_Import::ReadKBM(const char * szFilepath)
{
	// Open the file
	std::ifstream	ifstr(szFilepath, std::ios_base::in | std::ios_base::binary);

	if ( !ifstr )
		return m_err.SetError("Error opening the file.");

	// String which will receive the current line from the file
	CStringParser	strparser;
	strparser.InitStreamReading();

	// Initialize data
	// Important, because notes not listed in the tuning file
	// should always have standard tuning.
	ResetMapping();

	// Set Mapping name to file name per default if no description available
	m_strMappingName = szFilepath;
	m_strMappingName = m_strMappingName.substr(('\\' + m_strMappingName).find_last_of("/\\"));

	// Read the file
	bool	bResult = ReadKBM(ifstr, strparser);

	// Close the file
	ifstr.close();

	return bResult;
}



bool CSCL_Import::ReadKBM(std::istream & istr, CStringParser & strparser)
{
	// IMPORTANT: ResetMapping is expected to be called before calling this function!

	// Read keyboard map from stream
	long	lSettingCounter = -1;

	do
	{
		// Get next line
		if ( !strparser.GetLineAndTrim(istr, m_lReadLineCount) )
			return m_err.SetError("Premature end of file.", m_lReadLineCount);
		// Skip empty lines and comments
		if ( strparser.str().empty() || (strparser.str().at(0) == '!') )
			continue;

		long	lValue = atol(strparser.str().c_str());
		double	dblValue = atof(strparser.str().c_str());
		bool	bValueOK = true;

		switch ( ++lSettingCounter )
		{
		case 0: // Size of Keyboard Map (The pattern repeats every so many keys)
			m_lKeybMapPatternSize = RestrictMinMax(lValue, 1, 127);
			break;
		case 1: // First MIDI note number to retune
			m_lKeybMapFirstMIDINote = RestrictMinMax(lValue, 0, 127);
			break;
		case 2: // Last MIDI note number to retune
			m_lKeybMapLastMIDINote = RestrictMinMax(lValue, 0, 127);
			bValueOK = (m_lKeybMapFirstMIDINote < m_lKeybMapLastMIDINote);
			break;
		case 3: // Middle MIDI note where scale degree 0 is mapped to:
			m_lKeybMapMiddleMIDINote = RestrictMinMax(lValue, 0, 127);
			bValueOK = (m_lKeybMapMiddleMIDINote + m_lKeybMapPatternSize <= 127);
			break;
		case 4: // Reference MIDI note for which frequency is given
			m_lKeybMapReferenceMIDINote = RestrictMinMax(lValue, 0, 127);
			break;
		case 5: // Frequency to tune the above note to (floating point e.g. 440.0)
			m_dblKeybMapReferenceFreq_Hz = RestrictMinMax(dblValue, 0.001, 100000);
			break;
		case 6: // Scale degree to consider as formal octave (determines differences
				// in pitch between adjacent mapping patterns)
			m_lKeybMapOctaveHalftones = RestrictMinMax(lValue, 0, 127);
			break;
		}

		if ( !bValueOK )
			return m_err.SetError("Setting out of range.", m_lReadLineCount);
	} while ( lSettingCounter < 6 );

	// Read the mapping entries
	for ( long lEntryNumber = 0 ; lEntryNumber < m_lKeybMapPatternSize ; ++lEntryNumber )
	{
		// Default: MIDI note is not retuned
		m_lKeybMap[lEntryNumber] = -1;

		// Get next line
		if ( !strparser.GetLineAndTrim(istr, m_lReadLineCount) )
			continue; // Premature end of file means 'x' for any missing entry
		// Skip empty lines and comments
		if ( strparser.str().empty() || (strparser.str().at(0) == '!') )
		{
			--lEntryNumber;
			continue;
		}

		// Apply data
		if ( TUN::strx::GetAsLower(strparser.str()).at(0) != 'x' )
		{
			long	lScaleNoteIndex = atol(strparser.str().c_str());
			m_lKeybMap[lEntryNumber] = RestrictMinMax(lScaleNoteIndex, 0, 127);
		}
	}

	// File must end here
	while ( true )
	{
		// Get next line
		if ( !strparser.GetLineAndTrim(istr, m_lReadLineCount) )
			break; // End of file reached
		// Skip empty lines and comments
		if ( strparser.str().empty() || (strparser.str().at(0) == '!') )
			continue;
		return m_err.SetError("End of file expected, but additional data found.", m_lReadLineCount);
	}

	return true; // Everything nice!
}



bool CSCL_Import::ReadSCL(const char * szFilepath)
{
	// Open the file
	std::ifstream	ifstr(szFilepath, std::ios_base::in | std::ios_base::binary);

	if ( !ifstr )
		return m_err.SetError("Error opening the file.");

	// String which will receive the current line from the file
	CStringParser	strparser;
	strparser.InitStreamReading();

	// Initialize data
	// Important, because notes not listed in the tuning file
	// should always have standard tuning.
	ResetTuning();

	// Set Tuning name to file name per default if no description available
	m_strTuningName = szFilepath;
	m_strTuningName = m_strTuningName.substr(('\\' + m_strTuningName).find_last_of("/\\"));

	// Read the file
	bool	bResult = ReadSCL(ifstr, strparser);

	// Close the file
	ifstr.close();

	return bResult;
}



bool CSCL_Import::ReadSCL(std::istream & istr, CStringParser & strparser)
{
	// IMPORTANT: ResetTuning is expected to be called before calling this function!

	// Read scale dataset from stream
	bool	bNameRead = false;
	long	lCurrNote = 0;
	m_lScaleSize = -1;
	while ( true )
	{
		// Get next line
		if ( !strparser.GetLineAndTrim(istr, m_lReadLineCount) )
			break; // End of file reached
		// Skip empty lines and comments
		if ( strparser.str().empty() || (strparser.str().at(0) == '!') )
			continue;

		if ( !bNameRead )
		{
			// Read Name of scala tune
			bNameRead = true;
			// if line contains numbers only, there is no "Name line" or name is empty
			if ( strparser.str().find_first_not_of("0123456789") != std::string::npos )
			{
				// Name found
				m_strTuningName = strparser.str();
				continue; // Read next line
			}
		}

		if ( m_lScaleSize < 0 )
		{
			// Line contains number of tunes
			m_lScaleSize = atol(strparser.str().c_str());
			// Check number of notes. Must be from 1 to 127. Other tunings are rejected
			if ( (m_lScaleSize < 1) || (m_lScaleSize > 127) )
				return m_err.SetError("Scale size not allowed. Must be within [1;127].", m_lReadLineCount);

			// First note has always 1/1 tuning
			m_dblScaleCents[0] = 0;

			continue; // Read next line
		}

		if ( ++lCurrNote > m_lScaleSize )
			return m_err.SetError("End of file expected, but further data found.", m_lReadLineCount);

		// Check for ratio or cent value (cent values contain a '.')
		std::string::size_type	posMaybePeriod = 
									strparser.str().find_first_not_of("+-0123456789");
		if ( (posMaybePeriod == std::string::npos) || (strparser.str().at(posMaybePeriod) != '.') )
		{
			// No period --> ratio
			const char	* szCurr = strparser.str().c_str();
			double	dblNumber1 = strtod(szCurr, const_cast<char **>(&szCurr));
			while ( isspace(*szCurr) )
				++szCurr;
			if ( *szCurr == '/' )
			{
				double	dblNumber2 = strtod(++szCurr, NULL);
				if ( dblNumber2 == 0 )
					return m_err.SetError("Division by zero.", m_lReadLineCount);
				m_dblScaleCents[lCurrNote] = TUN::Factor2Cents(dblNumber1 / dblNumber2);
			}
			else
				return m_err.SetError("Unknown operator. '/' expected.", m_lReadLineCount);
		}
		else
		{
			// Period found --> cent value
			m_dblScaleCents[lCurrNote] = atof(strparser.str().c_str());
		}
	} // while (true)

	if ( (!bNameRead) || (m_lScaleSize < 0) )
		return m_err.SetError("No data in file.", m_lReadLineCount);
	if ( lCurrNote < m_lScaleSize )
		return m_err.SetError("Less tuning entries found than expected.", m_lReadLineCount);

	return true; // Everything nice!
}



void CSCL_Import::SetSingleScale(CSingleScale & SS)
{
	ApplyMapping();

	SS.Reset();

	// Ensure reference note frequency
	SS.InitEqual(m_lKeybMapReferenceMIDINote, m_dblKeybMapReferenceFreq_Hz);

	for ( long lMIDINote = 0 ; lMIDINote < 128 ; ++lMIDINote )
	{
		if ( lMIDINote != m_lKeybMapReferenceMIDINote )
		{
			double		dblEffectiveCents = m_dblMappedCents[lMIDINote] - m_dblMappedCents[m_lKeybMapReferenceMIDINote];
			CFormula	formula(lMIDINote);
			formula.SetToCentsAbsRef(dblEffectiveCents, m_lKeybMapReferenceMIDINote);
			SS.AddFormula(formula);
		}
	}
}



void CSCL_Import::ApplyMapping()
{
	for ( long lMIDINote = 0 ; lMIDINote < 128 ; ++lMIDINote )
	{
		// Set default tuning
		m_dblMappedCents[lMIDINote] = 100 * lMIDINote;

		// Check for MIDI note in mapping range
		if ( (lMIDINote >= m_lKeybMapFirstMIDINote) &&
			 (lMIDINote <= m_lKeybMapLastMIDINote) )
		{
			// Obtain keyboard map note index
			long	lNoteOffset = lMIDINote-m_lKeybMapMiddleMIDINote;
			long	lOctaveNumber = lNoteOffset / m_lKeybMapPatternSize;
			long	lNoteInOctave = lNoteOffset % m_lKeybMapPatternSize;
			if ( lNoteInOctave < 0 )
			{
				lOctaveNumber -= 1;
				lNoteInOctave += m_lKeybMapPatternSize;
			}
			if ( (m_lKeybMap[lNoteInOctave] < 0) || (m_lKeybMap[lNoteInOctave] >= m_lScaleSize) )
				continue; // MIDI note is not retuned

			// Retune note
			m_dblMappedCents[lMIDINote] = m_dblScaleCents[m_lKeybMap[lNoteInOctave]] + (m_lKeybMapMiddleMIDINote + lOctaveNumber * m_lKeybMapOctaveHalftones) * 100;
		}
	}
}



/*double CSCL_Import::GetMIDINoteFreqHz(int nMIDINote) const
{
	nMIDINote = RestrictMinMax(nMIDINote, 0, 127);
	double	dblEffectiveCents = m_dblMappedCents[nMIDINote] - m_dblMappedCents[m_lKeybMapReferenceMIDINote];
	return Cents2Hz(dblEffectiveCents, m_dblKeybMapReferenceFreq_Hz);
}*/





} // namespace TUN

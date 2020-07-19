// TUN_Scale.h: Interface of the class CSingleScale.
//
// (C)opyright in 2003-2009 by Mark Henning, Germany
//
// Implementation of AnaMark Tuning File Format V2.00 (*.TUN)
//
// Read TUN_Scale.cpp for more informations about this class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLESCALE_H__15693DC4_FB37_11D6_A827_F4C607C10000__INCLUDED_)
#define AFX_SINGLESCALE_H__15693DC4_FB37_11D6_A827_F4C607C10000__INCLUDED_





#pragma warning( disable : 4786 )

#include <fstream>
#include <list>
#include <vector>

#include "TUN_Error.h"
#include "TUN_StringTools.h"
#include "TUN_Formula.h"
#include "TUN_MIDIChannelRange.h"





namespace TUN
{





// Common constants:
extern const long	MaxNumOfNotes;
extern const double	DefaultBaseFreqHz; // 8.1757989156437073336 Hz -> refers to A=440Hz



// Common tool functions and constants:
double Hz2Cents(double dblHz, double dblBaseFreqHz); // Convert Hz to cents
double Cents2Hz(double dblCents, double dblBaseFreqHz); // Convert cents to Hz
double Cents2Factor(double dblCents); // Obtain factor from cents to multiply with a reference frequency in Hz
double Factor2Cents(double dblFactor); // Obtain Cents from a factor



// MIDI tool functions
double MIDINote_DefaultHz(int nMIDINote); // Important: Uses DefaultBaseFreqHz instead of the rounded value 8.1758 Hz given in the MIDI specs!
double MIDINote_DefaultCents(int nMIDINote);



class CSingleScale
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Identification of known sections and keys
public:
	enum eSection {
		SEC_Unknown,
		// Begin/End sections
		SEC_ScaleBegin,
		SEC_ScaleEnd,
		// Data sections
		SEC_Info,
		SEC_EditorSpecifics,
		// Tuning sections: Ordering also defines priority
		SEC_Tuning,
		SEC_ExactTuning,
		SEC_FunctionalTuning,
		// Mapping
		SEC_Mapping,
		// Sections specifically in Multi Scale Files
		SEC_Assignment,
		// For referring the complete data set
		SEC_DataSet,
		// Number of sections
		SEC_NumOfSections
	};

	enum eKey {
		KEY_Unknown,
		// Keys of section [Scale Begin]
		KEY_Format,
		KEY_FormatVersion,
		KEY_FormatSpecs,
		// Keys of section [Info]
		KEY_Name,
		KEY_ID,
		KEY_Filename,
		KEY_Author,
		KEY_Location,
		KEY_Contact,
		KEY_Date,
		KEY_Editor,
		KEY_EditorSpecs,
		KEY_Description,
		KEY_Keyword,
		KEY_History,
		KEY_Geography,
		KEY_Instrument,
		KEY_Composition,
		KEY_Comments,
		// Keys of sections [Tuning], [Exact Tuning] and [Functional Tuning]
		KEY_Note,
		KEY_BaseFreq,
		KEY_InitEqual,
		// Keys of sections [Mapping]
		KEY_LoopSize,
		KEY_Keyboard,
		// Keys of sections [Assignment]
		KEY_MIDIChannel,
		// No keys in section [Scale End]
		// For referring the complete data set
		KEY_AllData,
		// Number of keys
		KEY_NumOfKeys
	};

	static const std::vector<std::string> &	GetSections() { return m_vstrSections; }
	static const std::vector<std::string> &	GetKeys() { return m_vstrKeys; }
	static eSection							FindSection(const std::string & strSection);
	static eKey								FindKey(const std::string & strKey,
													long & lKeyIndex);

private:
	static std::vector<std::string>	m_vstrSections;
	static std::vector<std::string>	m_vstrKeys;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Main stuff
public:
	CSingleScale();
	virtual ~CSingleScale();



	// Error handling
	const CErr &	Err() const { return m_err; }
private:
	CErr	m_err;
public:



	// Initialize all keys (keyboard mapping) and scale to A=440Hz
	void	Reset();
private:
	void	ResetKeyboardMapping();
public:
	// Initialize scale (default values are A=440Hz)
	void	InitEqual(long lBaseNote = 69, double dblBaseFreqHz = 440);



	// Accessing the scale
	// Get base note / base frequency
	long	GetBaseNote() const { return m_lInitEqual_BaseNote; }
	double	GetBaseFreqHz() const { return m_dblInitEqual_BaseFreqHz; }

	/**
	 * Read-access of the note frequencies
	 *
	 * Be aware that frequencies <= 0 Hz could be returned, especially
	 * when the .tun file loaded makes use of the [Functional Tuning] section.
	 * It is strongly suggest to handle notes of such frequencies as "muted" notes.
	 * (i.e. do not output any sound on these notes.)
	 *
	 * (NOTE: Vector index is scale note number, NOT MIDI note number!)
	 * @return Frequencies of scale notes.
	 */
	const std::vector<double> &	GetNoteFrequenciesHz() const { return m_vdblNoteFrequenciesHz; }

	/**
	 * Be aware that frequencies <= 0 Hz could be returned, especially
 	 * when the .tun file loaded makes use of the [Functional Tuning] section.
 	 * It is strongly suggest to handle notes of such frequencies as "muted" notes.
 	 * (i.e. do not output any sound on these notes.)
	 * @param  lMIDINoteNumber MIDI note number (0 to 127)
	 * @return                 Frequency of that note in scale
	 */
	double						GetMIDINoteFreqHz(long lMIDINoteNumber) const { return m_vdblNoteFrequenciesHz.at(MapMIDI2Scale(lMIDINoteNumber)); }
	// Write-access of the note frequencies
	// When changing values you must make use of the CFormula class
	// The object stores *all* applied formulas in a list so that
	// it contains the complete history of changes, which is also
	// written to the file.
	void	AddFormula(CFormula formula);
	// Read/write-access of the mapping
	std::vector<long> &			GetMapping() { return m_vlMapping; }
	const std::vector<long> &	GetMapping() const { return m_vlMapping; }
	long						GetMappingLoopSize() const { return m_lMappingLoopSize; }
	void						SetMappingLoopSize(long lMappingLoopSize);
	long						MapMIDI2Scale(long lMIDINoteNumber) const; // Returns scale note number
	// Read/write-access of Assigment data for Multi Scale Files
	std::list<CMIDIChannelRange> &			GetChannels() { return m_lmcrChannels; }
	const std::list<CMIDIChannelRange> &	GetChannels() const { return m_lmcrChannels; }



	// Read/write files
	// Write tuning file
	//
	// lVersionFrom and lVersionTo define which sections to write.
	// Valid version values for From and To are 0, 100, 200
	bool	Write(const char * szFilepath,
				  long lVersionFrom = 0, long lVersionTo = 200, bool bWriteHeaderComment = true);
	bool	Write(std::ostream & os,
				  long lVersionFrom = 0, long lVersionTo = 200, bool bWriteHeaderComment = true);
private:
	void	WriteSection(std::ostream & os, eSection section) const;
	void	WriteKey(std::ostream & os, eKey key, const std::list<std::string> & lstrValues) const;
	void	WriteKey(std::ostream & os, eKey key, const std::string & strValue, long lKeyIndex = -1) const;
	void	WriteKey(std::ostream & os, eKey key, const double & dblValue, long lKeyIndex = -1) const;
	void	WriteKey(std::ostream & os, eKey key, const long & lValue, long lKeyIndex = -1) const;
public:

	// Read-functions return:
	// -1 = an error occurred
	// 0 = No scale dataset found
	// 1 = everything O.K.
	long	Read(const char * szFilepath);
	long	Read(std::istream & istr, CStringParser & strparser);
private:
	long	m_lReadLineCount;
	bool	CheckType(const std::string & strValue, std::string & strResult);
	bool	CheckType(const std::string & strValue, double & dblResult);
	bool	CheckType(const std::string & strValue, long & lResult);
public:



	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Variables with free public access
public:
	// Keys of section [Scale Begin]
	// Currently none

	// Keys of section [Info]
	std::string				m_strName;
	std::string				m_strID;
	std::string				m_strFilename;
	std::string				m_strAuthor;
	std::string				m_strLocation;
	std::string				m_strContact;
	std::string				m_strEditor;
	std::string				m_strEditorSpecs;
	std::string				m_strDescription;
	std::list<std::string>	m_lstrKeywords;
	std::string				m_strHistory;
	std::string				m_strGeography;
	std::string				m_strInstrument;
	std::list<std::string>	m_lstrCompositions; // Format: "Musician or Band|Album|Title|Year|Misc"
	std::string				m_strComments;



	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Accessing non-tuning variables without direct access
public:
	// Keys of section [Scale Begin]
	const char *			Format() const { return "AnaMark-TUN"; } // Never Change!
	long					FormatVersion() const { return 200; } // Never Change!
	const char *			FormatVersionAsStr() const { return "200"; } // Never Change!
	static const char *		FormatSpecs() { return "http:\\\\www.mark-henning.de\\eternity\\tuningspecs.html"; } // Never Change!

	// Keys of section [Info]
	static bool				IsDateFormatOK(const std::string & strDate);
	std::string				GetDate() const { return m_strDate; }
	bool					SetDate(std::string strDate);
	bool					SetDate(long lYear, long lMonth, long lDay);

	// Keys of section [Assignment]
	// Returns true, if scale applies to MIDI Channel given
	std::string				GetMIDIChannelsAssignment() const;
	bool					SetMIDIChannelsAssignment(std::string strMIDIChannels);
	bool					AppliesToChannel(long lMIDIChannel) const;



	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Variables without direct access
private:

	// Keys of section [Scale Begin]
	std::string	m_strFormat;
	long		m_lFormatVersion;
	std::string	m_strFormatSpecs;

	// Keys of sections [Assignment]
	// Note: if this list is empty, it means, that the scale is to be applied
	// to each MIDI channel (= there is no restriction)
	std::list<CMIDIChannelRange>	m_lmcrChannels;

	// Keys of section [Info]
	std::string	m_strDate;


private:
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Misc functions
	static bool		IsNoteIndexOK(int nIndex);



	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// Last but not least: the scale and its definitions
	long				m_lInitEqual_BaseNote;
	double				m_dblInitEqual_BaseFreqHz;
	std::vector<double>	m_vdblNoteFrequenciesHz; // index = Scale note number, see m_vlMapping
	std::list<CFormula>	m_lformulas;
	// Keyboard mapping:
	std::vector<long>	m_vlMapping; // index = MIDI note number, value = Scale note number
	long				m_lMappingLoopSize;
}; // class CSingleScale





} // namespace TUN





#endif // !defined(AFX_SINGLESCALE_H__15693DC4_FB37_11D6_A827_F4C607C10000__INCLUDED_)

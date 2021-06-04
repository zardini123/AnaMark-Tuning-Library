// TUN_Scale.h: Interface of the class CSingleScale.
//
// (C)opyright in 2003-2009 by Mark Henning, Germany
//
// Implementation of AnaMark Tuning File Format V2.00 (*.TUN)
//
// Read TUN_Scale.cpp for more informations about this class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ANAMARK_TUNING_LIBRARY_SRC_TUN_SCALE_H_
#define ANAMARK_TUNING_LIBRARY_SRC_TUN_SCALE_H_

#include <fstream>
#include <list>
#include <vector>

#include "TUN_Error.h"
#include "TUN_Formula.h"
#include "TUN_MIDIChannelRange.h"
#include "TUN_StringTools.h"
#include "ProvidesErrorMessage.h"

namespace TUN {

//////////////////////////////////////////////////////////////////////
// Common tool functions and constants
//////////////////////////////////////////////////////////////////////

/** @brief Used for indexing midi notes 0 - 127 */
extern const long MaxNumOfNotes;
/** @brief 8.1757989156437073336 Hz -> refers to A=440Hz */
extern const double DefaultBaseFreqHz;

double Hz2Cents(double dblHz, double dblBaseFreqHz);    // Convert Hz to cents
double Cents2Hz(double dblCents, double dblBaseFreqHz); // Convert cents to Hz
double Cents2Factor(double dblCents);  // Obtain factor from cents to multiply with a
                                       // reference frequency in Hz
double Factor2Cents(double dblFactor); // Obtain Cents from a factor

//////////////////////////////////////////////////////////////////////
// MIDI tool functions
//////////////////////////////////////////////////////////////////////

/**
 * @brief Convert midi note to hertz (Hz) using the default 12-tone equal tempermant mapping
 * provided by the MIDI specification.
 *
 * @attention Uses @p DefaultBaseFreqHz instead of the rounded value 8.1758 Hz given in the
 * MIDI specs!  This therefore has a higher resolution and accuracy.
 * @param  midiNote	 MIDI note from 0 to 127.  If outside the range, @p midiNote will be clamped.
 * @return           The midi note in hertz (Hz).
 */
double DefaultMIDINoteToHz(int midiNote);

/**
 * @brief Convert midi note to cents using the default 12-tone equal tempermant mapping
 * provided by the MIDI specification.
 * @param  midiNote  MIDI note from 0 to 127.  If outside the range, @p midiNote will be clamped.
 * @return           The midi note in cents.
 */
double DefaultMIDINoteToCents(int midiNote);

class CSingleScale : public ProvidesErrorMessage {
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Identification of known sections and keys
public:

  static const std::vector<std::string> &GetSections() {
    return m_vstrSections;
  }
  static const std::vector<std::string> &GetKeys() {
    return m_vstrKeys;
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Main stuff
public:
  CSingleScale();
  virtual ~CSingleScale();

public:
  // Initialize all keys (keyboard mapping) and scale to A=440Hz
  void Reset();

private:
  void ResetKeyboardMapping();

public:
  // Initialize scale (default values are A=440Hz)
  void InitEqual(long lBaseNote = 69, double dblBaseFreqHz = 440);

  // Accessing the scale
  // Get base note / base frequency
  long GetBaseNote() const {
    return m_lInitEqual_BaseNote;
  }
  double GetBaseFreqHz() const {
    return m_dblInitEqual_BaseFreqHz;
  }

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
  const std::vector<double> &GetNoteFrequenciesHz() const {
    return m_vdblNoteFrequenciesHz;
  }

  /**
   * Be aware that frequencies <= 0 Hz could be returned, especially
   * when the .tun file loaded makes use of the [Functional Tuning] section.
   * It is strongly suggest to handle notes of such frequencies as "muted" notes.
   * (i.e. do not output any sound on these notes.)
   * @param  lMIDINoteNumber MIDI note number (0 to 127)
   * @return                 Frequency of that note in scale
   */
  double GetMIDINoteFreqHz(long lMIDINoteNumber) const {
    return m_vdblNoteFrequenciesHz.at(MapMIDI2Scale(lMIDINoteNumber));
  }
  // Write-access of the note frequencies
  // When changing values you must make use of the CFormula class
  // The object stores *all* applied formulas in a list so that
  // it contains the complete history of changes, which is also
  // written to the file.
  void AddFormula(CFormula formula);
  // Read/write-access of the mapping
  std::vector<long> &GetMapping() {
    return m_vlMapping;
  }
  const std::vector<long> &GetMapping() const {
    return m_vlMapping;
  }
  long GetMappingLoopSize() const {
    return m_lMappingLoopSize;
  }
  void SetMappingLoopSize(long lMappingLoopSize);
  long MapMIDI2Scale(long lMIDINoteNumber) const; // Returns scale note number
  // Read/write-access of Assigment data for Multi Scale Files
  std::list<CMIDIChannelRange> &GetChannels() {
    return m_lmcrChannels;
  }
  const std::list<CMIDIChannelRange> &GetChannels() const {
    return m_lmcrChannels;
  }

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Accessing non-tuning variables without direct access
public:

  // Keys of section [Assignment]
  // Returns true, if scale applies to MIDI Channel given
  std::string GetMIDIChannelsAssignment() const;
  bool SetMIDIChannelsAssignment(std::string strMIDIChannels);
  bool AppliesToChannel(long lMIDIChannel) const;

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Variables without direct access
private:
  // Keys of section [Scale Begin]
  std::string m_strFormat;
  long m_lFormatVersion;
  std::string m_strFormatSpecs;

  // Keys of sections [Assignment]
  // Note: if this list is empty, it means, that the scale is to be applied
  // to each MIDI channel (= there is no restriction)
  std::list<CMIDIChannelRange> m_lmcrChannels;

  // Keys of section [Info]
  std::string m_strDate;

private:
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Misc functions
  static bool IsNoteIndexOK(int nIndex);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  // Last but not least: the scale and its definitions
  long m_lInitEqual_BaseNote;
  double m_dblInitEqual_BaseFreqHz;
  std::vector<double>
      m_vdblNoteFrequenciesHz; // index = Scale note number, see m_vlMapping
  std::list<CFormula> m_lformulas;
  // Keyboard mapping:
  std::vector<long> m_vlMapping; // index = MIDI note number, value = Scale note number
  long m_lMappingLoopSize;
}; // class CSingleScale

} // namespace TUN

#endif // ANAMARK_TUNING_LIBRARY_SRC_TUN_SCALE_H_

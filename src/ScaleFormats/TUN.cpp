#include "TUN.h"
#include <cassert>

namespace AnaMark {
namespace ScaleFormats {

// @TODO: Copied from original TUN_Scale.h.  Needs to be fully looked over and
// modernized using the new API
#if false
bool TUN::ReadDirectlyToScale(::TUN::TUN &scaleToOverwrite, std::istream &inputStream) {
  bool bInScaleData = false; // Flag to determine whether we are within a scale dataset
  eSection secCurr = SEC_Unknown; // Current section
  eSection secPriorityTuning =
      SEC_Unknown; // Tuning section with highest priority found so far

  // Initialize data
  // Important, because notes not listed in the tuning file
  // should always have standard tuning.
  Reset();

  // We need some temporary variables with initialization here
  // Keys of section [Tuning]
  long lT_TunesCents[MaxNumOfNotes]; // Refers to DefaultBaseFreqHz
  // Keys of section [Exact Tuning]
  double dblET_BaseFreqHz = DefaultBaseFreqHz;
  double dblET_TunesCents[MaxNumOfNotes]; // Refers to m_dblBaseFreqHz
  long lET_LastNoteFound = -1;            // For auto completion
  for (int i = 0; i < MaxNumOfNotes; ++i)
    dblET_TunesCents[i] = lT_TunesCents[i] = 100 * i;

  // Read scale dataset from stream
  while (true) {
    // Get next line
    if (!strparser.GetLineAndTrim(istr, m_lReadLineCount)) {
      // No scale dataset found
      if (!bInScaleData) {
        err.SetError("No scale dataset found", m_lReadLineCount);
        return 0;
      }
      // Format version >= 200 requires [Scale End] at dataset end
      if (m_lFormatVersion >= 200) {
        err.SetError("[Scale End] missing at file end or read error.", m_lReadLineCount);
        return -1;
      }
      // Format version < 200 ends with eof
      break;
    }

    // Skip empty lines and comments
    if (strparser.str().empty() || (strparser.str().at(0) == ';'))
      continue;

    // Check for new section
    if (strx::EvalSection(strparser.str())) {
      secCurr = FindSection(strparser.str());

      if (secCurr != SEC_ScaleBegin) {
        if (!bInScaleData) {
          // No [Scale Begin] found -> assume Version 1 file format
          m_strFormat = Format();
          m_lFormatVersion = 100;
          m_strFormatSpecs = FormatSpecs();
          // One might give some keys special default values here...
        }
        // Check for version compliance of section
        bool bSectionOK = false;
        if (m_lFormatVersion >= 200) {
          bSectionOK |= (secCurr == SEC_ScaleBegin);
          bSectionOK |= (secCurr == SEC_ScaleEnd);
          bSectionOK |= (secCurr == SEC_Info);
          bSectionOK |= (secCurr == SEC_EditorSpecifics);
          bSectionOK |= (secCurr == SEC_FunctionalTuning);
          bSectionOK |= (secCurr == SEC_Mapping);
          bSectionOK |= (secCurr == SEC_Assignment);
        }
        if (m_lFormatVersion >= 100) {
          bSectionOK |= (secCurr == SEC_Tuning);
          bSectionOK |= (secCurr == SEC_ExactTuning);
        }
        bSectionOK |=
            (secCurr == SEC_Unknown); // Each version must be aware of unknown sections!
        if (!bSectionOK) {
          err.SetError("Section not version compliant.", m_lReadLineCount);
          return -1;
        }
      } // if ( secCurr != SEC_ScaleBegin )
      bInScaleData = true;

      // Detection of [Scale End] stops file reading immediately:
      if (secCurr == SEC_ScaleEnd)
        break;

      // ignore tuning section, if section of higher priority was
      // already processed. Otherwise initialize tuning data
      if ((secCurr == SEC_Tuning) || (secCurr == SEC_ExactTuning) ||
          (secCurr == SEC_FunctionalTuning)) {
        if (secPriorityTuning > secCurr)
          secCurr = SEC_Unknown; // Ignore section content
        else
          // This section has a higher priority than the previously
          // found ones -> remember it and process it
          secPriorityTuning = secCurr;
      }
      continue; // Process the next line
    }           // if ( strx::EvalSection(strparser.str()) )

    // Skip lines not in a known section
    if (secCurr == SEC_Unknown)
      continue;

    // Here you might process you editor specific data
    if (secCurr == SEC_EditorSpecifics)
      continue; // Currently just ignore editor specific data

    // Split line into key and value
    std::string strKey, strValue;
    if (!strx::EvalKeyAndValue(strparser.str(), strKey, strValue)) {
      err.SetError("Syntax error", m_lReadLineCount);
      return -1;
    }

    // Now process the key:
    long lKeyIndex;
    eKey key = FindKey(strKey, lKeyIndex);

    switch (secCurr) {
    case SEC_ScaleBegin:
      switch (key) {
      case KEY_Format:
        if (!CheckType(strValue, m_strFormat))
          return -1;
        if (m_strFormat != Format()) {
          err.SetError("Format not supported.", m_lReadLineCount);
          return -1;
        }
        break;
      case KEY_FormatVersion:
        if (!CheckType(strValue, m_lFormatVersion))
          return -1;
        break;
      case KEY_FormatSpecs:
        if (!CheckType(strValue, m_strFormatSpecs))
          return -1;
        break;
      default: break;
      }
      break;

    case SEC_Info:
      switch (key) {
      case KEY_Name:
        if (!CheckType(strValue, m_strName))
          return -1;
        break;
      case KEY_ID:
        if (!CheckType(strValue, m_strID))
          return -1;
        break;
      case KEY_Filename:
        if (!CheckType(strValue, m_strFilename))
          return -1;
        break;
      case KEY_Author:
        if (!CheckType(strValue, m_strAuthor))
          return -1;
        break;
      case KEY_Location:
        if (!CheckType(strValue, m_strLocation))
          return -1;
        break;
      case KEY_Contact:
        if (!CheckType(strValue, m_strContact))
          return -1;
        break;
      case KEY_Date:
        if (!CheckType(strValue, m_strDate))
          return -1;
        if (!IsDateFormatOK(m_strDate)) {
          err.SetError("Date format mismatch. YYYY-MM-DD expected!", m_lReadLineCount);
          return -1;
        }
        break;
      case KEY_Editor:
        if (!CheckType(strValue, m_strEditor))
          return -1;
        break;
      case KEY_EditorSpecs:
        if (!CheckType(strValue, m_strEditorSpecs))
          return -1;
        break;
      case KEY_Description:
        if (!CheckType(strValue, m_strDescription))
          return -1;
        break;
      case KEY_Keyword: {
        std::string strNewKeyword;
        if (!CheckType(strValue, strNewKeyword))
          return -1;
        if (!strNewKeyword.empty())
          m_lstrKeywords.push_back(strNewKeyword);
      } break;
      case KEY_History:
        if (!CheckType(strValue, m_strHistory))
          return -1;
        break;
      case KEY_Geography:
        if (!CheckType(strValue, m_strGeography))
          return -1;
        break;
      case KEY_Instrument:
        if (!CheckType(strValue, m_strInstrument))
          return -1;
        break;
      case KEY_Composition: {
        std::string strNewComposition;
        if (!CheckType(strValue, strNewComposition))
          return -1;
        if (!strNewComposition.empty()) {
          // Check format:
          // Musician or Band|Album|Title|Year|Misc
          long lNumOfBars = 0;
          for (std::size_t l = 0; l < strNewComposition.size(); ++l)
            if (strNewComposition.at(l) == '|')
              ++lNumOfBars;
          if (lNumOfBars != 4) {
            err.SetError("Composition format mismatch. \"Musician or "
                         "Band|Album|Title|Year|Misc\" expected!",
                         m_lReadLineCount);
            return -1;
          }
          m_lstrCompositions.push_back(strNewComposition);
        }
      } break;
      case KEY_Comments:
        if (!CheckType(strValue, m_strComments))
          return -1;
        break;
      default: break;
      }
      break;

    case SEC_Tuning:
      if (key == KEY_Note)
        if (!CheckType(strValue, lT_TunesCents[lKeyIndex]))
          return -1;
      break;

    case SEC_ExactTuning:
      switch (key) {
      case KEY_BaseFreq:
        if (!CheckType(strValue, dblET_BaseFreqHz))
          return -1;
        break;
      case KEY_Note:
        if (!CheckType(strValue, dblET_TunesCents[lKeyIndex]))
          return -1;

        // Originally used __max, a windows only function macro
        // 	#define __max(a,b) (((a) > (b)) ? (a) : (b))
        lET_LastNoteFound = std::max(lET_LastNoteFound, lKeyIndex);
        break;
      default: break;
      }
      break;

    case SEC_FunctionalTuning:
      switch (key) {
      case KEY_InitEqual: {
        std::string strParams = strValue;
        if (!strx::EvalFunctionParam(strParams)) {
          err.SetError("Value type mismatch. Function parameter block expected!",
                       m_lReadLineCount);
          return -1;
        }

        const char *szBeginPtr = strParams.c_str();
        const char *szEndPtr;
        m_lInitEqual_BaseNote = strtol(szBeginPtr, const_cast<char **>(&szEndPtr), 10);
        while (isspace(*szEndPtr))
          ++szEndPtr;
        if (*szEndPtr != ',') {
          err.SetError("Coma after parameter 1 missing!", m_lReadLineCount);
          return -1;
        }
        szBeginPtr = szEndPtr + 1;
        m_dblInitEqual_BaseFreqHz = strtod(szBeginPtr, const_cast<char **>(&szEndPtr));
        while (isspace(*szEndPtr))
          ++szEndPtr;
        if (*szEndPtr != '\0') {
          err.SetError("No more data expected after parameter 2!", m_lReadLineCount);
          return -1;
        }
        InitEqual(m_lInitEqual_BaseNote, m_dblInitEqual_BaseFreqHz);
      } break;
      case KEY_Note: {
        std::string strFormula;
        if (!CheckType(strValue, strFormula))
          return -1;
        CFormula formula(lKeyIndex);
        if (!formula.SetFromStr(strFormula)) {
          err.SetError("Formula syntax error or parameter refers to invalid note index!",
                       m_lReadLineCount);
          return -1;
        }
        AddFormula(formula);
      } break;
      default: break;
      }
      break;

    case SEC_Mapping:
      if (key == KEY_LoopSize)
        if (!CheckType(strValue, m_lMappingLoopSize))
          return -1;
      if (key == KEY_Keyboard)
        if (!CheckType(strValue, m_vlMapping.at(lKeyIndex)))
          return -1;
      break;

    case SEC_Assignment:
      if (key == KEY_MIDIChannel) {
        std::string strMIDIChannels;
        if (!CheckType(strValue, strMIDIChannels))
          return -1;

        if (!SetMIDIChannelsAssignment(strMIDIChannels))
          return -1;
      }
      break;
    default: break;
    } // switch ( secCurr )
  }   // while ( true )

  // Apply tuning data of priority section found / check for existence of tuning data
  switch (secPriorityTuning) {
  case SEC_Unknown: err.SetError("No tuning data found!", m_lReadLineCount); return -1;

  case SEC_Tuning:
    // Ignore keyboard mapping
    ResetKeyboardMapping();
    // Transfer Values from [Tuning] to m_vdblNoteFrequenciesHz
    InitEqual(0, DefaultBaseFreqHz);
    for (int i = 0; i < MaxNumOfNotes; ++i)
      m_vdblNoteFrequenciesHz.at(i) = Cents2Hz(lT_TunesCents[i], DefaultBaseFreqHz);
    // Create formulas to represent values
    for (int i = MaxNumOfNotes - 1; i >= 0; --i) {
      CFormula formula(i);
      formula.SetToCentsAbsRef(lT_TunesCents[i], 0);
      m_lformulas.push_back(formula);
    }
    break;

  case SEC_ExactTuning:
    // Ignore keyboard mapping
    ResetKeyboardMapping();
    // Do the "auto expand"
    if ((lET_LastNoteFound >= 0) && (lET_LastNoteFound < MaxNumOfNotes - 1)) {
      long H = lET_LastNoteFound;     // Highest MIDI note number
      double P = dblET_TunesCents[H]; // Period length
      for (int i = H; i < MaxNumOfNotes; ++i)
        dblET_TunesCents[i] = dblET_TunesCents[i - H] + P;
    }
    // Transfer Values from [Exact Tuning] to m_vdblNoteFrequenciesHz
    InitEqual(0, dblET_BaseFreqHz);
    for (int i = 0; i < MaxNumOfNotes; ++i)
      m_vdblNoteFrequenciesHz.at(i) = Cents2Hz(dblET_TunesCents[i], dblET_BaseFreqHz);
    // Create formulas to represent values
    // (in reverse order to avoid special handling if note 0 != 0 cents)
    for (int i = MaxNumOfNotes - 1; i >= 0; --i) {
      CFormula formula(i);
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

bool TUN::WriteDirectlyFromScale(::TUN::TUN &scaleToReadFromImmediately,
                                 std::ostream &outputStream,
                                 FormatVersionRange formatVersionRangeToWrite) {
  // Evaluate which sections to write
  if (lVersionFrom < 0)
    lVersionFrom = 0;
  if (lVersionTo > 200)
    lVersionTo = 200;
  if (lVersionFrom > lVersionTo)
    return err.SetError(
        "Error in version settings - file not written."); // Versions to write mismatch

  bool bV000 = ((lVersionFrom <= 0) && (lVersionTo >= 0));
  bool bV100 = ((lVersionFrom <= 100) && (lVersionTo >= 100));
  bool bV200 = ((lVersionFrom <= 200) && (lVersionTo >= 200));

  int i;

  // Header comment
  if ((bV100 || bV200) && bWriteHeaderComment) {
    os << ";" << std::endl;
    os << "; This is an AnaMark tuning map file V2.00" << std::endl;
    if (!bV200)
      os << "; written in V1.00 compatibility mode" << std::endl;
    os << ";" << std::endl;
    os << "; Free .TUN file handling source code (C)2009 by Mark Henning, Germany"
       << std::endl;
    os << ";" << std::endl;
    os << "; Specifications and free source code see:" << std::endl;
    os << ";         " << FormatSpecs() << std::endl;
    os << ";" << std::endl;
    os << std::endl;
    os << std::endl;
  }

  // Section [Scale Begin]
  if (bV100 || bV200) {
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
  if (bV200 &&
      !m_lmcrChannels.empty()) // Versions below 200 do not support Multi Scale Files!
  {
    os << ";" << std::endl;
    os << "; Assignment of scale dataset" << std::endl;
    os << "; Note: This might be ignored, if this is not part of a MSF-File!"
       << std::endl;
    os << "; See the documentation of the software you use for details." << std::endl;
    os << ";" << std::endl;
    WriteSection(os, SEC_Assignment);
    WriteKey(os, KEY_MIDIChannel, GetMIDIChannelsAssignment());
    os << std::endl;
    os << std::endl;
  }

  // Section [Info]
  if (bV100 || bV200) {
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
  if (bV100 || bV200) {
    // Currently we don't have such specific data, so don't write the section
    if (false) {
      os << ";" << std::endl;
      os << "; Editor specific data" << std::endl;
      os << ";" << std::endl;
      WriteSection(os, SEC_EditorSpecifics);
      os << std::endl;
      os << std::endl;
    }
  }

  // Section [Functional Tuning]
  if (bV200) {
    os << ";" << std::endl;
    os << "; Version 2:" << std::endl;
    os << "; Functional tunings" << std::endl;
    os << ";" << std::endl;
    WriteSection(os, SEC_FunctionalTuning);
    os.precision(10);
    os << m_vstrKeys.at(KEY_InitEqual).c_str() << " = (" << m_lInitEqual_BaseNote << ", "
       << m_dblInitEqual_BaseFreqHz << ")" << std::endl;
    std::list<CFormula>::const_iterator it;
    for (it = m_lformulas.begin(); it != m_lformulas.end(); ++it)
      WriteKey(os, KEY_Note, it->GetAsStr(), it->GetMyIndex());
    os << std::endl;
    os << std::endl;
  }

  // Section [Exact Tuning]
  if (bV100) {
    double dblET_BaseFreqHz =
        m_dblInitEqual_BaseFreqHz * pow(2, -m_lInitEqual_BaseNote / 12.);

    os << ";" << std::endl;
    os << "; Version 1:" << std::endl;
    os << "; AnaMark-specific section with exact tunings" << std::endl;
    os << ";" << std::endl;
    WriteSection(os, SEC_ExactTuning);
    os.precision(10);
    WriteKey(os, KEY_BaseFreq, dblET_BaseFreqHz);
    for (i = 0; i < MaxNumOfNotes; ++i)
      WriteKey(os,
               KEY_Note,
               Hz2Cents(m_vdblNoteFrequenciesHz.at(MapMIDI2Scale(i)), dblET_BaseFreqHz),
               i);
    os << std::endl;
    os << std::endl;
  }

  // Section [Tuning]
  if (bV000) {
    os << ";" << std::endl;
    os << "; Version 0:" << std::endl;
    os << "; VAZ-section with quantized tunings" << std::endl;
    os << ";" << std::endl;
    WriteSection(os, SEC_Tuning);
    for (i = 0; i < MaxNumOfNotes; ++i)
      WriteKey(
          os,
          KEY_Note,
          long(static_cast<long>(floor(
              Hz2Cents(m_vdblNoteFrequenciesHz.at(MapMIDI2Scale(i)), DefaultBaseFreqHz) +
              0.5))),
          i);
    os << std::endl;
    os << std::endl;
  }

  // Section [Mapping]
  if (bV200) {
    // Write only if needed:
    bool bNeedsMapping = false;
    long lMapSize = ((m_lMappingLoopSize <= 0) || (m_lMappingLoopSize >= MaxNumOfNotes)
                         ? MaxNumOfNotes
                         : m_lMappingLoopSize);
    for (i = 0; i < lMapSize; ++i)
      bNeedsMapping |= (m_vlMapping.at(i) != i);

    if (bNeedsMapping) {
      os << ";" << std::endl;
      os << "; Keyboard mapping: Keyboard note number -> scale note number" << std::endl;
      os << ";" << std::endl;
      WriteSection(os, SEC_Mapping);
      WriteKey(os, KEY_LoopSize, m_lMappingLoopSize);
      for (i = 0; i < lMapSize; ++i) {
        if (m_vlMapping.at(i) != i)
          WriteKey(os, KEY_Keyboard, m_vlMapping.at(i), i);
      }
      os << std::endl;
      os << std::endl;
    }
  } else {
    if (bV100) {
      os << ";" << std::endl;
      os << "; In V1.00 compatibility mode, there is no explicit keyboard mapping"
         << std::endl;
      os << "; The order of frequencies above includes keyboard mapping settings."
         << std::endl;
      os << ";" << std::endl;
      os << std::endl;
      os << std::endl;
    }
  }

  // Section [Scale End]
  if (bV100 || bV200) {
    os << ";" << std::endl;
    os << "; End of tuning file" << std::endl;
    os << ";" << std::endl;
    WriteSection(os, SEC_ScaleEnd);
    os << std::endl;
    os << std::endl;
  }

  return err.SetOK();
}

////////////////////////////////////////////////////////
// TUN-specific utility functions
////////////////////////////////////////////////////////

void TUN::WriteSection(std::ostream &os, eSection section) const {
  if ((section <= SEC_Unknown) || (section >= SEC_NumOfSections))
    return;

  os << strx::GetAsSection(m_vstrSections.at(section)).c_str() << std::endl;
}

void TUN::WriteKey(std::ostream &os, eKey key,
                   const std::list<std::string> &lstrValues) const {
  if ((key <= KEY_Unknown) || (key >= KEY_NumOfKeys) || (lstrValues.empty()))
    return;

  std::list<std::string>::const_iterator it;
  for (it = lstrValues.begin(); it != lstrValues.end(); ++it) {
    if (it->empty())
      continue;
    os << m_vstrKeys.at(key).c_str() << " = " << strx::GetAsString(*it).c_str()
       << std::endl;
  }
}

void TUN::WriteKey(std::ostream &os, eKey key, const std::string &strValue,
                   long lKeyIndex /* = -1 */) const {
  if ((key <= KEY_Unknown) || (key >= KEY_NumOfKeys) || (strValue.empty()))
    return;

  os << m_vstrKeys.at(key).c_str();
  if ((key == KEY_Note) || (key == KEY_Keyboard))
    os << " " << lKeyIndex;
  os << " = " << strx::GetAsString(strValue).c_str() << std::endl;
}

void TUN::WriteKey(std::ostream &os, eKey key, const double &dblValue,
                   long lKeyIndex /* = -1 */) const {
  if ((key <= KEY_Unknown) || (key >= KEY_NumOfKeys))
    return;

  os << m_vstrKeys.at(key).c_str();
  if ((key == KEY_Note) || (key == KEY_Keyboard))
    os << " " << lKeyIndex;
  if (fabs(dblValue) <
      1e-8) // To avoid crude "near-zero" values due to numerical inaccuracies
    os << " = " << (double)0 << std::endl;
  else
    os << " = " << dblValue << std::endl;
}

void TUN::WriteKey(std::ostream &os, eKey key, const long &lValue,
                   long lKeyIndex /* = -1 */) const {
  if ((key <= KEY_Unknown) || (key >= KEY_NumOfKeys))
    return;

  os << m_vstrKeys.at(key).c_str();
  if ((key == KEY_Note) || (key == KEY_Keyboard))
    os << " " << lKeyIndex;
  os << " = " << lValue << std::endl;
}

bool TUN::CheckType(const std::string &strValue, std::string &strResult) {
  strResult = strValue;
  if (!strx::EvalString(strResult))
    return err.SetError("Value type mismatch. String expected!", m_lReadLineCount);
  else
    return err.SetOK();
}

bool TUN::CheckType(const std::string &strValue, double &dblResult) {
  std::string::size_type pos = 0;
  if (strx::Eval(strValue, pos, dblResult) && (pos == strValue.size()))
    return err.SetOK();
  else
    return err.SetError("Value type mismatch. Float expected!", m_lReadLineCount);
}

bool TUN::CheckType(const std::string &strValue, long &lResult) {
  std::string::size_type pos = 0;
  if (strx::Eval(strValue, pos, lResult) && (pos == strValue.size()))
    return err.SetOK();
  else
    return err.SetError("Value type mismatch. Integer expected!", m_lReadLineCount);
}

//////////////////////////////////////////////////////////////////////
// Keys of section [Info]
//////////////////////////////////////////////////////////////////////

bool TUN::IsDateFormatOK(const std::string &strDate) {
  // Check date format YYYY-MM-DD (only a formal check!)
  return !(
      (strDate.size() != 10) || (!isdigit(strDate.at(0))) || (!isdigit(strDate.at(1))) ||
      (!isdigit(strDate.at(2))) || (!isdigit(strDate.at(3))) || (strDate.at(4) != '-') ||
      (!isdigit(strDate.at(5))) || (!isdigit(strDate.at(6))) || (strDate.at(7) != '-') ||
      (!isdigit(strDate.at(8))) || (!isdigit(strDate.at(9))));
}

bool TUN::SetDate(std::string strDate) {
  if (IsDateFormatOK(strDate)) {
    m_strDate = strDate;
    return err.SetOK();
  } else
    return err.SetError("Date format mismatch. YYYY-MM-DD expected!");
}

bool TUN::SetDate(long lYear, long lMonth, long lDay) {
  // Do a rough check of Year, Month and Day
  if ((lYear < 0) || (lYear > 9999) || (lMonth < 0) || (lMonth > 12) || (lDay < 0) ||
      (lDay > 31))
    return false;

  char szDate[11] = "YYYY-MM-DD";
  sprintf(szDate, "%04li-%02li-%02li", lYear, lMonth, lDay);
  m_strDate = szDate;
  return true;
}

//////////////////////////////////////////////////////////////////////
// Private Functions: Known sections and keys
//////////////////////////////////////////////////////////////////////


TUN::eSection TUN::FindSection(const std::string & strSection)
{
	if ( !strSection.empty() )
		for ( std::size_t l = 0 ; l < m_vstrSections.size() ; ++l )
			if ( strSection == strx::GetAsLower(m_vstrSections.at(l)) )
				return static_cast<TUN::eSection>(l);
	return SEC_Unknown;
}



TUN::eKey TUN::FindKey(const std::string & strKey, long & lKeyIndex)
{
	if ( !strKey.empty() )
	{
		for ( std::size_t l = 0 ; l < m_vstrKeys.size() ; ++l )
		{
			std::string	strCurr = strx::GetAsLower(m_vstrKeys.at(l));
			if ( strKey == strCurr )
			{
				// Identity
				if ( (l == KEY_Note) || (l == KEY_Keyboard) )
					return KEY_Unknown; // Those keys need an index following
				return static_cast<TUN::eKey>(l);
			}

			if ( strKey.substr(0, strCurr.size()) == strCurr )
			{
				// Begin matches
				if ( (l == KEY_Note) || (l == KEY_Keyboard) )
				{
					// Evaluate note index
					lKeyIndex = atol(strKey.substr(strCurr.size()).c_str());
					if ( IsNoteIndexOK(lKeyIndex) )
						return static_cast<TUN::eKey>(l);
				}
			}
		}
	}

	return KEY_Unknown;
}
#endif

} // namespace ScaleFormats
} // namespace AnaMark

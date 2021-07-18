#ifndef ANAMARK_TUNING_LIBRARY_SRC_SCALE_FORMATS_TUN_H_
#define ANAMARK_TUNING_LIBRARY_SRC_SCALE_FORMATS_TUN_H_

#include "ScaleFormat.hpp"

#include <cassert>

namespace AnaMark {
namespace ScaleFormats {

class TUN : public ScaleFormat {
public:
  const char *FormatName() override {
    return "AnaMark-TUN";
  }

  const char *FormatSpecificationsURL() override {
    return "https://www.mark-henning.de/files/am/Tuning_File_V2_Doc.pdf";
  }

  const std::set<unsigned short> &AvaliableVersions() override {
    static std::set<unsigned short> versions = {
        Version(0, 00),
        Version(1, 00),
        Version(2, 00),
    };
    return versions;
  }

  const Flags<Requirements> FormatRequirements(
      VersionNumber versionNumber) override {
    ThrowIfVersionDoesNotExist(versionNumber);

    // All versions of TUN require scale information.  TUN version 2.00 introduced
    // keyboard mapping, though its optional.
    return Requirements::ScaleInformation;
  }

  const Flags<Capabilities> FormatCapabilities(
      VersionNumber versionNumber) override {
    ThrowIfVersionDoesNotExist(versionNumber);

    switch (versionNumber) {
    case Version(0, 00):
      return Flags<Capabilities>(Capabilities::ScaleInformation,
                                 Capabilities::Explicit0To127MIDINoteMapping);

    case Version(1, 00):
      return Flags<Capabilities>(Capabilities::ScaleInformation,
                                 Capabilities::KeyboardMapping,
                                 Capabilities::Explicit0To127MIDINoteMapping,
                                 Capabilities::PeriodicScale);

    case Version(2, 00):
      return Flags<Capabilities>(Capabilities::ScaleInformation,
                                 Capabilities::KeyboardMapping,
                                 Capabilities::ScaleName,
                                 Capabilities::Explicit0To127MIDINoteMapping,
                                 Capabilities::PeriodicScale,
                                 Capabilities::RequestMIDIChannelAssignment);

    default: throw std::logic_error("Format version is non-existent for TUN");
    }
  }

  void ReadDirectlyToScale(Scale &scaleToOverwrite,
                           std::istream &inputStream) override {
    // TUN read implementation needs to be redone using new API.

    // Set requested midi channel assignments.

    // End function by setting loaded format to the file read.

    assert(false);
  }

  void WriteDirectlyFromScale(
      Scale &scaleToReadFromImmediately, std::ostream &outputStream,
      FormatVersionRange formatVersionRangeToWrite) override {

    ThrowIfVersionRangeIsInvalid(formatVersionRangeToWrite);

    // TUN write implementation needs to be redone using new API.

    // Follow formatVersionRangeToWrite and write all parts of the TUN spec that
    // falls within that range.
    assert(false);
  }

// @TODO: Copied from original TUN_Scale.h.  Needs to be fully looked over and
// modernized using the new API
#if false
private:
  ////////////////////////////////////////////////////////
  // TUN-specific utility functions
  ////////////////////////////////////////////////////////
  void WriteSection(std::ostream &os, eSection section) const;

  void WriteKey(std::ostream &os, eKey key,
                const std::list<std::string> &lstrValues) const;
  void WriteKey(std::ostream &os, eKey key, const std::string &strValue,
                long lKeyIndex /* = -1 */) const;
  void WriteKey(std::ostream &os, eKey key, const double &dblValue,
                long lKeyIndex /* = -1 */) const;
  void WriteKey(std::ostream &os, eKey key, const long &lValue,
                long lKeyIndex /* = -1 */) const;

  bool CheckType(const std::string &strValue, std::string &strResult);
  bool CheckType(const std::string &strValue, double &dblResult);
  bool CheckType(const std::string &strValue, long &lResult);

  // Keys of section [Info]
  static bool IsDateFormatOK(const std::string &strDate);
  std::string GetDate() const {
    return m_strDate;
  }
  bool SetDate(std::string strDate);
  bool SetDate(long lYear, long lMonth, long lDay);

  static eSection FindSection(const std::string &strSection);
  static eKey FindKey(const std::string &strKey, long &lKeyIndex);

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
#endif
};

} // namespace ScaleFormats
} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_SCALE_FORMATS_TUN_H_

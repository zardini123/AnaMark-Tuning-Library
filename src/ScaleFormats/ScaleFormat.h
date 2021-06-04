#ifndef ANAMARK_TUNING_LIBRARY_SRC_SCALE_FORMATS_SCALE_FORMAT_H_
#define ANAMARK_TUNING_LIBRARY_SRC_SCALE_FORMATS_SCALE_FORMAT_H_

#include <exception>
#include <fstream>
#include <vector>

#include "../Format.h"
#include "../SingleScale.h"

namespace AnaMark {

class ScaleFormat : public Format {
public:
  // Bring Read and Write from Format into ScaleFormat namespace to have proper
  // function overloading with the virtual function.  This "un-hides" the base class
  // "overloads."
  using Format::Read;
  using Format::Write;

  // Requirements are properties of a SingleScale that is required for a format to
  // exist
  struct Requirements {
    bool scaleInformation : 1;
    bool keyboardMapping : 1;
  };

  // Capabilities are properties of a SingleScale that may or may not be required by
  // a format, but may result in informational loss if exporting to such format if
  // the SingleScale contains the information
  struct Capabilities {
    bool scaleInformation : 1;
    bool keyboardMapping : 1;
    bool scaleName : 1;
    bool explicit0To127MIDINoteMapping : 1;
    bool periodicScale : 1;
    bool requestMIDIChannelAssignment : 1;
  };

  class RequirementsNotPresentException : public Requirements {};

  ////////////////////////////////////////////////////////
  // Scale Format requirements and compatibility information
  ////////////////////////////////////////////////////////

  virtual const Requirements &FormatRequirements(VersionNumber versionNumber) = 0;

  const Requirements &FormatRequirements() {
    return FormatRequirements(NewestVersion());
  }

  virtual const Capabilities &FormatCapabilities(VersionNumber versionNumber) = 0;

  const Capabilities &FormatCapabilities() {
    return FormatCapabilities(NewestVersion());
  }

  ////////////////////////////////////////////////////////
  // Loaded Scale information
  ////////////////////////////////////////////////////////

  // If false, can assume all channels or any channel
  // If true, call RequestedMIDIChannelsAssignment to get the channels
  bool HasRequestedMIDIChannelAssignments() {
    // When empty, there are no assignment requests
    return !requestedMIDIChannels.empty();
  }

  const std::vector<unsigned char> &RequestedMIDIChannelAssignments() {
    return requestedMIDIChannels;
  }

  ////////////////////////////////////////////////////////
  // Format I/O
  ////////////////////////////////////////////////////////

  ////////////////////////////
  // READ

  virtual void ReadDirectlyToScale(SingleScale &scaleToOverwrite,
                                   std::istream &inputStream) = 0;

  void Read(std::istream &inputStream) override {
    ReadDirectlyToScale(this->transactionScale, inputStream);
  }

  ////////////////////////////
  // WRITE

  virtual void WriteDirectlyFromScale(
      SingleScale &scaleToReadFromImmediately, std::ostream &outputStream,
      FormatVersionRange formatVersionRangeToWrite) = 0;

  void WriteDirectlyFromScale(SingleScale &scaleToReadFromImmediately,
                              std::ostream &outputStream,
                              VersionNumber formatVersion) {
    FormatVersionRange range;
    range.startingVersionInclusive = 0;
    range.endingVersionInclusive = formatVersion;
    WriteDirectlyFromScale(scaleToReadFromImmediately, outputStream, range);
  }

  void Write(std::ostream &outputStream,
             VersionNumber formatVersionToWrite) override {
    WriteDirectlyFromScale(
        this->transactionScale, outputStream, formatVersionToWrite);
  }

protected:
  void SetRequestedMIDIChannelAssignments(
      const std::vector<unsigned char> &newMIDIChannelAssignments) {
    requestedMIDIChannels = newMIDIChannelAssignments;
  }

private:
  std::vector<unsigned char> requestedMIDIChannels;

  SingleScale transactionScale;
};

} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_SCALE_FORMATS_SCALE_FORMAT_H_

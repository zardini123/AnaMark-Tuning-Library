#ifndef ANAMARK_TUNING_LIBRARY_SRC_SCALE_FORMATS_SCALE_FORMAT_H_
#define ANAMARK_TUNING_LIBRARY_SRC_SCALE_FORMATS_SCALE_FORMAT_H_

#include <exception>
#include <fstream>
#include <type_traits>
#include <vector>

#include "../Format.hpp"
#include "../Scale.hpp"
#include "../Utilities.hpp"

namespace AnaMark {

class ScaleFormat : public Format {
public:
  // Bring Read and Write from Format into ScaleFormat namespace to have proper
  // function overloading with the virtual function.  This "un-hides" the base class
  // "overloads."
  // using Format::Read;
  // using Format::Write;

  // Requirements are properties of a Scale that is required for a format to
  // exist
  enum class Requirements {
    ScaleInformation = 1 << 0,
    KeyboardMapping = 1 << 1,
  };

  // Capabilities are properties of a Scale that may or may not be required by
  // a format, but may result in informational loss if exporting to such format if
  // the Scale contains the information
  enum class Capabilities {
    ScaleInformation = 1 << 0,
    KeyboardMapping = 1 << 1,
    ScaleName = 1 << 2,
    Explicit0To127MIDINoteMapping = 1 << 3,
    PeriodicScale = 1 << 4,
    RequestMIDIChannelAssignment = 1 << 5,
  };

  ////////////////////////////////////////////////////////
  // Scale Format requirements and compatibility information
  ////////////////////////////////////////////////////////

  virtual const Flags<Requirements> FormatRequirements(
      VersionNumber versionNumber) = 0;

  const Flags<Requirements> FormatRequirements() {
    return FormatRequirements(NewestVersion());
  }

  virtual const Flags<Capabilities> FormatCapabilities(
      VersionNumber versionNumber) = 0;

  const Flags<Capabilities> FormatCapabilities() {
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
  // Scale interface
  ////////////////////////////////////////////////////////

  void ExportTo(Scale &scaleToOverwrite) const {
    scaleToOverwrite = this->transactionScale;
  }

  void ImportFrom(Scale &scaleToReadFrom) {
    // @TODO: Need to reset requested MIDI channels.
    // @TODO: Run function to make scaleToReadFrom as optimal as possible (write
    // dirty frequencies).
    this->transactionScale = scaleToReadFrom;
  }

  ////////////////////////////////////////////////////////
  // Format I/O
  ////////////////////////////////////////////////////////

  ////////////////////////////
  // READ

  virtual void ReadDirectlyToScale(Scale &scaleToOverwrite,
                                   std::istream &inputStream) = 0;

  void Read(std::istream &inputStream) {
    ReadDirectlyToScale(this->transactionScale, inputStream);
  }

  void Read(const char *inputFilePath) {
    std::ifstream inputFile;
    // Set stream to throw exception on failbit or badbit being set
    inputFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    inputFile.open(inputFilePath);

    Read(inputFile);

    inputFile.close();
  }

  ////////////////////////////
  // WRITE

  virtual void WriteDirectlyFromScale(
      Scale &scaleToReadFromImmediately, std::ostream &outputStream,
      FormatVersionRange formatVersionRangeToWrite) = 0;

  void WriteDirectlyFromScale(Scale &scaleToReadFromImmediately,
                              std::ostream &outputStream,
                              VersionNumber formatVersion) {
    FormatVersionRange range;
    range.startingVersionInclusive = 0;
    range.endingVersionInclusive = formatVersion;
    WriteDirectlyFromScale(scaleToReadFromImmediately, outputStream, range);
  }

  void Write(std::ostream &outputStream, VersionNumber formatVersionToWrite) {
    WriteDirectlyFromScale(
        this->transactionScale, outputStream, formatVersionToWrite);
  }

  void Write(std::ostream &outputStream) {
    Write(outputStream, NewestVersion());
  }

  void Write(const char *outputFilePath) {
    std::ofstream outputFile;
    // Set stream to throw exception on failbit or badbit being set
    outputFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    outputFile.open(outputFilePath);

    Write(outputFile);

    outputFile.close();
  }

protected:
  void SetRequestedMIDIChannelAssignments(
      const std::vector<unsigned char> &newMIDIChannelAssignments) {
    requestedMIDIChannels = newMIDIChannelAssignments;
  }

private:
  std::vector<unsigned char> requestedMIDIChannels;

  Scale transactionScale;
}; // namespace Capabilities

} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_SCALE_FORMATS_SCALE_FORMAT_H_

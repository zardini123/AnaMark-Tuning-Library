#ifndef ANAMARK_TUNING_LIBRARY_SRC_SCALE_FORMATS_FORMAT_H_
#define ANAMARK_TUNING_LIBRARY_SRC_SCALE_FORMATS_FORMAT_H_

#include <fstream>
#include <set>
#include <stdexcept>

#include "ProvidesErrorMessage.h"

namespace AnaMark {

class Format {
public:
  using VersionNumber = unsigned short;

  constexpr static VersionNumber Version(VersionNumber majorVersionNumber,
                                         VersionNumber minorVersionNumber) {
    return (majorVersionNumber * 100) + minorVersionNumber;
  }

  struct FormatVersionRange {
    VersionNumber startingVersionInclusive;
    VersionNumber endingVersionInclusive;
  };

  ////////////////////////////////////////////////////////
  // Format information
  ////////////////////////////////////////////////////////

  virtual const char *FormatName() = 0;
  virtual const char *FormatSpecificationsURL() = 0;

  virtual const std::set<VersionNumber> &AvaliableVersions() = 0;

  VersionNumber NewestVersion() {
    return *AvaliableVersions().rbegin();
  }

  ////////////////////////////////////////////////////////
  // Loaded Format information
  ////////////////////////////////////////////////////////

  // Returns -1 if no valid scale was loaded
  VersionNumber LoadedVersion() const {
    return loadedFormatVersion;
  }

  ////////////////////////////////////////////////////////
  // Exceptions and Validation
  ////////////////////////////////////////////////////////

  void ThrowIfVersionDoesNotExist(VersionNumber versionNumberToCheck) {
    if (AvaliableVersions().count(versionNumberToCheck) == 0) {
      throw std::invalid_argument(
          std::string("Version ") + std::to_string(versionNumberToCheck) +
          std::string(" is not a possible version number for ") + FormatName());
    }
  }

  void ThrowIfVersionRangeIsInvalid(FormatVersionRange rangeToCheck) {
    if (AvaliableVersions().count(rangeToCheck.startingVersionInclusive) == 0) {
      throw std::invalid_argument(
          std::string("Starting version number inclusion ") +
          std::to_string(rangeToCheck.startingVersionInclusive) +
          std::string(" is not a possible version number for ") + FormatName());
    }
    if (AvaliableVersions().count(rangeToCheck.endingVersionInclusive) == 0) {
      throw std::invalid_argument(
          std::string("Ending version number inclusion ") +
          std::to_string(rangeToCheck.endingVersionInclusive) +
          std::string(" is not a possible version number for ") + FormatName());
    }
  }

protected:
  // MUST be called after a scale is successfully read
  void SetLoadedFormatVersion(VersionNumber loadedFormatVersionIn) {
    loadedFormatVersion = loadedFormatVersionIn;
  }

private:
  VersionNumber loadedFormatVersion = -1;
};

} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_SCALE_FORMATS_FORMAT_H_

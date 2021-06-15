#ifndef ANAMARK_TUNING_LIBRARY_SRC_SCALE_FORMATS_SCALE_MANAGER_FORMAT_H_
#define ANAMARK_TUNING_LIBRARY_SRC_SCALE_FORMATS_SCALE_MANAGER_FORMAT_H_

#include <fstream>
#include <vector>

#include "../Format.h"
#include "../ScaleManager.h"

namespace AnaMark {

class ScaleCollectionFormat : public Format {
public:
  ////////////////////////////////////////////////////////
  // Format I/O
  ////////////////////////////////////////////////////////

  ////////////////////////////
  // READ

  virtual void ReadDirectlyToManager(ScaleManager &collectionToOverwrite,
                                     std::istream &inputStream) = 0;

  void Read(std::istream &inputStream) {
    return ReadDirectlyToManager(this->transactionManager, inputStream);
  }

  ////////////////////////////
  // WRITE

  virtual void WriteDirectlyFromManager(
      ScaleManager &collectionToReadFromImmediately, std::ostream &outputStream,
      FormatVersionRange formatVersionRangeToWrite) = 0;

  void WriteDirectlyFromManager(ScaleManager &collectionToReadFromImmediately,
                                std::ostream &outputStream,
                                VersionNumber formatVersion) {
    FormatVersionRange range;
    range.startingVersionInclusive = 0;
    range.endingVersionInclusive = formatVersion;
    return WriteDirectlyFromManager(
        collectionToReadFromImmediately, outputStream, range);
  }

  void Write(std::ostream &outputStream, VersionNumber formatVersionToWrite) {
    return WriteDirectlyFromManager(
        this->transactionManager, outputStream, formatVersionToWrite);
  }

private:
  ScaleManager transactionManager;
};

} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_SCALE_FORMATS_SCALE_MANAGER_FORMAT_H_

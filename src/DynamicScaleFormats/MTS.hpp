// If single scale, update scale
// If scale manager, update all scales
//
// void Update(mts data)
//
// bool ChangesReadyToOutput()

#ifndef ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_MTS_H_
#define ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_MTS_H_

#include "DSFScaleRepresentative.hpp"
#include "DynamicScaleFormat.hpp"

class MTS : public DynamicScaleFormat, public DSFScaleRepresentativeSenderAndRecieve {
  const Flags<Capabilities> FormatCapabilities(
      VersionNumber versionNumber) override {}

  void Update() {
    // If only for single scale, use single channel fallback (-1 midi channel)
    // If for manager, check all channels
  }
}

#endif // ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_MTS_H_

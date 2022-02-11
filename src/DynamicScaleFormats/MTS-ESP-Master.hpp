#ifndef ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_MTS_ESP_MASTER_H_
#define ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_MTS_ESP_MASTER_H_

#include "../AttacherProvider.hpp"
#include "DynamicScaleFormat.hpp"

#include "../../lib/MTS-ESP/Master/libMTSMaster.h"

#include <array>
#include <numeric>
#include <vector>

namespace AnaMark {

class MTSESPMaster {
public:
  class ScaleRepresentative : public ChangeAttacher, public StateAttacher {
  public:
    ScaleRepresentative() : frequenciesFromProvider{128} {}

    bool UpdateState() {
      // Utilitiy for attaching MTSESPMaster as a StateAttacher.
      // Requires UpdateState() to be called every moment the scale data is desired
      // to be updated, as it will fetch state from StateProvider.

      this->RequestAllNotesFromProvider(noteRangeFromProvider,
                                        frequenciesFromProvider);

      MTS_SetNoteTunings(frequenciesFromProvider.data());
    }

  private:
    std::vector<double> frequenciesFromProvider;
  }; // class ScaleRepresentative

  // @TODO: Crash reinitalization with MTS_Reinitialize()

  void RegisterMaster() {
    if (!MTS_CanRegisterMaster()) {
      // @TODO: Throw exception stating there is a master already instanced
      assert(false);
    }

    MTS_RegisterMaster();
  }

  void DeregisterMaster() {
    MTS_DeregisterMaster();
  }

  int GetClientCount() {
    return MTS_GetNumClients();
  }
};

} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_MTS_ESP_MASTER_H_

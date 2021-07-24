#ifndef ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_MTS_H_
#define ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_MTS_H_

#include "../AttacherProvider.hpp"
#include "DynamicScaleFormat.hpp"

namespace AnaMark {

class MTS : public ChangeAttacher, public ChangeProvider {
private:
  void RecieveChangeFromProvider(const ChangeProvider *const changeOrigin,
                                 const ChangeProvider *const notifier, int scaleNote,
                                 double newFrequency) override {}

  void RecieveChangeFromProvider(const ChangeProvider *const changeOrigin,
                                 const ChangeProvider *const notifier,
                                 std::vector<int> &scaleNotes,
                                 std::vector<double> &newFrequencies) override {}
};
} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_MTS_H_

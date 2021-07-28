#ifndef ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_MTS_ESP_MASTER_H_
#define ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_MTS_ESP_MASTER_H_

#include "../AttacherProvider.hpp"
#include "DynamicScaleFormat.hpp"

#include "../../lib/MTS-ESP/Master/libMTSMaster.h"

#include <array>
#include <vector>

namespace AnaMark {

class MTSESPMaster : public DynamicScaleFormat {};

} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_MTS_ESP_MASTER_H_

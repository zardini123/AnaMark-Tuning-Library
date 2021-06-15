#ifndef ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_DYNAMIC_SCALE_FORMAT_H_
#define ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_DYNAMIC_SCALE_FORMAT_H_

namespace AnaMark {

class DynamicScaleFormat : public Format {

  // Capabilities are properties of a SingleScale that may or may not be required by
  // a format, but may result in informational loss if exporting to such format if
  // the SingleScale contains the information
  enum class Capabilities {
    InputNotesToScale = 0b1,
    OutputNotesFromScale = 0b10,
    InputMultiChannelNotesToScale = 0b100,
    OutputMultiChannelNotesFromScale = 0b1000,
  };
};

} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_DYNAMIC_SCALE_FORMAT_H_

#ifndef ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_DYNAMIC_SCALE_FORMAT_H_
#define ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_DYNAMIC_SCALE_FORMAT_H_

namespace AnaMark {

class DynamicScaleFormat : public Format {
public:
  enum class Capabilities {
    InputNotesToScale = 1 << 0,
    OutputNotesFromScale = 1 << 1,
    InputMultiChannelNotesToScale = 1 << 2,
    OutputMultiChannelNotesFromScale = 1 << 3,
  };

  enum class Assignment {
    Input = 1 << 0,
    Output = 1 << 1,
    InputAndOutput = Input | Output,
  };

  virtual const Flags<Capabilities> FormatCapabilities(
      VersionNumber versionNumber) = 0;

  // I want to send my changes to a scale
  // I want a scale to send its changes to me

  // RegisterOutputScale(Scale *)
  // RegisterInputScale(Scale *)

  // RegisterOutputScales()
};

} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_DYNAMIC_SCALE_FORMAT_H_

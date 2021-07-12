#ifndef ANAMARK_TUNING_LIBRARY_SRC_SCALE_COLLECTION_H_
#define ANAMARK_TUNING_LIBRARY_SRC_SCALE_COLLECTION_H_

namespace AnaMark {

class ScaleManager {
public:
  double FrequencyAtScaleNote(int scaleNote, int midiChannel) {}

  double ChangeFrequencyAtNote(int scaleNote, int midiChannel, double newFrequency) {

  }

private:
  // Map channel to scale
};
} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_SCALE_COLLECTION_H_

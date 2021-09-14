#ifndef ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_MTS_ESP_CLIENT_H_
#define ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_MTS_ESP_CLIENT_H_

#include "../AttacherProvider.hpp"
#include "DynamicScaleFormat.hpp"

#include "../../lib/MTS-ESP/Client/libMTSClient.h"

#include <array>
#include <map>

namespace AnaMark {

class MTSESPClient {
public:
  class ScaleRepresentative : public StateProvider {
    friend MTSESPClient;

  public:
    ScaleRepresentative(MTSClient *mtsClientIn, char midiChannelIn)
        : mtsClient{mtsClientIn}, midiChannel{midiChannelIn} {}

    Flags<Capabilities> StateProviderCapabilities() override {
      return {
          Capabilities::ScaleName,
          Capabilities::FilterMIDINotes,
      };
    }

    void HasBeenRequestedState(int scaleNoteToAcquire,
                               double &scaleNoteFrequencyOutput) override {
      assert(mtsClient != nullptr);
      assert(midiChannel >= -1 && midiChannel < 16);
      // @TODO: Send a rejection message/response stating scaleNoteToAcquire is out
      // of bounds
      // @TODO: Make the bounds (0-127) a class constant or part of the API
      assert(scaleNoteToAcquire >= 0 && scaleNoteToAcquire < 128);

      scaleNoteFrequencyOutput = MTS_NoteToFrequency(
          mtsClient, static_cast<char>(scaleNoteToAcquire), midiChannel);
    }

    MTSClient *mtsClient = nullptr;
    char midiChannel;
  };

  MTSESPClient()
      : mtsClient{MTS_RegisterClient()}, singleChannelScaleRep{mtsClient, -1} {}

  ~MTSESPClient() {
    MTS_DeregisterClient(mtsClient);
  }

  ScaleRepresentative &MultiChannel(char midiChannel) {
    assert(midiChannel >= 0 && midiChannel < 16);

    auto existingIt = multiChannelScaleReps.insert(
        {midiChannel, ScaleRepresentative(mtsClient, midiChannel)});

    return existingIt.first->second;
  }

  ScaleRepresentative &SingleChannel() {
    return singleChannelScaleRep;
  }

  bool ConnectedToAMTSESPMaster() {
    return MTS_HasMaster(mtsClient);
  }

private:
  // mtsClient must be first in parameter list here as therefore it will be
  // initalized first in the initalizer list
  MTSClient *mtsClient;
  ScaleRepresentative singleChannelScaleRep;
  std::map<char, ScaleRepresentative> multiChannelScaleReps;
};

} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_MTS_ESP_CLIENT_H_

// Access tuning for note.  MTS-ESP only has NoteToFrequency and ShouldFilterNote.
// If note access for scale is outside 0-127
// and dynamic sale input format is 0-127 bound, see what formula affects queried
// note.
//    Query dynamic scale input format attached to scale/manager for frequency at
//    that note. If different from cached, update base formula or fomula list to
//    match new frequency. Return new frequency.
// Else,
//    Query dynamic scale input formla at queried note.
//    If different from cached, update cache to new frequency.
//    Add formula for this note to match frequency.
//    Simplify the formula list.  (Updates formula dependency table).
//    Return new frequency.
//
// Formulas are needed for:
//    Extending outside 0-127
//    Complex/easy scale creation/manipulation
//    Periodicity for +/- 1 period oscillator and other possible usages
//
// Always should simplify formulas on scale change as required for periodicity.
//    Periodicity zones.
//    Formula dependency per note.
// Notify subscriber (callback) of scale change.
//
// If scale is being changed but no notes are being queried, do not block processing
// by calculating formula changes for every change.
//    Instead calculate formula changes if the UI requests formulas, which therefore
//    should calculate the formulas on the UI thread, not the processor thread.
//    Therefore the API should not expose the formula array directly, and instead
//    provide a function.  If the scale hasnt been updated yet (because no note
//    queries), update it before providing formulas to API user.
//    RACE CONDITIIONS!
//
// MTS-ESP does not know when note change as MTS-ESP API does not provide such
// listening. Only can check if note changed by query of frequency at note by API
// user.
//    Cannot update formulas until note is queried.
// MTS knows when note change as its provided by API user.
//    Can update formulas on message provision.
//    Can instead store current MTS scale (0-127) and check if differences when
//        querying notes, like with MTS-ESP interface.
//        To avoid recalculating formulas for
//
// Only method of non-blocking way of determining change is via storing the current
// formula set seperately from the scale formula set.
//
// Modifying scale formulas via UI is a input dynamic scale format.
//
// Memory:
//    0-127 vector.  1 KB.
//    0-127 vector for MTS-ESP client  1 KB.
//    Cache for outside 0-127 (variable).
//    Formulas (variable).
//    Formulas of UI (variable).
//
// Input: Client acquires new tuning (access note)

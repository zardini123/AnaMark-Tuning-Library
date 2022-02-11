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
    ScaleRepresentative(char midiChannelIn) : midiChannel{midiChannelIn} {}

    Flags<Provisions> StateProvisions() override {
      return {
          Provisions::Note, Provisions::MultipleNotes,
          // @TODO: Provisions::ScaleName,
          // @TODO: Provisions::FilterMIDINotes,
      };
    }

    NoteRange ProvisioningNoteRange() const override {
      return {0, 128};
    }

    double FrequencyForMIDINote(int midiNote) override {
      assert(mtsClient != nullptr);

      // @TODO: Return out of bounds response when midiNote out of provisioning range

      return MTS_NoteToFrequency(
          mtsClient, static_cast<char>(midiNote), midiChannel);
    }

    RequestResponse HasBeenRequestedNote(int scaleNoteToAcquire,
                                         double &scaleNoteFrequencyOutput) override {
      // @TODO: When a user of a ScaleRepresentative tries to obtain state from it
      // when the ScaleRepresentative has no mtsClient anymore (deregistered), should
      // the library assert, throw an exception, or return a error through the API
      // stating a problem?
      assert(mtsClient != nullptr);
      assert(midiChannel >= -1 && midiChannel < 16);

      // @TODO: Make the bounds (0-127) a class constant or part of the API
      if (scaleNoteToAcquire < 0 || scaleNoteToAcquire >= 128) {
        return RequestResponse::RequestedNoteOutOfBounds;
      }

      // @TODO: Create midi to scale note mapping provision, providing 1:1 mapping of
      // midi to scale note

      scaleNoteFrequencyOutput = FrequencyForMIDINote(scaleNoteToAcquire);

      return RequestResponse::RequestOK;
    }

    RequestResponse HasBeenRequestedMultipleNotes(
        NoteRange requestedNoteRange,
        std::vector<double> &scaleNotesFrequenciesOutput) override {
      assert(false);
    }

  private:
    MTSClient *mtsClient = nullptr;
    char midiChannel;
  }; // class ScaleRepresentative

  MTSESPClient() : singleChannelScaleRep{-1} {}

  void RegisterClient() {
    if (mtsClient == nullptr) {
      mtsClient = MTS_RegisterClient();

      UpdateRepresentativesMTSClient();
    }
  }

  void DeregisterClient() {
    if (mtsClient != nullptr) {
      MTS_DeregisterClient(mtsClient);

      mtsClient = nullptr;
      UpdateRepresentativesMTSClient();
    }
  }

  bool ClientRegistered() {
    return mtsClient != nullptr;
  }

  ~MTSESPClient() {
    DeregisterClient();
  }

  ScaleRepresentative &MultiChannelScale(char midiChannel) {
    assert(midiChannel >= 0 && midiChannel < 16);

    auto existingIt = multiChannelScaleReps.insert(
        {midiChannel, ScaleRepresentative(midiChannel)});

    // existingIt of type pair<iterator,bool>
    // bool is if the key existed or not
    ScaleRepresentative &theScaleRep = existingIt.first->second;

    return theScaleRep;
  }

  ScaleRepresentative &SingleChannelScale() {
    return singleChannelScaleRep;
  }

  bool IsConnectedToAMaster() {
    return MTS_HasMaster(mtsClient);
  }

private:
  void UpdateRepresentativesMTSClient() {
    singleChannelScaleRep.mtsClient = mtsClient;
    for (auto &multiChannelRep : multiChannelScaleReps) {
      multiChannelRep.second.mtsClient = mtsClient;
    }
  }

  // mtsClient must be first in parameter list here as therefore it will be
  // initalized first in the initalizer list
  MTSClient *mtsClient = nullptr;

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

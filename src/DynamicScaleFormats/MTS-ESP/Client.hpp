#ifndef ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_MTS_ESP_CLIENT_H_
#define ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_MTS_ESP_CLIENT_H_

#include "../../AttacherProvider.hpp"
#include "../DynamicScaleFormat.hpp"

#include "../../../lib/MTS-ESP/Client/libMTSClient.h"

#include <map>
#include <vector>

namespace AnaMark {

namespace MTSESP {

class Client;

class ClientScaleRepresentative : public StateProvider {
  friend Client;

public:
  ClientScaleRepresentative(char midiChannelIn)
      : mtsClient{nullptr}, midiChannel{midiChannelIn} {}

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
    // @TODO: Determine best way to notify user that they are attempting to acquire
    // MTSESP data with a client that is not connected.
    //
    // When a user of a ClientScaleRepresentative tries to obtain state from it when
    // the ClientScaleRepresentative has no mtsClient anymore (deregistered), should
    // the library assert, throw an exception, or return a error through the API
    // stating a problem?
    assert(mtsClient != nullptr);

    // @TODO: Return out of bounds response when midiNote out of provisioning range
    assert(ProvisioningNoteRange().HasNoteInside(midiNote));

    return MTS_NoteToFrequency(mtsClient, static_cast<char>(midiNote), midiChannel);
  }

  RequestResponse HasBeenRequestedNote(int scaleNoteToAcquire,
                                       double &scaleNoteFrequencyOutput) override {
    // @TODO: See FrequencyForMidiNote todo
    assert(mtsClient != nullptr);
    assert(midiChannel >= -1 && midiChannel < 16);
    assert(ProvisioningNoteRange().HasNoteInside(scaleNoteToAcquire));

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
    // @TODO: HasBeenRequestedMultipleNotes()
    assert(false);
  }

private:
  MTSClient *mtsClient;
  char midiChannel;
}; // class ClientScaleRepresentative

class Client
    : public ScaleCollection<ClientScaleRepresentative, ClientScaleRepresentative> {
public:
  Client() {
    // Setup individual channel scale reps
    for (char channel = 0; channel < 16; ++channel) {
      this->individualChannelReps.emplace(channel,
                                          ClientScaleRepresentative(channel));
    }

    // Setup no channel scale rep
    this->noChannelReps.emplace_back(ClientScaleRepresentative(-1));
  }

  Flags<Properties> CollectionProperties() override {
    return {
        Properties::HasScalesAffectingIndiviualChannels,
        Properties::HasScalesWithNoChannelAssignment,
        Properties::OnlyOneNoChannelScale,
    };
  }

  void RegisterClient() {
    if (mtsClient == nullptr) {
      mtsClient = MTS_RegisterClient();

      UpdateScaleRepMTSClientPointers();
    }
  }

  void DeregisterClient() {
    if (mtsClient != nullptr) {
      MTS_DeregisterClient(mtsClient);
      mtsClient = nullptr;

      UpdateScaleRepMTSClientPointers();
    }
  }

  bool ClientRegistered() {
    return mtsClient != nullptr;
  }

  ~Client() {
    DeregisterClient();
  }

  bool IsConnectedToAMaster() {
    return MTS_HasMaster(mtsClient);
  }

private:
  void UpdateScaleRepMTSClientPointers() {
    // Update individual channel scale reps
    for (auto &scaleRep : this->individualChannelReps) {
      scaleRep.second.mtsClient = mtsClient;
    }

    // Update no channel scale rep
    for (ClientScaleRepresentative &scaleRep : this->noChannelReps) {
      scaleRep.mtsClient = mtsClient;
    }
  }

  MTSClient *mtsClient = nullptr;
};

} // namespace MTSESP

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

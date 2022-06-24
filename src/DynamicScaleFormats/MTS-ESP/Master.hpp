#ifndef ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_MTS_ESP_MASTER_H_
#define ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_MTS_ESP_MASTER_H_

#include "../../AttacherProvider.hpp"
#include "../DynamicScaleFormat.hpp"

#include "../../../lib/MTS-ESP/Master/libMTSMaster.h"

#include <array>
#include <numeric>
#include <vector>

namespace AnaMark {

namespace MTSESP {

class MasterScaleRepresentative : public ChangeAttacher, public StateAttacher {
public:
  MasterScaleRepresentative(char midiChannelIn)
      : frequenciesFromProvider{128}, midiChannel{midiChannelIn} {}

  // Attacher

  NoteRange TunableNoteRange() const override {
    return {0, 128};
  }

  // End Attacher

  // ChangeAttacher

  Flags<Attachments> ChangeAttachments() override {
    return {
        Attachments::Note, Attachments::MultipleNotes,
        // @TODO: Attachments::ScaleName,
        // @TODO: Attachments::FilterMIDINotes,
    };
  }

  void RecieveNoteChangeFromProvider(const ChangeProvider *const notificationOrigin,
                                     int scaleNote, double newFrequency) override {
    assert(TunableNoteRange().HasNoteInside(scaleNote));

    SetNoteTuning(scaleNote, newFrequency);
  }

  void RecieveMultipleNotesChangeFromProvider(
      const ChangeProvider *const notificationOrigin, NoteRange changedNoteRange,
      std::vector<double> &newFrequencies) override {
    assert(changedNoteRange.IsNotOutside(TunableNoteRange()));

    for (auto i = changedNoteRange.firstNote; i < changedNoteRange.afterLastNote;
         ++i) {
      auto newFrequenciesIndex = changedNoteRange.firstNote - i;

      frequenciesFromProvider[i] = newFrequencies[newFrequenciesIndex];
    }

    SetNoteTunings();
  }

  // End ChangeAttacher

  // StateAttacher

  Flags<Attachments> StateAttachments() override {
    return ChangeAttachments();
  }

  void Import() override {
    // Requires UpdateState() to be called every moment the scale data is desired
    // to be updated, as it will fetch state from StateProvider.

    this->RequestMultipleNotesFromProvider(this->TunableNoteRange(),
                                           frequenciesFromProvider);

    SetNoteTunings();

    // @TODO: Scale name
    // @TODO: Note filters
  }

  // End StateAttacher

private:
  void ChannelUsed() {
    // Only can set used midi channel when multi channel
    if (midiChannel == -1) {
      return;
    }

    MTS_SetMultiChannel(true, midiChannel);
    mtsChannelEnabled = true;
  }

  void SetNoteTuning(int scaleNote, double newFrequency) {
    ChannelUsed();

    if (midiChannel == -1) {
      MTS_SetNoteTuning(newFrequency, static_cast<char>(scaleNote));
    } else {
      MTS_SetMultiChannelNoteTuning(
          newFrequency, static_cast<char>(scaleNote), midiChannel);
    }
  }

  void SetNoteTunings() {
    ChannelUsed();

    if (midiChannel == -1) {
      MTS_SetNoteTunings(frequenciesFromProvider.data());
    } else {
      MTS_SetMultiChannelNoteTunings(frequenciesFromProvider.data(), midiChannel);
    }
  }

  std::vector<double> frequenciesFromProvider;
  char midiChannel;
  bool mtsChannelEnabled;
}; // class MasterScaleRepresentative

class Master
    : public ScaleCollection<MasterScaleRepresentative, MasterScaleRepresentative> {
public:
  Master() {
    // @TODO: Defer creating scale representatives until API user requests it.
    // Requires changes in ScaleCollection API to allow creation of scale rep on
    // request of channel, and requesting inheriter if a scale rep is active or not.

    // Setup individual channel scale reps
    for (char channel = 0; channel < 16; ++channel) {
      this->individualChannelReps.emplace(channel,
                                          MasterScaleRepresentative(channel));
    }

    // Setup no channel scale rep
    this->noChannelReps.emplace_back(MasterScaleRepresentative(-1));
  }

  Flags<Properties> CollectionProperties() override {
    return {
        Properties::HasScalesAffectingIndiviualChannels,
        Properties::HasScalesWithNoChannelAssignment,
        Properties::OnlyOneNoChannelScale,
    };
  }

  // Completely reset MTS-ESP library.
  // To be called when MTSESPIsUsingIPC() and the program using this library crashes
  // when using an established MTSESP::Master.
  void ReinitalizeMTSESP() {
    if (!MTSESPIsUsingIPC()) {
      // @TODO: Throw exception notifying API user that MTS-ESP library maintainers
      // request that this function is only clalled when MTS-ESP has IPC in use.
      assert(false);
    }

    MTS_Reinitialize();
  }

  bool MTSESPIsUsingIPC() {
    return MTS_HasIPC();
  }

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
}; // class Master

} // namespace MTSESP

} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_MTS_ESP_MASTER_H_

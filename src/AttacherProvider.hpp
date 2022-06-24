#ifndef ANAMARK_TUNING_LIBRARY_SRC_OBSERVER_SUBJECT_H_
#define ANAMARK_TUNING_LIBRARY_SRC_OBSERVER_SUBJECT_H_

#include "Utilities.hpp"

#include <cassert>
#include <limits>
#include <map>
#include <set>
#include <vector>

namespace AnaMark {

// https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern

// @TODO: Do any dynamic collection formats exist?
// @TODO: Enforce template types to be of a ScaleRepresentative type
template <class ChannelScaleRepresentative, class NoChannelScaleRepresentative>
class ScaleCollection {
public:
  using ChannelNumber = int;

  enum class Properties {
    // Some formats (MSF, MTS-ESP) provide scale descriptions scales that affect
    // individual channels.  Some formats (MTS-ESP) allow both an all channel (single
    // channel) description and a individual channel (multi channel) descriptions.
    // Some formats (MSF) do not allow both.
    HasScalesAffectingAllChannels = 1 << 0,
    HasScalesAffectingIndiviualChannels = 1 << 1,
    HasScalesWithNoChannelAssignment = 1 << 2,
    // Without this flag set, the last all-channel scale in format is the scale that
    // overrides all.
    FirstAllChannelScaleOverridesAll = 1 << 3,
    // When enabled, provider collection only provides one scale in no-channel
    // collection (MTS-ESP).
    OnlyOneNoChannelScale = 1 << 5,
  };

  virtual Flags<Properties> CollectionProperties() = 0;

  // @TODO: ChannelIterator
  // @TODO: No channel, all channel, and individual channel read-only direct access
  // @TODO: Provide API for user to add/delete no channel, all channel, and
  // individual channel scale representatives, but allow inheriters of
  // ScaleCollection to force collection as immutable, and therefore require
  // making a copy before modifying the collection.

  // Provides scale representative that is mapped to a specific channel.
  //
  // Does not provide direct access to scales mapped to individual channels.
  // Collections with all-channel scales will return either the first or last
  // accounted all-channel scale regardless of midi channel (as per definition of
  // a scale being for all channels).  Therefore, if direct access of individual
  // channel scales is required, API user must use collection API to iterate over
  // individually mapped scale representatives.
  //
  // Function never returns any no-channel scales.  Use other functions for accessing
  // them.
  ChannelScaleRepresentative &ScaleForChannel(ChannelNumber channel) {
    if (!allChannelReps.empty()) {
      if (CollectionProperties().AreFlagsSet(
              Properties::FirstAllChannelScaleOverridesAll)) {
        return allChannelReps.front();
      }

      return allChannelReps.back();
    }

    auto scaleForChannelIt = individualChannelReps.find(channel);
    if (scaleForChannelIt == individualChannelReps.end()) {
      // @TODO: Throw exception that scale representative for channel does not exist.
      // Recommend if user is iterating over channels dynamically to use channel
      // range iterators (@TODO).
      assert(false);
    }

    // scaleForChannelIt of type pair<const key_type, mapped_type>
    return scaleForChannelIt->second;
  }

  NoChannelScaleRepresentative &NoChannelScale() {
    if (noChannelReps.size() != 1) {
      // @TODO: Throw exception stating request for no channel scale is ambiguous.
      // Requires user to use collection API to iterate over no-channel scale
      // representatives.
      assert(false);
    }

    return noChannelReps.front();
  }

protected:
  // Format may provide an order to their no-channel scales, therefore collection
  // uses vector over a set.
  std::vector<NoChannelScaleRepresentative> noChannelReps;
  std::vector<ChannelScaleRepresentative> allChannelReps;
  std::map<ChannelNumber, ChannelScaleRepresentative> individualChannelReps;
};

enum class RequestResponse {
  RequestOK = 0,
  NoNoteRequestSupport,
  RequestedNoteOutOfBounds,
  NoMultipleNotesRequestSupport,
  RequestedNoteRangeOutOfBounds
};

enum class ScaleProperties {
  Note = 1 << 0,
  MultipleNotes = 1 << 1,
  ScaleName = 1 << 2,
  FilterMIDINotes = 1 << 3,
  MIDINoteToScaleNoteMapping = 1 << 4,
  // @TODO: Periodic Scale Provisions
};

class Provider {
  // Providers represent one scale
public:
  using Provisions = ScaleProperties;

  // Note range the provider provides tuning for
  virtual NoteRange ProvisioningNoteRange() const = 0;

  // @TODO: Change function to provide response struct, especially for when midiNote
  // out of provisioning range
  virtual double FrequencyForMIDINote(int midiNote) = 0;

  // @TODO: FrequencyAtScaleNote()
};

class Attacher {
  // Attachers can only track one representation of a scale provided by a Provider.
  // Provisioning of scale is transported via change from a
  // - Provider change (ChangeProvider)
  // - API change
  // or state
  // - Request state from a Provider (StateProvider)
public:
  using Attachments = ScaleProperties;

  bool IsAttachedToAProvider() const {
    return attached;
  }

  bool CanAttachToAProvider() const {
    return !IsAttachedToAProvider();
  }

  // @TODO: OnAttachToProvider()
  // @TODO: OnDetachFromProvider()

  // Range to tune.
  // "If musician wants something to span the complete range, he has to tune it for
  // the complete range."
  // Optimize for what musicians consider the common case (0-127 scale notes, or
  // whatever they consider).

  // If an Attacher attaches to a Provider where ANY notes in the TunableNoteRange of
  // the Attacher is OUTSIDE the ProvisioningNoteRange of the Provider, DO NOT allow
  // the attacher to attach as the Attacher would track notes that do not exist.

  // @TODO: Attachers can track a partial representation of a scale if user of API
  // forcfully understands this issue.
  virtual NoteRange TunableNoteRange() const = 0;

protected:
  void SetStatusToAttached() {
    attached = true;
  }

  void RemoveAttachedStatus() {
    attached = false;
  }

private:
  bool attached = false;
};

class StateProvider : public virtual Provider {
public:
  virtual Flags<Provisions> StateProvisions() = 0;

  virtual RequestResponse HasBeenRequestedNote(int scaleNoteToAcquire,
                                               double &scaleNoteFrequencyOutput) = 0;

  virtual RequestResponse HasBeenRequestedMultipleNotes(
      NoteRange requestedNoteRange,
      std::vector<double> &scaleNotesFrequenciesOutput) = 0;

  // @TODO: HasBeenRequestedScaleName(): Route to request scale name
  // @TODO: HasBeenRequestedAllNotes(): Route to request all avaliable scale notes
};

class StateAttacher : public virtual Attacher {
public:
  virtual Flags<Attachments> StateAttachments() = 0;

  bool IsAttachedToAStateProvider() {
    return attachedStateProvider != nullptr;
  }

  void AttachToStateProvider(StateProvider *subjectToRegister) {
    if (IsAttachedToAProvider()) {
      // @TODO: Throw exception stating there is a subject already attached
      assert(false);
    }

    attachedStateProvider = subjectToRegister;
    SetStatusToAttached();
  }

  StateProvider *DetachFromStateProvider() {
    if (!IsAttachedToAStateProvider()) {
      // @TODO: Throw exception stating attempting to detach but there is no attached
      // state subject
      assert(false);
    }

    StateProvider *oldAttachedStateProvider = attachedStateProvider;

    attachedStateProvider = nullptr;
    RemoveAttachedStatus();

    return oldAttachedStateProvider;
  }

  // Utility for requesting all provisions that this StateAttacher has attachments
  // for to the StateProvider this StateAttacher is attached to. Calling this
  // function fully updates this attacher to have the same scale states as the
  // provider (for all attachments only).
  virtual void Import() = 0;

protected:
  // @TODO: Migrate return types to RequestResponse

  bool RequestNoteFromProvider(int scaleNoteToAcquire,
                               double &scaleNoteFrequencyOutput) {
    if (!IsAttachedToAProvider()) {
      return true;
    }

    // @FIXME: Return request response
    attachedStateProvider->HasBeenRequestedNote(scaleNoteToAcquire,
                                                scaleNoteFrequencyOutput);
    return false;
  }

  bool RequestMultipleNotesFromProvider(
      NoteRange noteRangeToRequest,
      std::vector<double> &scaleNotesFrequenciesOutput) {
    if (!IsAttachedToAProvider()) {
      return true;
    }

    // @FIXME: Return request response
    attachedStateProvider->HasBeenRequestedMultipleNotes(
        noteRangeToRequest, scaleNotesFrequenciesOutput);
    return false;
  }

private:
  StateProvider *attachedStateProvider = nullptr;
};

// Forward declare ChangeAttacher as ChangeProvider needs to know of ChangeAttacher
// name and ChangeAttacher needs to know of ChangeProvider name.
class ChangeAttacher;

class ChangeProvider : public virtual Provider {
  friend ChangeAttacher;

public:
  virtual Flags<Provisions> ChangeProvisions() = 0;

protected:
  void NotifyAttachersOfNoteChange(const ChangeProvider *const notificationOrigin,
                                   int scaleNote, double newFrequency);

  void NotifyAttachersOfMultipleNotesChange(
      const ChangeProvider *const notificationOrigin, NoteRange changedNoteRange,
      std::vector<double> &newFrequencies);

private:
  void AddChangeAttacher(ChangeAttacher *observerToRegister) {
    attachedChangeAttachers.push_back(observerToRegister);
  }

  void RemoveChangeAttacher(ChangeAttacher *observerToDeregister) {
    auto existingAttacherIt = Utilities::Find(attachedChangeAttachers.begin(),
                                              attachedChangeAttachers.end(),
                                              observerToDeregister);

    if (existingAttacherIt == attachedChangeAttachers.end()) {
      // @TODO: Throw exception stating observer does not exist or has already been
      // removed.
      assert(false);
    }

    attachedChangeAttachers.erase(existingAttacherIt);
  }

  std::vector<ChangeAttacher *> attachedChangeAttachers;
};

// The Attacher only cares about what it needs to do when informed. It doesn't care
// about registering/unregistering with anyone.
class ChangeAttacher : public virtual Attacher {
  friend ChangeProvider;

public:
  virtual Flags<Attachments> ChangeAttachments() = 0;

  bool IsAttachedToAChangeProvider() {
    return attachedChangeProvider != nullptr;
  }

  void AttachToChangeProvider(ChangeProvider *subjectToRegister) {
    if (IsAttachedToAProvider()) {
      // @TODO: Throw exception stating there is a subject already attached
      assert(false);
    }

    attachedChangeProvider = subjectToRegister;
    attachedChangeProvider->AddChangeAttacher(this);
    SetStatusToAttached();
  }

  ChangeProvider *DetachFromChangeProvider() {
    if (!IsAttachedToAChangeProvider()) {
      // @TODO: Throw exception stating attempting to detach but there is no attached
      // state subject
      assert(false);
    }

    ChangeProvider *oldAttachedChangeProvider = attachedChangeProvider;

    attachedChangeProvider->RemoveChangeAttacher(this);
    attachedChangeProvider = nullptr;
    RemoveAttachedStatus();

    return oldAttachedChangeProvider;
  }

  // @TODO: Migrate to RequestReponse return types

  bool ChangeScaleNoteFrequency(int scaleNote, double newFrequency) {
    // Do not allow users to modify frequencies if scale is already attached to a
    // provider.
    if (!IsAttachedToAProvider()) {
      return true;
    }

    // @TODO: Complain if outside TunableNoteRange()

    RecieveNoteChangeFromProvider(nullptr, scaleNote, newFrequency);
    return false;
  }

  bool ChangeScaleNotesFrequencies(NoteRange noteRangeToChange,
                                   std::vector<double> &newFrequencies) {
    // Do not allow users to modify frequencies if scale is already attached to a
    // provider.
    if (!IsAttachedToAProvider()) {
      return true;
    }

    // @TODO: Complain if outside TunableNoteRange()

    RecieveMultipleNotesChangeFromProvider(
        nullptr, noteRangeToChange, newFrequencies);
    return false;
  }

protected:
  virtual void RecieveNoteChangeFromProvider(
      const ChangeProvider *const notificationOrigin, int scaleNote,
      double newFrequency) = 0;

  virtual void RecieveMultipleNotesChangeFromProvider(
      const ChangeProvider *const notificationOrigin, NoteRange changedNoteRange,
      std::vector<double> &newFrequencies) = 0;

  ChangeProvider *attachedChangeProvider = nullptr;
};

// Define following functions later to have knowledge of ChangeAttacher functions
// Recieve*

inline void ChangeProvider::NotifyAttachersOfNoteChange(
    const ChangeProvider *notificationOrigin, int scaleNote, double newFrequency) {

  if (notificationOrigin == nullptr) {
    notificationOrigin = this;
  }

  for (auto &attachedAttacher : attachedChangeAttachers) {
    // Avoid providing change to attacher if the provider of change is the same as
    // the one about to recieve the change.  Prevents feedback loops.
    if (dynamic_cast<const ChangeAttacher *>(notificationOrigin) ==
        attachedAttacher) {
      continue;
    }

    // "this" is the notifier of change
    attachedAttacher->RecieveNoteChangeFromProvider(
        notificationOrigin, scaleNote, newFrequency);
  }
}

inline void ChangeProvider::NotifyAttachersOfMultipleNotesChange(
    const ChangeProvider *notificationOrigin, NoteRange changedNoteRange,
    std::vector<double> &newFrequencies) {

  if (notificationOrigin == nullptr) {
    notificationOrigin = this;
  }

  for (auto &attachedAttacher : attachedChangeAttachers) {
    // Avoid providing change to attacher if the provider of change is the same as
    // the one about to recieve the change.  Prevents feedback loops.
    if (dynamic_cast<const ChangeAttacher *>(notificationOrigin) ==
        attachedAttacher) {
      continue;
    }

    attachedAttacher->RecieveMultipleNotesChangeFromProvider(
        notificationOrigin, changedNoteRange, newFrequencies);
  }
}

} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_OBSERVER_SUBJECT_H_

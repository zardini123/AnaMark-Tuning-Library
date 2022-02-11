#ifndef ANAMARK_TUNING_LIBRARY_SRC_OBSERVER_SUBJECT_H_
#define ANAMARK_TUNING_LIBRARY_SRC_OBSERVER_SUBJECT_H_

#include "Utilities.hpp"

#include <cassert>
#include <limits>
#include <vector>

namespace AnaMark {

enum class RequestResponse {
  RequestOK = 0,
  NoNoteRequestSupport,
  RequestedNoteOutOfBounds,
  NoMultipleNotesRequestSupport,
  RequestedNoteRangeOutOfBounds
};

class Provider {
public:
  enum class Provisions {
    Note = 1 << 0,
    MultipleNotes = 1 << 1,
    ScaleName = 1 << 2,
    FilterMIDINotes = 1 << 3,
    MIDINoteToScaleNoteMapping = 1 << 4,
  };

  // Note range the provider provides tuning for
  virtual NoteRange ProvisioningNoteRange() const = 0;

  // @TODO: Change function to provide response, especially for when midiNote out of
  // provisioning range
  virtual double FrequencyForMIDINote(int midiNote) = 0;

  // @TODO: FrequencyAtScaleNote()
};

class Attacher {
  // Attachers can only track one representation of a scale, described (transported)
  // via change from a
  // - Provider change
  // - API change
  // or state
  // - Request state from a Provider
public:
  bool IsAttachedToAProvider() const {
    return attached;
  }

  bool CanAttachToAProvider() const {
    return !IsAttachedToAProvider();
  }

  // @TODO: OnAttachToProvider()
  // @TODO: OnDetachFromProvider()

  // If attacher requests note inside its TunableNoteRange that is outside
  // ProvisioningNoteRange, complain

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

  // @TODO: Route to request scale name
  // @TODO: Route to request all avaliable scale notes
};

class StateAttacher : public virtual Attacher {
public:
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

protected:
  bool RequestNoteFromProvider(int scaleNoteToAcquire,
                               double &scaleNoteFrequencyOutput) {
    if (!IsAttachedToAProvider()) {
      return true;
    }

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

    attachedStateProvider->HasBeenRequestedMultipleNotes(
        noteRangeToRequest, scaleNotesFrequenciesOutput);
    return false;
  }

private:
  StateProvider *attachedStateProvider = nullptr;
};

// Forward declare
class ChangeAttacher;

class ChangeProvider : public virtual Provider {
  friend ChangeAttacher;

protected:
  void NotifyAttachersOfNoteChange(const ChangeAttacher *const notificationOrigin,
                                   int scaleNote, double newFrequency);

  void NotifyAttachersOfMultipleNotesChange(
      const ChangeAttacher *const notificationOrigin, NoteRange changedNoteRange,
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

  bool ChangeScaleNoteFrequency(int scaleNote, double newFrequency) {
    // Do not allow users to modify frequencies if scale is already attached to a
    // provider.
    if (!IsAttachedToAProvider()) {
      return true;
    }

    // @TODO: Complain if outside TunableNoteRange()

    RecieveNoteChangeFromProvider(this, scaleNote, newFrequency);
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

    RecieveMultipleNotesChangeFromProvider(this, noteRangeToChange, newFrequencies);
    return false;
  }

private:
  virtual void RecieveNoteChangeFromProvider(
      const ChangeAttacher *const notificationOrigin, int scaleNote,
      double newFrequency) = 0;

  virtual void RecieveMultipleNotesChangeFromProvider(
      const ChangeAttacher *const notificationOrigin, NoteRange changedNoteRange,
      std::vector<double> &newFrequencies) = 0;

  ChangeProvider *attachedChangeProvider = nullptr;
};

// Define following functions later to have knowledge of ChangeAttacher functions
// Recieve*

inline void ChangeProvider::NotifyAttachersOfNoteChange(
    const ChangeAttacher *const notificationOrigin, int scaleNote,
    double newFrequency) {
  assert(notificationOrigin != nullptr);

  for (auto &attachedAttacher : attachedChangeAttachers) {
    // Prevent feedback loops
    if (notificationOrigin == attachedAttacher) {
      continue;
    }

    // "this" is the notifier of change
    attachedAttacher->RecieveNoteChangeFromProvider(
        notificationOrigin, scaleNote, newFrequency);
  }
}

inline void ChangeProvider::NotifyAttachersOfMultipleNotesChange(
    const ChangeAttacher *const notificationOrigin, NoteRange changedNoteRange,
    std::vector<double> &newFrequencies) {
  assert(notificationOrigin != nullptr);

  for (auto &attachedAttacher : attachedChangeAttachers) {
    // Prevent feedback loops
    if (notificationOrigin == attachedAttacher) {
      continue;
    }

    attachedAttacher->RecieveMultipleNotesChangeFromProvider(
        notificationOrigin, changedNoteRange, newFrequencies);
  }
}

} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_OBSERVER_SUBJECT_H_

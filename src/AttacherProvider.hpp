#ifndef ANAMARK_TUNING_LIBRARY_SRC_OBSERVER_SUBJECT_H_
#define ANAMARK_TUNING_LIBRARY_SRC_OBSERVER_SUBJECT_H_

#include "Utilities.hpp"

#include <cassert>
#include <limits>
#include <vector>

namespace AnaMark {

class Provider {};

class Attacher {
  // Can only track one representation of a scale, that being transported via change
  // or state
public:
  bool IsAttachedToAProvider() const {
    return attached;
  }

  bool CanAttachToAProvider() const {
    return !IsAttachedToAProvider();
  }

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
  virtual void GetNoteState(int scaleNoteToAcquire,
                            double &scaleNoteFrequencyOutput) = 0;

  virtual void GetNoteState(const std::vector<int> &scaleNotesToAcquire,
                            std::vector<double> &scaleNotesFrequenciesOutput) {
    scaleNotesFrequenciesOutput.clear();

    for (int scaleNote : scaleNotesToAcquire) {
      double frequencyOut = 0.0;
      GetNoteState(scaleNote, frequencyOut);
      scaleNotesFrequenciesOutput.push_back(frequencyOut);
    }
  }
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
  bool GetNoteState(int scaleNoteToAcquire, double &scaleNoteFrequencyOutput) {
    if (!IsAttachedToAProvider()) {
      return true;
    }

    attachedStateProvider->GetNoteState(scaleNoteToAcquire,
                                        scaleNoteFrequencyOutput);
    return false;
  }

  bool GetNoteState(std::vector<int> &scaleNotesToAcquire,
                    std::vector<double> &scaleNotesFrequenciesOutput) {
    if (!IsAttachedToAProvider()) {
      return true;
    }

    attachedStateProvider->GetNoteState(scaleNotesToAcquire,
                                        scaleNotesFrequenciesOutput);
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
  void NotifyAttachersOfChange(const ChangeProvider *const changeOrigin,
                               int scaleNote, double newFrequency);

  void NotifyAttachersOfChange(const ChangeProvider *const changeOrigin,
                               std::vector<int> &scaleNotes,
                               std::vector<double> &newFrequencies);

private:
  void AddChangeAttacher(ChangeAttacher *observerToRegister) {
    attachedAttachers.push_back(observerToRegister);
  }

  void RemoveChangeAttacher(ChangeAttacher *observerToDeregister) {
    auto existingAttacherIt = Utilities::Find(
        attachedAttachers.begin(), attachedAttachers.end(), observerToDeregister);

    if (existingAttacherIt == attachedAttachers.end()) {
      // @TODO: Throw exception stating observer does not exist or has already been
      // removed.
      assert(false);
    }

    attachedAttachers.erase(existingAttacherIt);
  }

  std::vector<ChangeAttacher *> attachedAttachers;
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

private:
  virtual void RecieveChangeFromProvider(const ChangeProvider *const changeOrigin,
                                         const ChangeProvider *const notifier,
                                         int scaleNote, double newFrequency) = 0;

  virtual void RecieveChangeFromProvider(const ChangeProvider *const changeOrigin,
                                         const ChangeProvider *const notifier,
                                         std::vector<int> &scaleNotes,
                                         std::vector<double> &newFrequencies) = 0;

  ChangeProvider *attachedChangeProvider = nullptr;
};

inline void ChangeProvider::NotifyAttachersOfChange(
    const ChangeProvider *const changeOrigin, int scaleNote, double newFrequency) {
  // Prevent feedback loops
  if (changeOrigin == this) {
    return;
  }

  for (auto &attachedAttacher : attachedAttachers) {
    attachedAttacher->RecieveChangeFromProvider(
        changeOrigin, this, scaleNote, newFrequency);
  }
}

inline void ChangeProvider::NotifyAttachersOfChange(
    const ChangeProvider *const changeOrigin, std::vector<int> &scaleNotes,
    std::vector<double> &newFrequencies) {
  // Prevent feedback loops
  if (changeOrigin == this) {
    return;
  }

  for (auto &attachedAttacher : attachedAttachers) {
    attachedAttacher->RecieveChangeFromProvider(
        changeOrigin, this, scaleNotes, newFrequencies);
  }
}

/*
template <unsigned int MaxRegistrations = std::numeric_limits<unsigned int>::max()>
class ChangeAttacherLimitedRegistrations : public ChangeAttacher {
public:
  bool CanAddNewAttacher() override {
    return subjectsUsingThis.size() < MaxRegistrations;
  }

private:
  void RegisteredWithProvider(ChangeProvider *subjectWasAdded) override {
    subjectsUsingThis.push_back(subjectWasAdded);
  }

  void DeregisteredWithProvider(ChangeProvider *subjectWasRemoved) override {
    auto existingProviderIt = Utilities::Find(
        subjectsUsingThis.begin(), subjectsUsingThis.end(), subjectWasRemoved);

    if (existingProviderIt == subjectsUsingThis.end()) {
      // @TODO: Throw exception stating observer does not exist or has already been
      // removed.
      assert(false);
    }

    subjectsUsingThis.erase(existingProviderIt);
  }

  std::vector<ChangeProvider *> subjectsUsingThis;
};
*/

} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_OBSERVER_SUBJECT_H_

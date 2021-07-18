#ifndef ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_DSF_SCALE_REPRESENTATIVE_H_
#define ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_DSF_SCALE_REPRESENTATIVE_H_

#include "../ChangeMediator.hpp"

#include <cassert>
#include <vector>

namespace AnaMark {

// class ChangeMediator;

// Send changes to mediator
class DSFScaleRepresentativeSender {
protected:
  void SendChangeToMediator(ChangeMediator &mediator, int scaleNote,
                            double newFrequency) {
    mediator.DSFScaleRepresentativeChange(this, scaleNote, newFrequency);
  }

  void SendChangesToMediator(ChangeMediator &mediator, std::vector<int> &scaleNotes,
                             std::vector<double> &newFrequencies) {
    assert(scaleNotes.size() == newFrequencies.size());
    if (scaleNotes.empty()) {
      return;
    }

    mediator.DSFScaleRepresentativeChanges(this, scaleNotes, newFrequencies);
  }

  // public:
  //   void RegisterSendChangesToScale(Scale *scale) {
  //     this->RegisterSendChangesToScale(scale,
  //     ChangeMediatorSingleton::GetInstance());
  //   }
  //
  //   void RegisterSendChangesToScale(Scale *scale, ChangeMediator &mediator) {
  //     mediator.RegisterSendChangesFromTo(this, scale);
  //   }
};

// Recieve changes from mediator
class DSFScaleRepresentativeReciever {
protected:
  void RecieveNoteHasChanged(int scaleNote, double newFrequency) {
    assert(false);
  }

  void RecieveNotesHasChanged(std::vector<int> &scaleNotes,
                              std::vector<double> &newFrequencies) {
    assert(scaleNotes.size() == newFrequencies.size());
    if (scaleNotes.empty()) {
      return;
    }

    assert(false);
  }

  // public:
  //   void RegisterRecieveChangesFromScale(Scale *scale) {
  //     this->RegisterRecieveChangesFromScale(scale,
  //                                           ChangeMediatorSingleton::GetInstance());
  //   }
  //
  //   void RegisterRecieveChangesFromScale(Scale *scale, ChangeMediator &mediator)
  //   {
  //     mediator.RegisterSendChangesFromTo(scale, this);
  //   }
};

// Send changes to and receive changse from mediator
class DSFScaleRepresentativeSenderAndReciever
    : public DSFScaleRepresentativeSender,
      public DSFScaleRepresentativeReciever {};
} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_DYNAMIC_SCALE_FORMATS_DSF_SCALE_REPRESENTATIVE_H_

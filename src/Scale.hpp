#ifndef ANAMARK_TUNING_LIBRARY_SRC_SCALE_H_
#define ANAMARK_TUNING_LIBRARY_SRC_SCALE_H_

#include "AttacherProvider.hpp"

#include <array>
#include <bitset>
#include <cassert>
#include <vector>

namespace AnaMark {

class FormulaSet {};

class Scale : public ChangeAttacher,
              public ChangeProvider,
              public StateAttacher,
              public StateProvider {

public:
  // Range to tune.
  // "If musician wants something to span the compelte range, he has to tune it for
  // the complete range."
  // Optimize for what musicians consider the common case (0-127 scale notes, or
  // whatever they consider).
  static constexpr int firstTunableScaleNote = 0;
  static constexpr int afterLastTunableScaleNote = 128;
  static constexpr int tunableRangeSize =
      afterLastTunableScaleNote - firstTunableScaleNote;

  // Changes to the scale, when should dynamic scale formats that are set to output
  // result in output?

  double FrequencyForMIDINote(int midiNote) {
    if (IsAttachedToAStateProvider()) {
      double frequency = 0.0;
      // @TODO:  Deal with creating API for if note is unavalible to provide
      // frequency state for
      // @TODO:  Deal with creating API for filtered notes
      RequestState(midiNote, frequency);

      assert(midiNote >= firstTunableScaleNote &&
             midiNote < afterLastTunableScaleNote);

      inBoundCache[midiNote] = frequency;
      // @TODO: Deal with updating formula set cache upon change

      return frequency;
    }

    // @TOOD: Create out of bounds logic and caching once formula set logic is
    // completed
    assert(midiNote >= firstTunableScaleNote &&
           midiNote < afterLastTunableScaleNote);

    // @TODO:  Deal with going through MIDI Note to scale note mapping
    return inBoundCache[midiNote];

    // If in 0-127, grab from inBoundsCache.
    // If outside 0-127, check if inBoundsCacheDirty.
    //    If not dirty, grab from outOfBoundsCache.
    //    If dirty, update formulas.
    //        Clear dirty bits.
    //        Clear outOfBoundsCache.
    //    Check outOfBoundsCache for note.
    //        If exists, return.
    //    Else:
    //        If no periodic formulas overlapping scaleNote:
    //            Set outOfBoundsCache at note to 0.0 (no valid frequency).
    //            Return.
    //        If periodic formulas exist, calculate frequency after required repeats
    //        to hit scaleNote.
    //            Set outOfBoundsCache.
    //            Return.

    // Old idea:
    // Query MTS-ESP client API for new frequency if Client MTS-ESP is specified by
    // library user.
    // Client MTS-ESP is master if its enabled.  Formulas are simply for
    // informational and cannot be changed.
    //
    // Update frequency table read frequency to MTS-ESP.
    // Add new formula with highest priority that is frequency of MTS-ESP frequency
    // using frequency shift operand.
    // Add new formula when user queries frequency change with MTS input as well.
    //
    // Simplified formula table is required for calculating outside 0-127 efficently,
    // and to acquire periodicity zones.
    //
    // Simplified table (which requires formula dependency tree of each midi note) is
    // also required for being able to calculate note frequency from formulas at a
    // note-individual level (instead at a scale level).  Required for outside 0-127.
    assert(false);
  }

  // double FrequencyAtScaleNote(int scaleNote) {
  //   // Direct access to scale, untouched by mapping
  //   assert(false);
  // }
  //
  // int GetMIDINoteToScaleNoteMapping(int midiNote) {
  //   assert(false);
  // }

  bool ChangeScaleNoteFrequency(int scaleNote, double newFrequency) {
    // Do not allow users to modify frequencies if scale is already attached to a
    // provider.
    if (!IsAttachedToAProvider()) {
      return true;
    }

    // nullptr as we have no history in this change yet
    ChangeScaleNoteFrequency(nullptr, scaleNote, newFrequency);
    return false;
  }

  bool ChangeScaleNotesFrequencies(std::vector<int> &scaleNotes,
                                   std::vector<double> &newFrequencies) {
    // Do not allow users to modify frequencies if scale is already attached to a
    // provider.
    if (!IsAttachedToAProvider()) {
      return true;
    }

    // nullptr as we have no history in this change yet
    ChangeScaleNotesFrequencies(nullptr, scaleNotes, newFrequencies);
    return false;
  }

  // const FormulaSet &GetFormulas() {
  //   // First updates scale formulas to reflect all previous dynmaic changes made
  //   to the scale.  This is to be as reflective to the current scale state as
  //   possible.  Unfortunately the requirement of being as representative as
  //   possible makes this function slower than others.
  //
  //   // Request all possible notes from StateProvider, if attached to one.
  //
  //   // If inBoundCache is dirty (inBoundCacheDirty) update formulas before sending
  //   // out.
  //   assert(false);
  // }

  // @TODO:  Create additions to ChangeProvider API that provides output only when
  // formulas have (possibly) changed.  It is unknown how much change has been made
  // to the formulas until the API user requests for the formulas (and therefore the
  // formula updates occur).

  // bool IsDirty() {
  //   // Dirty when there has been changes have been made to the scale that are not
  //   // reflected in the formulas.
  //   assert(false);
  // }

  // void UpdateFormulasToReflectChanges() {
  //   // Update Scale formulas to reflect all previous dynamic changes made to
  //   // the scale.
  //
  //   // Go over every individual dirty note (inBoundCachePerNoteDirty) and update
  //   as
  //   // if each dirty note was updated independently.
  //   assert(false);
  // }

  // bool ReplaceFormulaSet(const FormulaSet &newFormulasToUse) {
  //   // Completely overwrites scale with new formula set.
  //   // Returns true (error) if cannot replace (due to being attached to provider).
  //
  //   formulaSet = newFormulasToUse;
  //   // OverwriteCachesWithFormulaFrequencies();
  //   assert(false);
  // }

  // Which should be prioritized, manual formula changes or ChangeScaleNoteFrequency
  // calls?

  // unsigned int GetPeriodicity(int scaleNote) {
  //   // If inBoundCacheDirty, update formulas, clear dirty bits.
  //   // If there are no periodic formulas that overlap scaleNote, return 0 (no
  //   // periodicity)
  //   // If there are, look at formulas for repeat.
  //   assert(false);
  // }

  // StateProvider

  Flags<Capabilities> StateProviderCapabilities() override {
    return {
        Capabilities::ScaleName,
        Capabilities::FilterMIDINotes,
        Capabilities::MIDINoteToScaleNoteMapping,
    };
  }

  void HasBeenRequestedState(int scaleNoteToAcquire,
                             double &scaleNoteFrequencyOutput) override {
    scaleNoteFrequencyOutput = FrequencyForMIDINote(scaleNoteToAcquire);
  }

  // End StateProvider

private:
  void RecieveChangeFromProvider(const ChangeProvider *const changeOrigin,
                                 const ChangeProvider *const notifier, int scaleNote,
                                 double newFrequency) override {
    // Change origin is thrown away in this cycle
    // Cast to void to mitigate "unused parameter" warnings
    (void)changeOrigin;
    // Change origin is now the notifier
    ChangeScaleNoteFrequency(notifier, scaleNote, newFrequency);
  }

  void RecieveChangeFromProvider(const ChangeProvider *const changeOrigin,
                                 const ChangeProvider *const notifier,
                                 std::vector<int> &scaleNotes,
                                 std::vector<double> &newFrequencies) override {
    // Change origin is thrown away in this cycle
    // Cast to void to mitigate "unused parameter" warnings
    (void)changeOrigin;
    // Change origin is now the notifier
    ChangeScaleNotesFrequencies(notifier, scaleNotes, newFrequencies);
  }

  void ChangeScaleNoteFrequency(const ChangeProvider *const changeOrigin,
                                int scaleNote, double newFrequency) {
    // Directly changing one note frequency outside tunable range is ambiguous,
    // therefore disallowed.  Create/modify periodic boundary formulas to modify
    // out-of-bound note frequencies.
    assert(scaleNote >= firstTunableScaleNote &&
           scaleNote < afterLastTunableScaleNote);
    // If want to update notes outside 0-127, one must use ProviderTree for that note
    // to then explicity change that/those formula(s).

    // Updating notes inside 0-127:
    //    Overwrite inBoundCache at scaleNote.
    //    Set scaleNote to be dirty.

    inBoundCache[scaleNote] = newFrequency;
    // @TODO: Deal with formula caching in change

    this->NotifyAttachersOfChange(changeOrigin, scaleNote, newFrequency);
  }

  void ChangeScaleNotesFrequencies(const ChangeProvider *const changeOrigin,
                                   std::vector<int> &scaleNotes,
                                   std::vector<double> &newFrequencies) {
    assert(scaleNotes.size() == newFrequencies.size());

    for (std::size_t i = 0; i < scaleNotes.size(); ++i) {
      inBoundCache[scaleNotes[i]] = newFrequencies[i];
    }

    // @TODO: Deal with formula caching in change

    this->NotifyAttachersOfChange(changeOrigin, scaleNotes, newFrequencies);
  }

  // void OverwriteCachesWithFormulaFrequencies() {
  //   // Clear all dirty bits.
  //   inBoundCacheDirty = false;
  //   inBoundCachePerNoteDirty.reset();
  //   // Clear outOfBoundsCache.
  //   // Recalculate inBoundCache.
  // }

  std::array<double, tunableRangeSize> inBoundCache;
  // std::bitset<tunableRangeSize> inBoundCachePerNoteDirty;
  // outOfBoundsCache;
  // FormulaSet formulaSet;
  // bool inBoundCacheDirty = false;
};
} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_SCALE_H_
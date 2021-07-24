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
  static constexpr int FIRST_TUNABLE_SCALE_NOTE = 0;
  static constexpr int AFTER_LAST_TUNABLE_SCALE_NOTE = 128;
  static constexpr int TUNABLE_RANGE_SIZE =
      AFTER_LAST_TUNABLE_SCALE_NOTE - FIRST_TUNABLE_SCALE_NOTE;

  // Changes to the scale, when should dynamic scale formats that are set to output
  // result in output?

  double FrequencyAtScaleNote(int scaleNote) {
    if (IsAttachedToAStateProvider()) {
      //
    }

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

  bool ChangeScaleNoteFrequency(int scaleNote, double newFrequency) {
    // Do not allow users to modify frequencies if scale is already tracking a
    // Provider
    if (!IsAttachedToAProvider()) {
      return true;
    }

    // nullptr as we have no history in this change yet
    ChangeScaleNoteFrequency(nullptr, scaleNote, newFrequency);
    return false;
  }

  bool ChangeScaleNotesFrequencies(std::vector<int> &scaleNotes,
                                   std::vector<double> &newFrequencies) {
    if (!IsAttachedToAProvider()) {
      return true;
    }

    // nullptr as we have no history in this change yet
    ChangeScaleNotesFrequencies(nullptr, scaleNotes, newFrequencies);
    return false;
  }

  const FormulaSet &GetFormulas() {
    // Provides a "snapshot" of the current FormulaSet in this Scale.
    // Dynamic changes made to this scale after this call are not guarenteed to be
    // reflected instantly.  To have all dynmaic changes be synced with formulas,
    // UpdateFormulas must be called when the most accurate formulas are needed.

    // If inBoundCache is dirty (inBoundCacheDirty) update formulas before sending
    // out.
    assert(false);
  }

  bool IsDirty() {
    // Dirty when there has been changes have been made to the scale that are not
    // reflected in the formulas.
    assert(false);
  }

  void UpdateFormulasToReflectChanges() {
    // Update Scale formulas to reflect all previous dynamic changes made to
    // the scale.

    // Go over every individual dirty note (inBoundCachePerNoteDirty) and update as
    // if each dirty note was updated independently.
    assert(false);
  }

  void OverwriteScale(const FormulaSet &newFormulasToUse) {
    // Completely overwrites scale with new formula set.
    formulaSet = newFormulasToUse;
    OverwriteCachesWithFormulaFrequencies();
  }

  // Which should be prioritized, manual formula changes or ChangeScaleNoteFrequency
  // calls?

  unsigned int GetPeriodicity(int scaleNote) {
    // If inBoundCacheDirty, update formulas, clear dirty bits.
    // If there are no periodic formulas that overlap scaleNote, return 0 (no
    // periodicity)
    // If there are, look at formulas for repeat.
    assert(false);
  }

  void GetNoteState(int scaleNoteToAcquire,
                    double &scaleNoteFrequencyOutput) override {
    scaleNoteFrequencyOutput = FrequencyAtScaleNote(scaleNoteToAcquire);
  }

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
    assert(scaleNote >= FIRST_TUNABLE_SCALE_NOTE &&
           scaleNote < AFTER_LAST_TUNABLE_SCALE_NOTE);
    // If want to update notes outside 0-127, one must use ProviderTree for that note
    // to then explicity change that/those formula(s).

    // Updating notes inside 0-127:
    //    Overwrite inBoundCache at scaleNote.
    //    Set scaleNote to be dirty.

    this->NotifyAttachersOfChange(changeOrigin, scaleNote, newFrequency);
  }

  void ChangeScaleNotesFrequencies(const ChangeProvider *const changeOrigin,
                                   std::vector<int> &scaleNotes,
                                   std::vector<double> &newFrequencies) {
    this->NotifyAttachersOfChange(changeOrigin, scaleNotes, newFrequencies);
  }

  void OverwriteCachesWithFormulaFrequencies() {
    // Clear all dirty bits.
    inBoundCacheDirty = false;
    inBoundCachePerNoteDirty.reset();
    // Clear outOfBoundsCache.
    // Recalculate inBoundCache.
  }

  std::array<double, TUNABLE_RANGE_SIZE> inBoundCache;
  std::bitset<TUNABLE_RANGE_SIZE> inBoundCachePerNoteDirty;
  // outOfBoundsCache;
  FormulaSet formulaSet;
  bool inBoundCacheDirty = false;
};
} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_SCALE_H_

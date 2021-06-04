#ifndef ANAMARK_TUNING_LIBRARY_SRC_SINGLE_SCALE_H_
#define ANAMARK_TUNING_LIBRARY_SRC_SINGLE_SCALE_H_

#include "ProvidesErrorMessage.h"

namespace AnaMark {

class SingleScale : public ProvidesErrorMessage {

  void Import(ScaleFormat &scaleFormatToImport) {
    // Assignment Operator
    *this = scaleFormatToImport.transactionScale;
  }

  void Export(ScaleFormat &scaleFormatToExportTo) {
    // @TODO: Need to reset requested MIDI channels
    scaleFormatToExportTo.transactionScale = *this;
  }

  double FrequencyAtScaleNote(int scaleNote) {
    // Query MTS-ESP client API for new frequency if Client MTS-ESP is specified by
    // library user.
    // Client MTS-ESP is master if its enabled.  Formulas are simply for
    // informational and cannot be changed.

    // Update frequency table read frequency to MTS-ESP.
    // Add new formula with highest priority that is frequency of MTS-ESP frequency
    // using frequency shift operand.
    // Add new formula when user queries frequency change with MTS input as well.

    // Simplified formula table is required for calculating outside 0-127 efficently,
    // and to acquire periodicity zones.

    // Simplified table (which requires formula dependency tree of each midi note) is
    // also required for being able to calculate note frequency from formulas at a
    // note-individual level (instead at a scale level).  Required for outside 0-127.
  }
};
} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_SINGLE_SCALE_H_

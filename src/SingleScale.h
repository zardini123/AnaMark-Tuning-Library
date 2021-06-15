#ifndef ANAMARK_TUNING_LIBRARY_SRC_SINGLE_SCALE_H_
#define ANAMARK_TUNING_LIBRARY_SRC_SINGLE_SCALE_H_

#include <stdexcept>

#include "DynamicScaleFormats/DynamicScaleFormat.h"

namespace AnaMark {

class SingleScale {
public:
  // Templated to get around circular dependency due to ScaleFormat needing
  // SingleScale and SingleScale needing ScaleFormat.
  template <class ScaleFormatType>
  void Import(ScaleFormatType &scaleFormatToImport) {
    // Assignment Operator
    *this = scaleFormatToImport.transactionScale;
  }

  template <class ScaleFormatType>
  void Export(ScaleFormatType &scaleFormatToExportTo) {
    // @TODO: Need to reset requested MIDI channels
    scaleFormatToExportTo.transactionScale = *this;
  }

  // double FrequencyAtScaleNote(int scaleNote) {
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
  // }

  enum class DSFAssignment {
    Input = 0b1,
    Output = 0b10,
    InputAndOutput = 0b11,
  };

  void DetachDSFInstance(DSFAssignment assignmentToDetach) {
    if (assignmentToDetach & DSFAssignment::Input) {
      if (inputDSFInstanceIsOurs) {
        delete inputDSFInstance;
      }
    }
    if (assignmentToDetach & DSFAssignment::Output) {
      if (outputDSFInstanceIsOurs) {
        delete outputDSFInstance;
      }
    }
  }

  void AttachDSFInstance(DynamicScaleFormat *instance, DSFAssignment assignment) {
    // @TODO: Check for Dynamic Scale Format's capabilities and if it can do what the
    // assignment asks for.
    // Throw if compatibility issues.

    // Get rid of the previous instances if we created it.
    DetachDynamicScaleFormat(assignment);

    // Attach
  }

  template <typename DSFType>
  void CreateDSFInstanceAndAttach(DSFType format, DSFAssignment assignment) {
    try {
      newInstance = new DynamicScaleFormatType();
      AttachDSFInstance(newInstance, assignment);
    } catch (std::exception &exception) {
      // Clean up to make sure no leaked memory.
      delete newInstance;
      // Re-throw
      throw exception;
    }

    if (assignment & DSFAssignment::Input) {
      inputDSFInstanceIsOurs = true;
    }
    if (assignment & DSFAssignment::Output) {
      outputDSFInstanceIsOurs = true;
    }
  }

  ~SingleScale() {
    DetachDSFInstance(DSFAssignment::InputAndOutput);
  }

private:
  DynamicScaleFormat *inputDSFInstance = nullptr;
  DynamicScaleFormat *outputDSFInstance = nullptr;
  bool inputDSFInstanceIsOurs : 1;
  bool outputDSFInstanceIsOurs : 1;
};
} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_SINGLE_SCALE_H_

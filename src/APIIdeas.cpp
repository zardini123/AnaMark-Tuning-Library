#include "ScaleCollectionFormats/ScaleCollectionFormat.h"
#include "ScaleFormats/TUN.h"
#include "SingleScale.h"

using namespace AnaMark;

int main(int argc, char const *argv[]) {

  /////////////////////////
  // Scale Collection

  // {
  //   ScaleCollection collection;
  //
  //   ScaleFormats::TUN scaleImport;
  //   scaleImport.Read("File");
  //   // User if statement for invalid file, etc.
  //   // User should not import if fail
  //
  //   // Defaults to all channels
  //   collection.Import(scaleImport);
  //
  //   // Import to channel 0
  //   collection.Import(scaleImport, 0);
  // }
  //
  // {
  //   ScaleCollection collection;
  //
  //   ScaleCollectionFormats::MSF msfImport;
  //   msfImport.Read("File");
  //   // Check if valid read
  //   // Can probably extract the single scales at this point
  //
  //   // Scale collection import
  //   // What is a scale collection?
  //   collection.Import(msfImport);
  // }

  /////////////////////////
  // Single Scale

  // Cannot allow this as scale needs to be around for all of the synthesizer, even
  // for equal tempermeant. This will also result in KBM being ignored or reading a
  // kbm.
  // { SingleScale scale = ScaleFormats::TUN::Read("File"); }
  // { SingleScale scale(ScaleFormats::TUN::Read("File")); }

  {
    SingleScale scale; // been used, initalized

    ScaleFormats::TUN scaleImport;
    scaleImport.Read("File");
    // User if statement for invalid file, etc.
    // User should not import if fail

    scale.Import(scaleImport);
    // KBM can effect the scale even when equal tempermant.
    // Is there any possible errors to catch??
  }

  {                    // Filename argument constructor for ScaleFormat
    SingleScale scale; // been used, initalized

    ScaleFormats::TUN scaleImport("File");
    // User if statement for invalid file, etc.

    scale.Import(scaleImport);
  }

  {
    SingleScale scale; // been used, initalized

    ScaleFormats::TUN export;
    scale.Export(export);
    // Check for compatibility mismatch, therefore information loss
    // User should reject if undesired
    // USERS MUST CHECK THIS
    export.Write("File");
  }

  {
    SingleScale scale; // been used, initalized

    ScaleFormats::TUN export(scale);
    // Check for compatibility mismatch
    export.Write("File");
  }

  // Scale conversion
  {
    // Should this be allowed?
    ScaleFormats::TUN tun;
    tun.Read("File");

    ScaleFormats::SCL::Write(tun);
  }

  // Full read/write
  {
    SingleScale scale; // been used, initalized

    ScaleFormats::KBM kbmImport;
    kbmImport.Read("File");
    // User if statement for invalid file, etc.
    // User should not import if fail

    scale.Import(kbmImport);

    ScaleFormats::SCL sclExport;

    if (scale.CheckIncompatibilityOfExport(sclExport)) {
      ScaleFormat::Requirements scaleMissing = sclExport.RequirementsNotInScale();

      if (scaleMissing.scaleInformation) {
        // Show error to user that scale does not contain scale information and
        // therefore cannot export
      } else if (scaleMissing.keyboardMapping) {
        // Show error to user that scale does not contain keyboard mapping and
        // therefore cannot export
      }
    } else {
      if (scale.CheckLossesOfExport(sclExport)) {
        ScaleFormat::Compaibilies losses =
            sclExport.CompatabilitiesNotAvalibleInFormat();

        if (losses.scaleInformation && sclExport.Requirements().keyboardMapping) {
          // Show error to user that a keyboard mapping format is being written to
          // when the scale has scale informtaion, so make sure the user knows to
          // also export a scale type if wanted.
        } else {
          // Show error that some loss has occured, and can list out possibily as
          // bullet points using the losses object.
        }
      }

      // Will not throw exceptions if scale export has compatibility losses, but will
      // throw exceptions if there are requirement incompatibilities with the scale
      scale.Export(sclExport);
    }

    try {

    } catch (RequirementsNotPresentException &requirementsNotPresent) {

      if (requirementsNotPresent.scaleInformation) {
        // Show error to user that scale does not contain scale information and
        // therefore cannot export
      } else if (requirementsNotPresent.keyboardMapping) {
        // Show error to user that scale does not contain keyboard mapping and
        // therefore cannot export
      }
    }

    sclExport.Write("File");
  }

  // Any format
  {
    SingleScale scale; // been used, initalized

    ScaleFormats::AnyFormat anyImport("File");
    scale.Import(anyImport);
  }

  // Dynamic scale
  {
    SingleScale scale; // been used, initalized

    // Called from the plugin constructor
    MTS_ESP mtsESPManager();

    // mtsESPManager is desturcted in plugin destructor
  }

  return 0;
}

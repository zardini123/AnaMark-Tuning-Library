#include "AnaMark.hpp"

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
  // { Scale scale = ScaleFormats::TUN::Read("File"); }
  // { Scale scale(ScaleFormats::TUN::Read("File")); }

  {
    Scale scale; // been used, initalized

    ScaleFormats::TUN scaleImport;
    scaleImport.Read("File");
    // Will throw exception if file does not exist, and if there is a parsing error
    // in the file itself.

    scale.Import(scaleImport);
    // KBM can effect the scale even when equal tempermant.
    // Is there any possible errors to catch??
  }

  // {                    // Filename argument constructor for ScaleFormat
  //   Scale scale; // been used, initalized
  //
  //   ScaleFormats::TUN scaleImport("File");
  //   // User if statement for invalid file, etc.
  //
  //   scale.Import(scaleImport);
  // }

  {
    Scale scale; // been used, initalized

    ScaleFormats::TUN scaleExport;
    scale.Export(scaleExport);
    // Check for compatibility mismatch, therefore information loss
    // User should reject if undesired
    // USERS MUST CHECK THIS
    scaleExport.Write("File");
  }

  // {
  //   Scale scale; // been used, initalized
  //
  //   ScaleFormats::TUN scaleExport(scale);
  //   // Check for compatibility mismatch
  //   scaleExport.Write("File");
  // }

  // Scale conversion
  // {
  //   // Should this be allowed?
  //   ScaleFormats::TUN tun;
  //   tun.Read("File");
  //
  //   ScaleFormats::SCL::Write(tun);
  // }

  // Export has no information to work off of.  I.e. export KBM to SCL
  {
    Scale scale; // been used, initalized

    ScaleFormats::KBM kbmImport;
    kbmImport.Read("File");
    // Will throw exception if file does not exist, and if there is a parsing error
    // in the file itself.

    // Scale only contains keyboard mapping information, no scale information
    // currently.
    scale.Import(kbmImport);

    ScaleFormats::SCL sclExport;

    if (scale.CheckIfMissingRequirementsForExport(sclExport)) {
      ScaleFormat::Requirements informationMissing =
          sclExport.RequirementsNotInScale();

      if (informationMissing.scaleInformation) {
        // Show error to user that scale does not contain scale information and
        // therefore cannot export
      } else if (informationMissing.keyboardMapping) {
        // Show error to user that scale does not contain keyboard mapping and
        // therefore cannot export
      }
    } else {
      if (scale.CheckForLossesInExport(sclExport)) {
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

    sclExport.Write("File");
  }

  // Exception based export fail
  {
    Scale scale; // been used, initalized

    ScaleFormats::KBM kbmImport;
    kbmImport.Read("File");
    // Will throw exception if file does not exist, and if there is a parsing error
    // in the file itself.

    // Scale only contains keyboard mapping information, no scale information
    // currently.
    scale.Import(kbmImport);

    ScaleFormats::SCL sclExport;

    try {
      scale.Export(sclExport);
    } catch (MissingRequirementsException &requirementsNotPresent) {

      if (requirementsNotPresent.scaleInformation) {
        // Show error to user that scale does not contain scale information and
        // therefore cannot export
      } else if (requirementsNotPresent.keyboardMapping) {
        // Show error to user that scale does not contain keyboard mapping and
        // therefore cannot export
      }
    }
  }

  // Any format
  {
    Scale scale; // been used, initalized

    ScaleFormats::AnyFormat anyImport("File");
    scale.Import(anyImport);
  }

  // Dynamic scale
  {
    Scale scale; // been used, initalized

    // Called from the plugin constructor
    MTS_ESP mtsESPManager();

    // mtsESPManager is desturcted in plugin destructor
  }

  return 0;
}

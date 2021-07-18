// TUN_Error.h: Interface of class Error.
//
// (C)opyright in 2009 by Mark Henning, Germany
//
// Contact: See contact form at www.mark-henning.de
//
// This class provides basic error handling.
//
//////////////////////////////////////////////////////////////////////

#ifndef ANAMARK_TUNING_LIBRARY_SRC_ERROR_H_
#define ANAMARK_TUNING_LIBRARY_SRC_ERROR_H_

#include <string>

namespace AnaMark {

using ErrorValue = bool;

class Error {
private:
  std::string errorMessage;

public:
  Error() {
    SetOK();
  }

  virtual ~Error() = default;

  /**
   * @return Returns 0 if this Error instance is a non-error (i.e. is "Ok").  1
   * otherwise.
   */
  bool IsOK() const {
    return !errorMessage.empty();
  }

  /**
   * Returns the error message.
   * @return The error message.
   */
  const std::string &GetMessage() const {
    return errorMessage;
  }

  /**
   * @breif Set the error message.
   * @param  newErrorMessage Message to print when recalling error.
   * @param  lineNumber      Optional line number to include in message.
   * @return                 If @p newErrorMessage is nullptr, it clears the error
   * message and returns 0.  If it is not nullptr, the error message is set and
   * function returns 1.
   */
  bool SetError(const char *newErrorMessage, long lineNumber = -1) {
    if (newErrorMessage == nullptr)
      errorMessage = "";
    else {
      //              012345
      // Unused: char	sz[16] = "Line 1234567890"; // Enough for 32-Bit
      if (lineNumber >= 0)
        // CHANGED:  Removed ltoa usage
        // Original usage:  ... ltoa(lLineNr, &sz[5], 10) ...
        errorMessage = "Line " + std::to_string(lineNumber) + ": " + newErrorMessage;
      else
        errorMessage = newErrorMessage;
    }

    return IsOK();
  }

  /**
   * Overwrite this error by some other Error instance.
   * @param  errorToBeOverwrittenBy Error to be overwritten by.
   * @return     										Return 0 if @p err is ok (calls
   * IsOK()), 1 otherwise.
   */
  bool SetError(const Error &errorToBeOverwrittenBy) {
    *this = errorToBeOverwrittenBy;
    return IsOK();
  }

  /**
   * Set the error to be a non-error.
   * @return Returns 0.
   */
  bool SetOK() {
    return SetError(nullptr);
  }
};

} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_ERROR_H_

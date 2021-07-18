#ifndef ANAMARK_TUNING_LIBRARY_SRC_PROVIDES_ERROR_MESSAGE_TUN_H_
#define ANAMARK_TUNING_LIBRARY_SRC_PROVIDES_ERROR_MESSAGE_TUN_H_

#include "TUN_Error.hpp"

namespace AnaMark {

class ProvidesErrorMessage {
protected:
  Error err;

public:
  const Error &GetError() const {
    return err;
  }
};

} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_PROVIDES_ERROR_MESSAGE_TUN_H_

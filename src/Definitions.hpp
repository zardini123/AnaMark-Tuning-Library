#ifndef ANAMARK_TUNING_LIBRARY_SRC_DEFINITIONS_H_
#define ANAMARK_TUNING_LIBRARY_SRC_DEFINITIONS_H_

namespace AnaMark {

template <typename FlagsType,
          typename UnderlyingType = typename std::underlying_type<FlagsType>::type>
class Flags {
public:
  template <typename... TFlagsPack>
  Flags(TFlagsPack... initalFlags) {
    SetFlags(initalFlags...);
  }

  // Required termination version of SetFlags, as the last call of parameter pack
  // will have no arguments
  void SetFlags() {}

  template <typename... TFlagsPack>
  void SetFlags(FlagsType firstFlags, TFlagsPack... flags) {
    value |= static_cast<UnderlyingType>(firstFlags);
    SetFlags(flags...);
  }

  constexpr bool AreFlagsSet(FlagsType flags) const {
    return (value & static_cast<UnderlyingType>(flags)) ==
           static_cast<UnderlyingType>(flags);
  }

private:
  UnderlyingType value = UnderlyingType();
};

} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_DEFINITIONS_H_

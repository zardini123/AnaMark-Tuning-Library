#ifndef ANAMARK_TUNING_LIBRARY_SRC_CHANGE_MEDIATOR_H_
#define ANAMARK_TUNING_LIBRARY_SRC_CHANGE_MEDIATOR_H_

#include "DynamicScaleFormats/DSFScaleRepresentative.hpp"
#include "Scale.hpp"

#include <cassert>
#include <set>
#include <unordered_map>
#include <vector>

namespace AnaMark {

// Forward declare classes to mitigate non-declared class errors
// class Scale;
// class DSFScaleRepresentativeSender;
// class DSFScaleRepresentativeReciever;

class ChangeMediator {
  friend DSFScaleRepresentativeSender;
  friend Scale;

  // Publisher-subscriber pattern

public:
  void RegisterSendChangesFromTo(DSFScaleRepresentativeSender *scaleRepresentative,
                                 Scale *scale) {
    // Send DSFScaleRepresentativeSender changes to scales
    ThrowIfInVectorElseAdd(dsfScaleRepChangeMappings[scaleRepresentative], scale);
  }

  void RegisterSendChangesFromTo(
      Scale *scale, DSFScaleRepresentativeReciever *scaleRepresentative) {
    // Fail if DSFScaleRepresentativeReciever has already been used
    if (usedDsfScaleRepRecievers.count(scaleRepresentative) == 1) {
      // @TODO: Throw exception stating scaleRepresentative is already being sent to
      // by a scale
      assert(false);
    } else {
      // Add to make sure its not used again
      usedDsfScaleRepRecievers.insert(scaleRepresentative);
    }

    // Send Scale changes to DSFScaleRepresentativeRecievers
    // Creates new vector if not avalible, grabs existing if there is one already.
    ThrowIfInVectorElseAdd(scaleChangeMappings[scale], scaleRepresentative);
  }

  void DeregisterSendChangesFromTo(DSFScaleRepresentativeSender *scaleRepresentative,
                                   Scale *scale) {
    Deregister(subscriberVectorIt->second, scale);
  }

  void DeregisterSendChangesFromTo(
      Scale *scale, DSFScaleRepresentativeReciever *scaleRepresentative) {
    Deregister(subscriberVectorIt->second, scaleRepresentative);
  }

private:
  template <typename InputIterator, typename T>
  InputIterator find(InputIterator first, InputIterator last, const T &val) {
    while (first != last) {
      if (*first == val) {
        return first;
      }

      ++first;
    }
    return last;
  }

  template <typename VectorType, typename VectorElementType>
  void ThrowIfInVectorElseAdd(VectorType &subscriberVector,
                              VectorElementType &vectorElementToAdd) {
    // Check if representative is already registered
    auto existingRepIt =
        find(subscriberVector.begin(), subscriberVector.end(), vectorElementToAdd);

    if (existingRepIt != subscriberVector.end()) {
      // @TODO: Throw exception stating scale rep already exists.
      assert(false);
    }

    subscriberVector.push_back(vectorElementToAdd);
  }

  template <typename MapType, typename KeyType, typename VectorType,
            typename VectorElementType>
  void Deregister(MapType &map, KeyType &key, VectorType &subscriberVector,
                  VectorElementType &vectorElementToRemove) {

    // Send Scale changes to DSFScaleRepresentativeReciever
    auto mapIt = map.find(key);
    if (mapIt == map.end()) {
      // @TODO: Throw an exception stating no registered scale representatives from
      // scale
      assert(false);
    }

    auto &subscriberVector = mapIt->second;

    if (subscriberVector.size() == 0) {
      // There should be a element in the vector if there is a vector present at this
      // key.  Therefore this is a AnaMark Tuning Library developer error!
      assert(false);
    }

    // Find existing element to remove
    auto existingIt = find(
        subscriberVector.begin(), subscriberVector.end(), vectorElementToRemove);

    if (existingIt == subscriberVector.end()) {
      // @TODO: Throw exception stating no scale exists for this scaleRepresentative
      assert(false);
    }

    subscriberVector.erase(existingIt);

    if (subscriberVector.size() == 0) {
      // Delete the vector entry from map to make space as there are no elements
      map.erase(mapIt);
    }
  }

  // Called by a DSFScaleRepresentativeSender when a note is changed
  void DSFScaleRepHasChanged(DSFScaleRepresentativeSender *subject, int scaleNote,
                             double newFrequency) {
    // ** DSFScaleRepresentativeSender sends to many scales
    // Scale can only recieve from 1 DSFScaleRepresentativeSender (can only represent
    // one scale)

    auto mapIt = dsfScaleRepChangeMappings.at(subject);
    auto &subscriberVector = mapIt->second;

    for (Scale &subscribers : subscriberVector) {
      scale.ChangeScaleNoteFrequency(this, subject, scaleNote, newFrequency);
    }
  }

  void DSFScaleRepHasChanged(DSFScaleRepresentativeSender *subject,
                             std::vector<int> &scaleNotes,
                             std::vector<double> &newFrequencies) {
    assert(scaleNotes.size() == newFrequencies.size());
  }

  // Called by a Scale for when a note is changed
  void ScaleHasChanged(Scale *subject, DSFScaleRepresentativeSender *changer,
                       int scaleNote, double newFrequency) {
    // For all attached Scales that DSFScaleRepresentative recieves from
    // ** Scale sends to many DSFScaleRepresentativesRecievers
    // DSFScaleRepresentativeReciever can only receive from 1 scale (can only
    // represent one scale)

    auto mapIt = scaleChangeMappings.at(subject);
    auto &subscriberVector = mapIt->second;

    for (DSFScaleRepresentativeReciever &subscriber : subscriberVector) {
      if (&subscriber == changer) {
        // Prevent loopback
        continue;
      }

      subscriber.RecieveNoteHasChanged(scaleNote, newFrequency);
    }
  }

  void ScaleHasChanged(Scale *scale, DSFScaleRepresentativeSender *changer,
                       std::vector<int> &scaleNotes,
                       std::vector<double> &newFrequencies) {
    assert(scaleNotes.size() == newFrequencies.size());
  }

  std::unordered_map<DSFScaleRepresentativeSender *, std::vector<Scale *>>
      dsfScaleRepChangeMappings;
  std::unordered_map<Scale *, std::vector<DSFScaleRepresentativeReciever *>>
      scaleChangeMappings;

  std::set<DSFScaleRepresentativeReciever *> usedDsfScaleRepRecievers;
};

class ChangeMediatorSingleton {
public:
  static ChangeMediator &GetInstance() {
    // Guaranteed to be destroyed.  Instantiated on first use.
    static ChangeMediator instance;

    return instance;
  }
  // Delete to make sure accidents of getting copies of the singleton do not happen.
  ChangeMediatorSingleton(ChangeMediatorSingleton const &) = delete;
  void operator=(ChangeMediatorSingleton const &) = delete;
};

} // namespace AnaMark

#endif // ANAMARK_TUNING_LIBRARY_SRC_CHANGE_MEDIATOR_H_

#pragma once

#include <zmq.hpp>
#include <msgpack.hpp>

class Blackboard {
  public:
    virtual ~Blackboard() = default;
};

// Mixin for serialization
// TODO: AircraftHandle is on blackboard, and I don't want to
// have to figure out how to serialize that...
template <typename DerivedBlackboardType> 
class SerializableMixin {
  public:
    // Serialization
    msgpack::sbuffer pack() const {
      msgpack::sbuffer sbuf;
      msgpack::pack(sbuf, *static_cast<DerivedBlackboardType*>(this));
      return sbuf;
    }

    // Deserialization
    void unpack(const char* data, const size_t size) const {
      msgpack::object_handle oh = msgpack::unpack(data, size);
      oh.get().convert(*static_cast<DerivedBlackboardType*>(this));
    }
};

class SendableBlackboard : SerializableMixin<Blackboard> {
  public:
    virtual ~SendableBlackboard() = default;
};

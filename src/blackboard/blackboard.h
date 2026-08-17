#pragma once

#include <zmq.hpp>
#include <msgpack.hpp>

class Blackboard {
  public:
    virtual ~Blackboard() = default;
};

// Mixin for serialization
template <typename DerivedBlackboardType> 
class SerializableMixin {
  public:
    // Serialization
    msgpack::sbuffer pack() const {
      msgpack::sbuffer sbuf;
      msgpack::pack(sbuf, *static_cast<const DerivedBlackboardType*>(this));
      return sbuf;
    }

    // Deserialization
    void unpack(const char* data, const size_t size) {
      msgpack::object_handle oh = msgpack::unpack(data, size);
      oh.get().convert(*static_cast<DerivedBlackboardType*>(this));
    }
};

class SendableBlackboard : SerializableMixin<Blackboard> {
  public:
    virtual ~SendableBlackboard() = default;
    virtual msgpack::sbuffer pack() const = 0;
    virtual void unpack(const char* data, size_t size) = 0;
};

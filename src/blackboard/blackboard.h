#pragma once

#include <zmq.hpp>
#include <msgpack.hpp>

class Blackboard {
  public:
    virtual ~Blackboard() = default;
};

class SerializableBlackboard : Blackboard {
  public:
    virtual ~SerializableBlackboard() = default;
    virtual msgpack::sbuffer pack() const = 0;
    virtual void unpack(const char* data, size_t size) = 0;
};

// Mixin for serialization
template <typename DerivedBlackboardType> 
class SerializeMixin : SerializableBlackboard {
  public:
    // Serialization
    msgpack::sbuffer pack() const override {
      msgpack::sbuffer sbuf;
      msgpack::pack(sbuf, *static_cast<const DerivedBlackboardType*>(this));
      return sbuf;
    }

    // Deserialization
    void unpack(const char* data, const size_t size) override {
      msgpack::object_handle oh = msgpack::unpack(data, size);
      oh.get().convert(*static_cast<DerivedBlackboardType*>(this));
    }
};

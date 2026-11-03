#pragma once
#include "Arduino.h"
#include <cassert>
#include <thread>

inline std::thread::id homeKitThread;
inline int characteristicWrites = 0;

struct SpanCharacteristic {
  explicit SpanCharacteristic(int initial = 0) : value(initial) {}
  void setVal(int v) {
    assert(std::this_thread::get_id() == homeKitThread);
    ++characteristicWrites;
    value = v;
  }
  bool updated() const { return written; }
  int getNewVal() const { return value; }
  void simulateWrite(int v) { value = v; written = true; }
  int value;
  bool written = false;
};
struct SpanService {
  virtual ~SpanService() = default;
  virtual boolean update() { return true; }
  SpanService *addLink(SpanService *) { return this; }
};
namespace Service {
struct Television : SpanService {};
struct TelevisionSpeaker : SpanService {};
struct InputSource : SpanService {};
}
namespace Characteristic {
struct Active : SpanCharacteristic { using SpanCharacteristic::SpanCharacteristic; };
struct RemoteKey : SpanCharacteristic {};
struct ConfiguredName : SpanCharacteristic { explicit ConfiguredName(const char *) {} };
struct VolumeControlType : SpanCharacteristic { using SpanCharacteristic::SpanCharacteristic; };
struct VolumeSelector : SpanCharacteristic {};
struct Mute : SpanCharacteristic { using SpanCharacteristic::SpanCharacteristic; };
struct Identifier : SpanCharacteristic { using SpanCharacteristic::SpanCharacteristic; };
struct IsConfigured : SpanCharacteristic { using SpanCharacteristic::SpanCharacteristic; };
struct CurrentVisibilityState : SpanCharacteristic { using SpanCharacteristic::SpanCharacteristic; };
}
namespace Category { constexpr int Television = 1; }
struct TestHomeSpan {
  void setLogLevel(int) {}
  void setConnectionTimes(int, int, int) {}
  void enableAutoStartAP() {}
  void begin(int, const char *) {}
  void poll() { assert(std::this_thread::get_id() == homeKitThread); }
};
inline TestHomeSpan homeSpan;
#define SPAN_ACCESSORY() do {} while (false)

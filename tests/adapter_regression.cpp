// Compile the actual adapters against small host fakes. These tests exercise
// observable task ownership, coalesced state delivery and publish failures.
#include "../relay_bridge/HomeKitAdapter.cpp"
#include "../relay_bridge/MqttAdapter.cpp"
#include <iostream>
#include <vector>

namespace HonorBle {
static TvStateCallback stateCallback;
static bool tvConnected = false;
static std::vector<uint8_t> keys;
void setTvStateCallback(TvStateCallback callback) {
  stateCallback = callback;
  callback(tvConnected);
}
bool isTvConnected() { return tvConnected; }
void setPower(bool) {}
void sendKey(uint8_t key) { keys.push_back(key); }
}

int main() {
  homeKitThread = std::this_thread::get_id();
  HomeKitAdapter::begin();
  // Initial state is delivered by the loop, not by registration/callback.
  assert(characteristicWrites == 0);
  HomeKitAdapter::loop();
  assert(characteristicWrites == 1);

  std::thread ble([] {
    for (int i = 0; i < 10000; ++i) HonorBle::stateCallback(i % 2 == 0);
    HonorBle::stateCallback(true);
  });
  ble.join();
  assert(characteristicWrites == 1);
  HomeKitAdapter::loop();
  assert(characteristicWrites == 2);
  assert(HomeKitAdapter::activeCharacteristic->value == 1);
  HomeKitAdapter::loop();
  assert(characteristicWrites == 2);

  // Preserve the user's mute-toggle remote behavior for both UI writes.
  HomeKitAdapter::HonorSpeaker speaker;
  speaker.mute->simulateWrite(1);
  assert(speaker.update());
  speaker.mute->simulateWrite(0);
  assert(speaker.update());
  assert((HonorBle::keys == std::vector<uint8_t>{HonorBle::MUTE, HonorBle::MUTE}));

  auto& mqtt = MqttAdapter::mqtt;
  mqtt.publishSucceeds = false;
  MqttAdapter::publishState();
  assert(mqtt.attempts.size() == 1);
  // A failed publish is retried without requiring a TV state change.
  mqtt.publishSucceeds = true;
  MqttAdapter::publishState();
  assert(mqtt.attempts.size() == 2);
  assert(mqtt.attempts.back().second == "OFF");
  MqttAdapter::publishState();
  assert(mqtt.attempts.size() == 2);

  HonorBle::tvConnected = true;
  mqtt.publishSucceeds = false;
  MqttAdapter::publishState();
  mqtt.publishSucceeds = true;
  MqttAdapter::publishState();
  assert(mqtt.attempts.size() == 4);
  assert(mqtt.attempts.back().second == "ON");

  // Reconnect's forced publish can fail even when cached state is unchanged.
  mqtt.publishSucceeds = false;
  MqttAdapter::publishState(true);
  mqtt.publishSucceeds = true;
  MqttAdapter::publishState();
  assert(mqtt.attempts.size() == 6);
  MqttAdapter::publishState();
  assert(mqtt.attempts.size() == 6);
  std::cout << "Adapter regressions passed\n";
}

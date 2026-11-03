#pragma once
#include "WiFi.h"
#include <utility>
#include <vector>

class PubSubClient {
 public:
  explicit PubSubClient(WiFiClient&) {}
  bool connected() const { return online; }
  bool publish(const char *topic, const char *payload, bool) {
    attempts.emplace_back(topic, payload);
    return online && publishSucceeds;
  }
  template<class... Args> bool connect(Args...) { online = true; return true; }
  int state() { return 0; }
  void subscribe(const char *) {}
  void setServer(const char *, int) {}
  template<class T> void setCallback(T) {}
  void setBufferSize(int) {}
  void loop() {}
  bool online = true;
  bool publishSucceeds = true;
  std::vector<std::pair<std::string, std::string>> attempts;
};

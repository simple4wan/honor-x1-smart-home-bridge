#pragma once
#include "Arduino.h"
constexpr int WL_CONNECTED = 3;
struct WiFiClient {};
struct TestWiFi { int status() { return WL_CONNECTED; } };
inline TestWiFi WiFi;

#pragma once
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

using boolean = bool;
using byte = unsigned char;
constexpr int HEX = 16;

class String {
 public:
  String(const char *s = "") : value(s) {}
  String(uint32_t n, int) : value(std::to_string(n)) {}
  void reserve(size_t n) { value.reserve(n); }
  void trim() {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) { value.clear(); return; }
    value = value.substr(first, value.find_last_not_of(" \t\r\n") - first + 1);
  }
  void toUpperCase() {
    for (char& c : value) c = char(std::toupper(static_cast<unsigned char>(c)));
  }
  const char *c_str() const { return value.c_str(); }
  String& operator+=(const char *s) { value += s; return *this; }
  String& operator+=(const String& s) { value += s.value; return *this; }
  String& operator+=(char c) { value += c; return *this; }
  bool operator==(const String& other) const { return value == other.value; }
 private:
  std::string value;
};

struct TestSerial {
  void println(const char *) {}
  template<class... Args> void printf(const char *, Args...) {}
};
inline TestSerial Serial;
inline uint32_t millis() { return 1000; }
struct TestEsp { uint64_t getEfuseMac() { return 123; } };
inline TestEsp ESP;

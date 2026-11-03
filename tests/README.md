# Adapter regression tests

Run from the repository root (no ESP32 required):

```sh
c++ -std=c++17 -Wall -Wextra -Werror -pthread -Itests/stubs tests/adapter_regression.cpp -o /tmp/adapter-regression
/tmp/adapter-regression
```

The tests compile the production HomeKit/MQTT adapters with host fakes and
check that BLE state callbacks never mutate HomeSpan characteristics, the
latest state reaches the loop, mute remains a toggle, and failed state
publishes (including forced reconnect publishes) retry without a state change.
They do not simulate NimBLE, radio timing or NVS persistence.

Hardware checks after flashing the update firmware:

- Power off the configured remote during reconnection; confirm HomeKit/MQTT
  continue responding while connection/encryption/discovery times out.
- Advertise another connectable HDRC-BV1 at a different known address; confirm
  it is ignored and its address is absent from the bond list.
- Confirm the configured remote pairs, relays Report 1, reconnects after a
  disconnect and restores its bond after an ESP32 restart.
- Turn the TV on/off repeatedly while using HomeKit; check Active converges to
  BLE connection state and mute toggles twice back to its original state.

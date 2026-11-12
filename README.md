# A.T.O.M. — Access, Tracking & Operation Monitoring

ESP32-S3 biometric access-control and operator-monitoring prototype developed for a Komatsu PC200 application.

> **Safety notice:** This is development firmware for prototyping and bench testing. It is not a certified machine safety or anti-theft system and has not been validated for installation into OEM excavator wiring. Do not bypass OEM interlocks. Validate relay isolation, fail-safe behavior, power transients, environmental durability, and machine-specific integration before field use.

## Branch snapshot

- Branch: `v1.7.4`
- Firmware sketch: `ATOMIC_FIRMWARE_V1_7_4.ino`
- Development focus: **A.T.O.M. rebranding**
- Prototype Device ID: `ATOMIC-001`

Introduces A.T.O.M. branding in place of PC200 ACCESS and updates associated bitmap/icon assets. Inspect this historical snapshot before using it as a build reference.

This document describes the project context and the focus of this branch. Features introduced in later branches are not necessarily present here. Inspect the actual sketch before compiling or integrating.

## Hardware platform

- ESP32-S3 DevKitC-1 N16R8 (16 MB flash, 8 MB OPI PSRAM)
- SSD1306 128×64 OLED over I²C
- DS3231 real-time clock
- FPM10A / DY50-compatible fingerprint sensor (UART 57600 in later firmware)
- Onboard RGB NeoPixel, four buttons, buzzer, relay driver, and ignition-sense input

V1.9.5 GPIO reference (older versions may differ): buttons GPIO 4–7; I²C SDA/SCL GPIO 8/9; relay GPIO 10; ignition sense GPIO 11; buzzer GPIO 12; fingerprint UART TX/RX GPIO 17/18; RGB LED GPIO 48.

## Project capabilities

Capabilities added progressively across the repository include fingerprint enrollment and matching, operator-name association, relay control, OLED menus, ESP32 SoftAP Admin Mode, embedded Web Admin, device identity, Wi-Fi Station configuration, MQTT heartbeat/access/session telemetry, RTC/NTP timekeeping, and hybrid MQTT transport.

## Development history

| Version | Milestone |
|---|---|
| V1.6 | Admin Mode foundation and core fingerprint workflow |
| V1.7.1–V1.7.3 | Admin Mode and enrollment-feedback refinements |
| V1.7.4 | A.T.O.M. rebranding and bitmap/icon updates |
| V1.7.5 | OLED INFORMATION screen and firmware identity |
| V1.8.0 | Device ID foundation |
| V1.8.1–V1.8.4 | Fingerprint slot validation, captive portal, and embedded JavaScript compatibility fixes |
| V1.8.5 | Earlier stable benchmark for operator administration and device UI |
| V1.9.0 | Wi-Fi Station configuration |
| V1.9.1 | Initial MQTT telemetry using PubSubClient |
| V1.9.2–V1.9.3 | Intermediate integration refinements |
| V1.9.4 | NTP synchronization and DS3231 timekeeping |
| V1.9.5 Hybrid MQTT | ESP-MQTT LAN TCP / Internet WSS; user-confirmed dashboard telemetry |
| V1.9.5 All State | Separate feature branch for fingerprint grants with ignition ON or OFF; hardware validation pending |

## Later V1.9.5 reference architecture

The project context identifies `v1.9.5-hybrid-mqtt` as the user-confirmed benchmark. It uses LAN MQTT at `mqtt://192.168.0.128:1883` and secure WebSocket MQTT at `wss://mqtt.endyarfian.web.id:443/mqtt`. Device topics use the `atomic/devices/ATOMIC-001/` namespace for heartbeat, events, sessions, and operators. The user confirmed telemetry reached the dashboard over WSS from a different Wi-Fi network. This confirmation applies to the tested V1.9.5 Hybrid MQTT branch, not automatically to this historical branch.

The separate `v1.9.5-all-state` branch changes successful fingerprint authentication so that a matching fingerprint grants access in both ignition states. Its physical compile/upload and ignition-OFF testing remain to be confirmed.

## Build and upload

1. Check out this branch and open its matching `.ino` file in Arduino IDE.
2. Select an ESP32-S3 board profile appropriate for N16R8; configure 16 MB flash and OPI PSRAM where supported.
3. Install dependencies used by this sketch. Common libraries include Adafruit GFX, Adafruit SSD1306, RTClib, Adafruit Fingerprint Sensor Library, and Adafruit NeoPixel. MQTT dependencies vary by revision.
4. Run Verify/Compile before upload.
5. Upload to the ESP32-S3 and inspect Serial Monitor at 115200 baud.
6. Test changes on a safe bench setup before any machine integration.

Later development used Arduino-ESP32 Core 3.3.0 and ESP-IDF 5.5 through the Arduino core.

## Validation and security

A source file or Git commit does not prove compilation or hardware operation. Record the exact branch, commit SHA, board configuration, and test conditions for each validation. User-confirmed milestones include V1.6 core/Admin Mode operation, V1.7.1 normal operation, and V1.9.5 Hybrid MQTT telemetry over WSS.

Never commit Wi-Fi passwords, MQTT passwords, API tokens, or private keys. Replace prototype SoftAP/Web Admin credentials before broader deployment, use unique device identities and MQTT credentials, enforce broker ACLs, and do not expose embedded Web Admin directly to the public Internet. Review relay default state, reset/brownout behavior, flash protection, and OEM safety requirements.

## Repository workflow

Keep known-good baselines unchanged. Develop new features on dedicated branches from an explicitly selected base. Promote a branch to a tested benchmark only after compilation, upload, and relevant bench tests pass.

**Repository:** `endyarfian/ATOMIC-FIRMWARE`

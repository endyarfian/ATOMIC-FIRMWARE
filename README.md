# A.T.O.M. — Access, Tracking & Operation Monitoring

**ATOMIC FIRMWARE** is an ESP32-S3-based biometric access-control and operator-monitoring prototype developed for a Komatsu PC200 application.

> **Prototype and safety notice:** This repository documents development firmware and bench testing. The project has not been validated as a certified machine safety or anti-theft system, nor confirmed for installation into OEM excavator wiring. Do not bypass OEM interlocks. Validate electrical isolation, relay fail-safe behavior, power transients, environmental durability, and machine-specific integration before any field use.

## Branch snapshot

- **Branch:** `v1.8.1`
- **Firmware file:** `ATOMIC_FIRMWARE_V1_8_1.ino`
- **Focus:** Fingerprint slot validation and captive portal
- **Device ID used in the prototype:** `ATOMIC-001`

Adds fingerprint slot availability validation for IDs 1–127, checks sensor occupancy alongside NVS metadata, avoids automatically recreating legacy operator metadata for IDs 1–3, and adds captive DNS/HTTP redirect behavior for supported client devices.

This README describes the repository/project context and the intended focus of this branch. Features documented for later versions are **not necessarily present** in this branch. Always inspect the source code on the selected branch before compiling or integrating it.

## Hardware platform

- **Controller:** ESP32-S3 DevKitC-1 N16R8
- **Flash:** 16 MB
- **PSRAM:** 8 MB OPI
- **OLED:** SSD1306, 128 × 64, I²C (commonly address `0x3C`)
- **RTC:** DS3231
- **Fingerprint sensor:** FPM10A / DY50-compatible UART sensor
- **RGB LED:** onboard NeoPixel
- **Inputs:** four push buttons
- **Outputs:** buzzer and relay driver
- **Ignition input:** digital ignition-sense signal

### V1.9.5 GPIO reference

The following pin assignment is documented for the V1.9.5 firmware. Earlier branches may differ; confirm the pin definitions in the selected sketch.

| Function | GPIO |
|---|---:|
| UP / DOWN / OK / BACK buttons | 4 / 5 / 6 / 7 |
| I²C SDA / SCL | 8 / 9 |
| Relay control | 10 |
| Ignition sense | 11 |
| Buzzer | 12 |
| Fingerprint UART TX / RX | 17 / 18 |
| RGB LED | 48 |

Fingerprint UART is configured for 57600 baud in the V1.9.5 source. Verify actual relay-driver polarity and isolation before connecting a load.

## Project capabilities

Capabilities introduced progressively across the version history include:

- Fingerprint enrollment, matching, and access decisions.
- Operator-name association with fingerprint sensor slots.
- Relay control following the firmware's access decision.
- OLED menus, system information, and user feedback.
- ESP32 SoftAP Admin Mode and embedded Web Admin.
- Device identity and system-status reporting.
- Wi-Fi Station configuration and reconnect behavior.
- MQTT heartbeat, access-event, and operator-session telemetry.
- RTC/NTP timekeeping and timestamp reporting.
- Hybrid local/remote MQTT transport in the V1.9.5 benchmark.

The exact feature set depends on the checked-out branch.

## Development history

| Version | Main milestone |
|---|---|
| V1.6 | Admin Mode foundation, SoftAP/Web Admin development, and core fingerprint workflow |
| V1.7.1–V1.7.3 | Admin Mode and OLED/Web Admin enrollment-feedback refinements |
| V1.7.4 | A.T.O.M. rebranding and bitmap/icon changes |
| V1.7.5 | OLED INFORMATION screen and firmware identity |
| V1.8.0 | Device ID foundation |
| V1.8.1–V1.8.4 | Fingerprint slot validation, captive portal behavior, and embedded JavaScript compatibility fixes |
| V1.8.5 | Earlier stable development benchmark for operator administration and device UI |
| V1.9.0 | Wi-Fi Station configuration and reconnect |
| V1.9.1 | Initial MQTT telemetry using PubSubClient |
| V1.9.2–V1.9.3 | Intermediate MQTT/device integration refinements |
| V1.9.4 | NTP synchronization and DS3231 timekeeping |
| V1.9.5 Hybrid MQTT | ESP-MQTT hybrid LAN TCP / Internet WSS transport; user-confirmed dashboard telemetry |
| V1.9.5 All State | Separate feature branch enabling successful fingerprint authentication with ignition ON or OFF; hardware validation pending |

## V1.9.5 Hybrid MQTT benchmark

The project context identifies `v1.9.5-hybrid-mqtt` as the user-confirmed benchmark. It uses:

- LAN MQTT over TCP: `mqtt://192.168.0.128:1883`
- Internet MQTT over secure WebSocket: `wss://mqtt.endyarfian.web.id:443/mqtt`
- Device-specific MQTT client identity based on `DEVICE_ID`.
- Device topic namespace:
  ```text
  atomic/devices/ATOMIC-001/heartbeat
  atomic/devices/ATOMIC-001/events
  atomic/devices/ATOMIC-001/sessions
  atomic/devices/ATOMIC-001/operators
  ```
- Controlled transport selection and periodic LAN recovery probing while WSS is active.

The user confirmed that data reached the dashboard through WSS while the device and dashboard were connected through different Wi-Fi networks. This confirmation applies to the tested V1.9.5 Hybrid MQTT firmware, not automatically to every historical branch.

## Operator identity and session model

The project distinguishes fingerprint sensor slots, device-local operator identifiers, and permanent backend identities:

- Each new operator registration receives a new permanent `operator_db_id`.
- A deleted database identity is marked inactive so historical records remain attributable.
- `device_operator_id` is managed by firmware and may be reused.
- Fingerprint sensor slots may be reused after deletion.
- A new registration does not automatically reactivate an older inactive database identity.

Access decisions and operator sessions are separate concepts: an access-granted event is not itself proof that a session has started or ended. Verify the implementation on the selected branch.

## Build and upload

The firmware is developed as an Arduino IDE sketch.

1. Check out the intended branch.
2. Open the matching `.ino` sketch in Arduino IDE.
3. Select the ESP32-S3 board profile appropriate for the N16R8 module.
4. Configure 16 MB flash and OPI PSRAM where supported by the selected board package.
5. Install the libraries required by that specific sketch. Common dependencies include Adafruit GFX, Adafruit SSD1306, RTClib, Adafruit Fingerprint Sensor Library, and Adafruit NeoPixel. MQTT dependencies vary by version.
6. Run **Verify/Compile** before uploading.
7. Upload to the ESP32-S3 and inspect Serial Monitor at 115200 baud.
8. Test the changed behavior on a safe bench setup before considering field integration.

Development environment used for later firmware revisions: Arduino IDE, Arduino-ESP32 Core 3.3.0, ESP-IDF 5.5 (through the selected Arduino core).

## Validation status

A source file or Git commit does not prove that firmware compiles or works on hardware.

User-confirmed milestones in the project history include the V1.6 core/Admin Mode operation, V1.7.1 normal operation, V1.9.5 Wi-Fi reconnect behavior, and V1.9.5 Hybrid MQTT telemetry reaching the dashboard over WSS from another Wi-Fi network.

The V1.9.5 All State branch has been committed, but physical compilation/upload and ignition-OFF relay/session testing remain to be confirmed. Do not infer test results for this branch from the Hybrid MQTT baseline.

## Security and deployment notes

- Never commit Wi-Fi passwords, MQTT passwords, API tokens, private keys, or production credentials.
- Early SoftAP/Web Admin credentials are prototype credentials and must be replaced before broader deployment.
- Use unique device identities and credentials; do not share MQTT client IDs or device topic namespaces across devices.
- Enforce broker ACLs per device.
- Do not expose embedded Web Admin directly to the public Internet.
- Review NVS/flash protection, Secure Boot, and Flash Encryption for any production security design.
- Validate relay default state, reset behavior, brownout behavior, isolation, and wiring fail-safe characteristics.
- Do not bypass OEM machine safety systems.

## Repository workflow

- Treat each version branch as its own firmware snapshot.
- Keep confirmed baselines unchanged while developing new features.
- Create feature branches from an explicitly selected known-good base.
- Record the tested branch, commit SHA, board configuration, and test conditions for every hardware validation.
- Promote a feature branch to a new benchmark only after the relevant compile, upload, and bench tests pass.

**Project:** A.T.O.M. — Access, Tracking & Operation Monitoring  
**Repository:** `endyarfian/ATOMIC-FIRMWARE`

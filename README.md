# A.T.O.M. — Access, Tracking & Operation Monitoring

**A.T.O.M.** is an ESP32-S3-based biometric access, operator-session, and telemetry prototype developed for a Komatsu PC200 excavator application. The firmware combines fingerprint authentication, relay control, ignition-state monitoring, local operator administration, Wi-Fi connectivity, and MQTT telemetry.

> **Prototype / safety notice:** The system has been bench-tested with a dummy 12 V supply and dummy relay load (up to 2 A). It has **not** been validated on a real excavator or connected to OEM machine wiring. Do not treat this repository as a certified safety or anti-theft system, and do not bypass OEM safety interlocks. Validate electrical isolation, relay fail-safe behavior, environmental durability, and machine-specific integration before any field deployment.

---

## Table of contents

- [Project goals](#project-goals)
- [Hardware platform](#hardware-platform)
- [Firmware and repository structure](#firmware-and-repository-structure)
- [Development history](#development-history)
- [Current V1.9.5 Hybrid MQTT baseline](#current-v195-hybrid-mqtt-baseline)
- [V1.9.5 All State branch](#v195-all-state-branch)
- [Authentication and operator sessions](#authentication-and-operator-sessions)
- [Admin Mode and local configuration](#admin-mode-and-local-configuration)
- [Wi-Fi and MQTT architecture](#wi-fi-and-mqtt-architecture)
- [Timekeeping](#timekeeping)
- [Power management](#power-management)
- [Build and upload](#build-and-upload)
- [Testing status](#testing-status)
- [Known limitations and roadmap](#known-limitations-and-roadmap)
- [Security notes](#security-notes)

## Project goals

The project evolved from a local fingerprint access prototype into **A.T.O.M. — Access, Tracking & Operation Monitoring**. Its intended responsibilities are:

1. Authenticate an operator using a fingerprint template.
2. Associate a fingerprint slot with an operator identity/name.
3. Control an external relay after successful authentication.
4. Track ignition state and operator-session start/end events.
5. Provide a local OLED interface and an ESP32 SoftAP-based Admin Mode.
6. Allow authorized administration of operators and device connectivity.
7. Report device health, access events, and operator sessions to a remote dashboard through MQTT.

The device identifier used by the current prototype is `ATOMIC-001`.

## Hardware platform

### Main controller

- **Board:** ESP32-S3 DevKitC-1 N16R8
- **Flash:** 16 MB
- **PSRAM:** 8 MB OPI
- **Arduino-ESP32 Core:** 3.3.0
- **ESP-IDF:** 5.5 (provided by the selected Arduino core)

### Peripherals

- 128 × 64 SSD1306 OLED, I²C address `0x3C`
- DS3231 real-time clock
- FPM10A / DY50-compatible fingerprint sensor
- One-channel RGB LED (onboard NeoPixel)
- Four push buttons
- Buzzer
- Relay driver and ignition-sense input

### GPIO assignment in the V1.9.5 firmware

| Function | GPIO |
|---|---:|
| Button UP | 4 |
| Button DOWN | 5 |
| Button OK | 6 |
| Button BACK | 7 |
| I²C SDA | 8 |
| I²C SCL | 9 |
| Buzzer | 12 |
| Fingerprint UART RX | 18 |
| Fingerprint UART TX | 17 |
| RGB LED | 48 |
| Relay control | 10 |
| Ignition sense | 11 |

Fingerprint UART is configured for **57600 baud**. The relay interface is documented in firmware as a 5 V HIGH-trigger relay input with a PC817 output that is active LOW. Verify the actual driver-board polarity and isolation before connecting any external load.

## Firmware and repository structure

The firmware is maintained as versioned Arduino sketches and Git branches. A feature branch should be created from a known-good baseline; experimental changes should not be committed directly to the stable benchmark.

Important source files include:

- `ATOMIC_FIRMWARE_V1_9_5.ino` — current V1.9.5 firmware source used by the Hybrid MQTT and All State branches.
- Earlier versioned `.ino` files — historical firmware snapshots.

### Branch policy

- Keep the confirmed V1.9.5 Hybrid MQTT baseline unchanged.
- Develop new behavior on dedicated branches created from that baseline.
- Treat a source-code commit as **code available**, not as proof that it compiles or works on hardware.
- Promote a branch to a tested baseline only after compilation, upload, and the relevant physical tests have been completed.

## Development history

The following is a high-level record of the milestones discussed and developed in this project. Not every intermediate revision is a separate production release.

| Version / milestone | Summary | Validation status |
|---|---|---|
| Early PC200 ACCESS prototype | ESP32-S3 fingerprint access control, OLED interface, RTC, buttons, buzzer, RGB indicator, and relay concept. | Bench prototype; not installed on a real excavator. |
| V1.6 | Admin Mode foundation and ESP32 SoftAP/web administration work. | User confirmed V1.6 functions, Admin Mode, persistence, fingerprint operation, and general stability on the ESP32-S3. |
| V1.7.1 | Improved Admin Mode OLED experience and enrollment status presentation; OLED messages aligned with Web Admin. | User confirmed the firmware ran normally. |
| V1.7.3 | Enrollment success/failure returns to the enrollment idle state after approximately four seconds; OLED and Web Admin state synchronization. | Source and GitHub change reviewed; based on the user-tested V1.7.1. |
| V1.7.4 | Project branding changed from PC200 ACCESS to A.T.O.M. | Historical branch had a bitmap/icon source issue identified during review. Do not use that historical snapshot as a build reference without checking it. |
| V1.8.5 | Established an improved baseline for operator administration, Admin Mode, status reporting, and RGB feedback. | Historical baseline documented in the repository. |
| V1.9.0 | Added Wi-Fi Station configuration through Web Admin, persistent router credentials, non-blocking connection attempts, and connection status reporting. | Firmware feature milestone; see the testing caveat below. |
| V1.9.1 | Added MQTT telemetry for heartbeat, access events, and operator sessions using PubSubClient. | Initial MQTT implementation; later replaced by the ESP-MQTT hybrid transport work. |
| V1.9.2–V1.9.3 | Intermediate development revisions leading to the V1.9.4 timekeeping work and V1.9.5 hybrid MQTT baseline. | Refer to the corresponding Git branches and source history for exact revision-level differences. |
| V1.9.4 | Added automatic NTP synchronization (Asia/Jakarta / WIB), periodic resynchronization, DS3231 updates, and manual Admin Mode time setting saved persistently. | Code changes reviewed; physical ESP32-S3/DS3231 validation was not confirmed in the project discussion. |
| V1.9.5 Hybrid MQTT | Established the hybrid LAN TCP / Internet WSS MQTT transport, with controlled connection routing and LAN recovery probing. | **User-confirmed working:** device data reached the dashboard over WSS while the device and dashboard were on different Wi-Fi networks. |
| V1.9.5 All State | New feature branch based on V1.9.5 Hybrid MQTT. A matching fingerprint grants access whether ignition is ON or OFF; relay activation and operator-session start follow the normal grant path. | Code change committed; physical compile/upload and hardware verification remain pending. |

### Current Git branches

- Stable benchmark: `v1.9.5-hybrid-mqtt`
- New feature branch: `v1.9.5-all-state`
- The human-readable feature label is **V1.9.5 (All State)**. The Git branch uses hyphens because spaces and parentheses are unsuitable in branch names.

## Current V1.9.5 Hybrid MQTT baseline

V1.9.5 Hybrid MQTT is the current user-confirmed firmware benchmark. It uses ESP-IDF's **ESP-MQTT** client rather than the earlier PubSubClient implementation.

### MQTT transports

| Transport | Endpoint | Purpose |
|---|---|---|
| LAN MQTT over TCP | `mqtt://192.168.0.128:1883` | Preferred local broker path |
| Internet MQTT over secure WebSocket | `wss://mqtt.endyarfian.web.id:443/mqtt` | Remote path when the LAN broker is unavailable |

The WSS path uses TLS certificate bundle attachment through `esp_crt_bundle_attach`. The firmware controls transport selection and reconnection rather than relying on ESP-MQTT automatic reconnect.

### Connection and recovery behavior

- LAN connection window: approximately 8 seconds.
- WSS connection window: approximately 15 seconds.
- While WSS is active, a FreeRTOS background task probes LAN availability approximately every 60 seconds.
- When LAN becomes available again, firmware can return to the local broker path.
- The MQTT client ID is based on `DEVICE_ID`.
- The Admin Mode MQTT status endpoint reports the active transport and WSS endpoint.
- The configured LAN broker host/port remain the local broker settings; the WSS endpoint is a firmware-side endpoint in this revision.

### MQTT topic namespace

For the current prototype device ID `ATOMIC-001`:

```text
atomic/devices/ATOMIC-001/heartbeat
atomic/devices/ATOMIC-001/events
atomic/devices/ATOMIC-001/sessions
atomic/devices/ATOMIC-001/operators
```

The first three topics are used for device heartbeat, access events, and operator-session telemetry. The operators topic is part of the device topic namespace; do not assume full operator database synchronization is complete unless the matching backend and firmware implementation have been verified.

### Backend and broker arrangement

The documented prototype arrangement is:

- Mosquitto local TCP listener on port `1883`.
- Mosquitto WebSocket listener on port `9001`, bound to `127.0.0.1`.
- Cloudflare Tunnel exposes the WSS hostname and forwards it to `http://127.0.0.1:9001`.
- The backend subscriber connects locally to `127.0.0.1:1883` using the `atomic_backend` account.
- The device MQTT account is scoped by ACL to its device topic namespace.

The WSS handshake was previously confirmed with HTTP status **101 Switching Protocols**. The user also confirmed that telemetry arrived at the dashboard from a different Wi-Fi network.

### Wi-Fi behavior

The V1.9.5 Wi-Fi reconnect logic includes:

- Check whether the station is already connected before starting another connection attempt.
- Disconnect the previous station state before a new connection attempt.
- Use a 20-second connection timeout.
- Retry at approximately 30-second intervals.

The user tested the reconnect change and reported that it worked correctly and dashboard data was received.

## V1.9.5 All State branch

Branch: [`v1.9.5-all-state`](https://github.com/endyarfian/ATOMIC-FIRMWARE/tree/v1.9.5-all-state)

This branch is cloned from the V1.9.5 Hybrid MQTT benchmark and changes the ignition restriction applied after a successful fingerprint match.

### Intended access behavior

| Ignition state | Fingerprint result | Expected result |
|---|---|---|
| ON | Match | ACCESS GRANTED |
| OFF | Match | ACCESS GRANTED |
| ON or OFF | No match | ACCESS DENIED |

For a successful match in either ignition state, the firmware follows the full grant path:

1. Mark the operator session active.
2. Start the operator session.
3. Activate the relay.
4. Publish the access-granted event.
5. Show the granted indication and success feedback.

The existing ignition transition handler remains in place: when ignition transitions to OFF while a session is active, it closes the session and deactivates the relay. The All State change removes the **ignition-OFF fingerprint denial**; it does not remove the existing session shutdown behavior on an ignition transition.

**Important scope:** “All State” in this branch means both ignition states while the firmware is awake and the fingerprint scan loop is running. It does not mean fingerprint scanning while the ESP32 is in deep sleep, during Admin Mode, or on screens where scanning is intentionally inactive.

This branch has been committed to GitHub, but compilation and physical-device testing have not yet been confirmed. It must not be described as hardware-validated until the OFF-state and ON-state tests have both passed.

## Authentication and operator sessions

The firmware uses the fingerprint sensor to capture an image, convert it to a template buffer, and search the sensor's enrolled template database.

A successful match provides a fingerprint slot ID and confidence value. The firmware resolves the slot to an operator name and uses that identity in access/session reporting.

### Operator identity model

The project distinguishes between:

- A permanent database-side operator identity (`operator_db_id`).
- A firmware-managed device operator ID (`device_operator_id`).
- A fingerprint sensor slot.

The selected identity policy is:

- Every new operator registration receives a new permanent `operator_db_id`.
- A deleted database identity is marked inactive rather than erased, preserving its historical records.
- `device_operator_id` is managed by firmware and may be reused.
- Fingerprint slots may be reused after deletion.
- A newly registered operator does not automatically recover an older inactive identity.

This policy is intended to prevent old activity records from being silently attributed to a different person after a slot or device-local ID is reused.

### Access events and sessions

The firmware distinguishes access decisions from operator sessions:

- `ACCESS_GRANTED` / `ACCESS_DENIED` describe an authentication result.
- `SESSION_STARTED` / `SESSION_ENDED` describe an operator's active session.
- Session telemetry includes the operator identity, timestamps, and session duration where available.

Do not treat an access event as equivalent to a session record.

## Admin Mode and local configuration

Admin Mode is provided through the ESP32 SoftAP and an embedded Web Admin interface. It is intended for local device setup and operator administration.

Capabilities developed through the project include:

- Operator/fingerprint enrollment and operator management.
- System status and firmware/device identity display.
- Wi-Fi Station configuration and connection status.
- MQTT broker configuration/status in the earlier MQTT implementation.
- Time configuration, including automatic NTP and manual time setting.
- Local device administration without requiring the remote dashboard.

The Web Admin exposes JSON endpoints for device status and configuration. Endpoints documented in earlier revisions include:

- `GET /api/status`
- `POST /api/wifi/configure`
- `GET /api/wifi/status`
- `POST /api/mqtt/configure`
- `GET /api/mqtt/status`

The exact endpoint set may differ by firmware branch. Check the source on the branch being built before integrating a client against these routes.

Admin Mode has an inactivity timeout (approximately 10 minutes in the documented V1.8.x design). The operational firmware also has a separate automatic sleep timeout.

## Timekeeping

The device includes a DS3231 RTC. The V1.9.4 timekeeping work provides:

- Automatic NTP synchronization using the **Asia/Jakarta** timezone (WIB).
- Periodic NTP resynchronization, documented at approximately six-hour intervals.
- Updating the DS3231 after a valid network time is obtained.
- Manual time setting from Admin Mode.
- Saving manually configured time/settings persistently.
- Protection against replacing a valid RTC time with an invalid NTP result.

The RTC is used for timestamps in telemetry. Earlier MQTT implementation notes specify an explicit unavailable marker when the RTC cannot provide a valid time. Validate time behavior on the target branch and hardware before relying on timestamps for audit purposes.

## Power management

The V1.9.5 source defines an automatic sleep timeout of approximately **one hour**. The sleep logic is designed to avoid entering sleep while an operator session is active, while ignition is ON, or while Admin Mode is active.

Deep sleep affects fingerprint availability: the All State branch does not make the fingerprint sensor continuously available while the ESP32 is asleep. Wake behavior and the user interaction required to resume scanning should be tested on the actual hardware.

## Build and upload

The firmware is developed as an Arduino IDE sketch for the ESP32-S3 DevKitC-1 N16R8.

1. Check out the branch you intend to test.
2. Open the matching `.ino` file in Arduino IDE.
3. Select the ESP32-S3 DevKitC-1 board and configure flash/PSRAM settings for the N16R8 target.
4. Install the libraries used by the sketch, including Adafruit GFX, Adafruit SSD1306, RTClib, Adafruit Fingerprint Sensor Library, and Adafruit NeoPixel. The current hybrid MQTT firmware also uses ESP-IDF's `mqtt_client.h` and `esp_crt_bundle.h`, supplied by the ESP32 Arduino core.
5. Compile/Verify before uploading.
6. Upload to the ESP32-S3 and inspect Serial Monitor at **115200 baud**.
7. Test each changed feature using a safe bench setup before considering deployment.

Library versions and exact Arduino IDE board-menu settings should be recorded alongside a release when a reproducible build is prepared.

## Testing status

### Confirmed by user

- V1.6 core functions, Admin Mode, persistence, fingerprint operation, and general stability worked on the ESP32-S3.
- V1.7.1 was reported to run normally.
- V1.9.5 Wi-Fi reconnect behavior was tested and reported working.
- V1.9.5 Hybrid MQTT delivered device data to the dashboard over WSS while the device and dashboard used different Wi-Fi networks.

### Not yet confirmed

- V1.9.5 All State compilation and physical behavior.
- Full relay and ignition integration on an actual excavator.
- Long-duration field reliability, electrical transient tolerance, and environmental testing.
- End-to-end operator identity synchronization and historical record behavior across multiple physical devices.
- Recovery and event-delivery guarantees during broker/network outages.

Always report test results with the exact firmware branch, source commit, hardware setup, and test conditions.

## Known limitations and roadmap

The following items were discussed as future work or remain outside the confirmed V1.9.5 implementation:

1. **All State validation:** compile and test fingerprint authentication with ignition ON and OFF, including relay activation and session lifecycle.
2. **Multi-device provisioning:** the current `DEVICE_ID` and MQTT topic namespace are compile-time/device-specific. A new device must not share another device's client ID or topic namespace. Per-device identity provisioning, credentials, ACLs, and backend registration need a complete design before plug-and-play multi-device deployment.
3. **Operator synchronization:** complete and verify the backend workflow for registering, updating, deactivating, and mapping operator identities without losing historical attribution.
4. **Offline telemetry:** define buffering, persistence, replay, ordering, and duplicate handling for events generated while MQTT is disconnected.
5. **Security hardening:** replace prototype credentials, review authentication and authorization, protect stored secrets, and assess ESP32 flash/NVS security.
6. **Field validation:** only after controlled electrical and safety reviews, validate the system in the intended machine environment without bypassing OEM safety systems.

## Security notes

- Do not commit Wi-Fi passwords, MQTT passwords, API tokens, private keys, or production credentials to this repository.
- Prototype SoftAP/Web Admin credentials used during early development are not production-grade; rotate them before any broader use.
- Keep MQTT credentials unique per device and enforce broker ACLs for each device's topic namespace.
- Do not expose the embedded Web Admin directly to the public Internet.
- NVS storage alone does not guarantee protection against physical flash extraction. Evaluate ESP32 Secure Boot and Flash Encryption as part of a production security design.
- Use TLS for remote MQTT transport and validate certificate handling.
- Review relay default state, reset behavior, brownout behavior, and wiring fail-safe characteristics before connecting to external equipment.

---

## Project status

**Current benchmark:** V1.9.5 Hybrid MQTT (`v1.9.5-hybrid-mqtt`).

**Active feature development:** V1.9.5 All State (`v1.9.5-all-state`).

The All State branch is intended to grant full access on a valid fingerprint match regardless of ignition ON/OFF. It remains an experimental branch until compile and bench tests are completed. The stable benchmark branch should remain unchanged while this feature is validated.

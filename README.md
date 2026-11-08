# ATOMIC FIRMWARE

**A.T.O.M. — Access, Tracking & Operation Monitoring**

Firmware sistem akses biometrik berbasis ESP32-S3 dengan sensor sidik jari, OLED, RTC, dan Admin Mode melalui Wi-Fi SoftAP.

## Perangkat keras

- **Board:** ESP32-S3 DevKitC-1 N16R8
- **Flash:** 16 MB
- **PSRAM:** 8 MB OPI
- **OLED:** SSD1306 128×64, I²C
- **RTC:** DS3231
- **Fingerprint:** FPM10A / DY50, UART 57600
- **RGB LED:** GPIO48
- **Buzzer:** GPIO12

Konfigurasi pin dan komponen harus diperiksa pada source firmware untuk versi yang digunakan.

## Fitur utama

- Autentikasi sidik jari.
- Admin Mode melalui ESP32 SoftAP.
- Web Admin untuk pengelolaan operator dan sidik jari.
- Informasi status sistem.
- Tampilan OLED untuk jam, status Admin Mode, proses enrollment, hasil enrollment, informasi firmware, dan logo.
- Timeout sesi Admin Mode dan umpan balik proses enrollment.

## Versi firmware

Setiap versi disimpan pada branch tersendiri. Nama file firmware menggunakan pola:

`ATOMIC_FIRMWARE_V1_7_X.ino`

| Versi | Branch | Catatan |
|---|---|---|
| V1.7.1 | `v1.7.1` | Admin Mode awal |
| V1.7.2 | `v1.7.2` | Penyempurnaan pembaruan tampilan OLED |
| V1.7.3 | `v1.7.3` | Timeout tampilan hasil enrollment |
| V1.7.4 | `v1.7.4` | Rebranding menjadi A.T.O.M. serta pembaruan bitmap |
| V1.7.5 | `v1.7.5` | Penambahan menu INFORMATION dan identitas firmware |

> Catatan: tabel merangkum perubahan utama yang tercatat. Status kompilasi dan pengujian perangkat harus dilihat dari hasil pengujian masing-masing versi; keberadaan source di repository bukan bukti bahwa versi tersebut sudah diuji pada perangkat.

## Lingkungan pengembangan

Konfigurasi yang digunakan untuk pengembangan:

- Arduino IDE
- Arduino-ESP32 Core **3.3.0**
- Board profile: **ESP32S3 Dev Module**
- Flash Size: **16 MB**
- PSRAM: **OPI PSRAM**

Pastikan library yang diperlukan tersedia di Arduino IDE, termasuk Adafruit GFX, Adafruit SSD1306, RTClib, Adafruit Fingerprint Sensor Library, dan Adafruit NeoPixel.

## Cara menggunakan

1. Buka branch versi yang ingin digunakan.
2. Unduh `ATOMIC_FIRMWARE_V1_7_X.ino` dari branch tersebut.
3. Buka file di Arduino IDE.
4. Pilih konfigurasi board ESP32-S3 yang sesuai dengan perangkat.
5. Periksa konfigurasi pin dan kredensial sebelum proses upload.
6. Kompilasi terlebih dahulu, kemudian lakukan pengujian pada perangkat bench.

## Catatan keselamatan

Repository ini ditujukan untuk pengembangan dan pengujian prototipe. Pengujian awal harus menggunakan catu daya dan beban dummy yang sesuai. Jangan menghubungkan firmware ke sistem kontrol mesin atau mengubah/mem-bypass interlock OEM tanpa kajian teknis, dokumentasi mesin yang spesifik, dan penanganan oleh teknisi yang kompeten.

## Identitas proyek

**A.T.O.M. — Access, Tracking & Operation Monitoring**  
Nama repository yang dituju: **ATOMIC FIRMWARE**

# ATOMIC FIRMWARE

**A.T.O.M. — Access, Tracking & Operation Monitoring**

Firmware sistem akses biometrik berbasis ESP32-S3.

## Versi dan baseline

**V1.8.5 tetap menjadi baseline resmi; V1.9.0 adalah pengembangan berikutnya yang dibuat langsung dari baseline tersebut.**

- Baseline: branch `v1.8.5` — `ATOMIC_FIRMWARE_V1_8_5.ino`
- Pengembangan: branch `v1.9.0` — `ATOMIC_FIRMWARE_V1_9_0.ino`
- Device ID prototipe: `ATOMIC-001`
- V1.8.5 dikembangkan langsung dari V1.8.0.
- V1.8.1–V1.8.4 tidak digunakan sebagai basis pengembangan karena memiliki bug.
- Pengembangan versi berikutnya harus menggunakan V1.8.5 sebagai sumber dasar dan mempertahankan fitur yang sudah ada.

## Fitur utama

- Autentikasi sidik jari menggunakan sensor fingerprint.
- Pengelolaan nama operator berdasarkan ID fingerprint.
- Admin Mode melalui Wi-Fi SoftAP.
- Web Admin untuk enrollment dan pengelolaan operator.
- Tampilan informasi perangkat pada OLED.
- RTC untuk informasi waktu.
- Indikator RGB LED untuk status sistem, autentikasi, dan enrollment.

## Perubahan dan ketentuan V1.8.5

### RGB LED

| Kondisi | Indikator |
|---|---|
| Sistem menunggu sidik jari | Biru |
| Akses diberikan / fingerprint cocok | Hijau |
| Akses ditolak / fingerprint tidak terverifikasi | Merah |
| Admin Mode idle | Breathing biru |
| Proses enrollment berlangsung | Blinking oranye |
| Enrollment berhasil | Blinking hijau |
| Enrollment gagal | Blinking merah |
| Kegagalan sistem/perangkat fingerprint | Merah |

### Admin Mode dan Web Admin

- SSID SoftAP mengikuti pola `<DEVICE_ID> SSID` (contoh: `ATOMIC-001 SSID`).
- Password SoftAP: `atomicadmin`.
- HTTP Basic Authentication:
  - Username: `admin`
  - Password: `atomicadmin`
- Halaman Admin menggunakan ikon fingerprint dan judul **A.T.O.M. — ATOMIC ADMIN PAGE**.
- Subtitle: **Access Tracking & Operational Monitoring**.
- Footer menampilkan **ATOMIC SYSTEM © 2026, ENLISTERS TECHNOLOGY**.
- Admin Mode memiliki timeout tidak aktif sekitar 10 menit.
- Indikator RGB Admin Mode diperbarui secara non-blocking.

### Enrollment dan operator

- ID operator baru dimulai dari 1.
- Rentang ID fingerprint yang dapat digunakan: 1–127.
- Nama placeholder operator lama untuk ID 1–3 dibersihkan dari penyimpanan, tanpa menghapus template sidik jari.
- Proses enrollment menggunakan dua kali pemindaian sidik jari yang sama.
- Status keberhasilan atau kegagalan enrollment ditampilkan pada OLED dan Web Admin.

### Identitas dan status perangkat

- Informasi OLED menampilkan Device ID, nama firmware, versi firmware, dan alamat IP Admin Mode.
- Endpoint `/api/status` menyediakan field:
  - `device_id`
  - `firmware_version`
- Web Admin menampilkan Device ID dan Firmware Version pada bagian System Status.

## Perubahan V1.9.0 — Wi-Fi STA Configuration

- Menambahkan konfigurasi Wi-Fi router melalui bagian **Wi-Fi Configuration** pada Web Admin.
- Admin memasukkan SSID dan password, kemudian memilih **Save & Connect**.
- ESP32 menjalankan koneksi STA secara non-blocking sehingga Web Admin melalui SoftAP tetap dapat digunakan selama proses koneksi.
- Status koneksi diperbarui otomatis setiap 2 detik: not configured, connecting, connected, atau disconnected.
- Saat tersambung, Web Admin menampilkan SSID, alamat IP lokal, dan RSSI.
- Kredensial disimpan pada namespace NVS terpisah (`atomicwifi`) dan digunakan kembali setelah perangkat restart.
- Wi-Fi STA mencoba koneksi ulang secara otomatis; timeout percobaan 20 detik dan interval percobaan ulang 30 detik.
- Mode Wi-Fi saat Admin Mode aktif adalah AP+STA. Saat Admin Mode ditutup, SoftAP dimatikan sementara STA dipertahankan jika kredensial tersedia.
- Admin AP tetap hanya aktif selama Admin Mode; Wi-Fi STA dapat tetap aktif pada mode operasional untuk konektivitas backend.
- Password router tidak dikirim kembali oleh endpoint status dan tidak dicetak ke Serial Monitor.
- Endpoint Admin: `POST /api/wifi/configure` dan `GET /api/wifi/status`.
- Status MQTT belum diimplementasikan pada firmware V1.9.0; koneksi Wi-Fi yang berhasil tidak berarti MQTT sudah tersambung.

## Konfigurasi perangkat

Target perangkat pengembangan:

- Board: ESP32-S3 DevKitC-1 N16R8
- Flash: 16 MB
- PSRAM: 8 MB OPI
- Arduino-ESP32 Core: 3.3.0

Pastikan konfigurasi board dan library pada Arduino IDE sesuai dengan perangkat yang digunakan.

## Catatan keamanan

Kredensial SoftAP dan Web Admin di atas merupakan kredensial prototipe. Ganti kredensial sebelum perangkat digunakan di lingkungan produksi. Jangan mengekspos Web Admin ke jaringan publik tanpa lapisan keamanan tambahan. Kredensial Wi-Fi router disimpan di NVS perangkat; perlindungan terhadap pembacaan fisik flash memerlukan konfigurasi keamanan ESP32 tambahan seperti flash encryption.

## Status pengujian dan pengembangan

V1.8.5 tetap menjadi baseline resmi dan V1.9.0 merupakan pengembangan pertama di atas baseline tersebut.

Source di repository belum otomatis berarti firmware telah lolos kompilasi atau pengujian perangkat. Lakukan Verify/Compile di Arduino IDE dan pengujian bench pada ESP32-S3 sebelum digunakan. Integrasi MQTT, sinkronisasi data operator, pencatatan event akses, dan manajemen sesi belum termasuk implementasi V1.9.0 dan akan dikembangkan serta diuji secara bertahap.

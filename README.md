# ATOMIC FIRMWARE

**A.T.O.M. — Access, Tracking & Operation Monitoring**

Firmware sistem akses biometrik berbasis ESP32-S3.

## Versi dan baseline

**V1.8.5 ditetapkan sebagai benchmark/baseline pengembangan firmware A.T.O.M. saat ini.**

- Branch aktif: `v1.8.5`
- File firmware: `ATOMIC_FIRMWARE_V1_8_5.ino`
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

## Konfigurasi perangkat

Target perangkat pengembangan:

- Board: ESP32-S3 DevKitC-1 N16R8
- Flash: 16 MB
- PSRAM: 8 MB OPI
- Arduino-ESP32 Core: 3.3.0

Pastikan konfigurasi board dan library pada Arduino IDE sesuai dengan perangkat yang digunakan.

## Catatan keamanan

Kredensial SoftAP dan Web Admin di atas merupakan kredensial prototipe. Ganti kredensial sebelum perangkat digunakan di lingkungan produksi. Jangan mengekspos Web Admin ke jaringan publik tanpa lapisan keamanan tambahan.

## Status pengujian dan pengembangan

V1.8.5 merupakan baseline pengembangan yang ditetapkan saat ini. Setiap perubahan berikutnya harus dibuat sebagai versi baru berdasarkan baseline ini.

Source di repository belum otomatis berarti firmware telah lolos kompilasi atau pengujian perangkat. Lakukan Verify/Compile di Arduino IDE dan pengujian bench pada ESP32-S3 sebelum digunakan. Integrasi MQTT, sinkronisasi data operator, pencatatan event akses, dan manajemen sesi akan dikembangkan serta diuji secara bertahap.

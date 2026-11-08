# ATOMIC FIRMWARE

**A.T.O.M. — Access, Tracking & Operation Monitoring**

Firmware akses biometrik berbasis ESP32-S3.

## Versi

Branch ini adalah pengembangan **A.T.O.M. V1.8.5** dari baseline stabil V1.7.5.

File firmware: `ATOMIC_FIRMWARE_V1_8_5.ino`.

## Perubahan V1.8.5

V1.8.5 dibuat langsung dari baseline V1.8.0. Branch V1.8.1–V1.8.4 tidak dijadikan dasar karena masih memiliki bug.

- Admin Mode: LED RGB breathing biru.
- Enrollment berhasil: LED RGB blinking hijau.
- Enrollment gagal: LED RGB blinking merah.
- ID operator dapat dimulai dari 1; penamaan operator lama untuk ID 1–3 tidak lagi dibuat otomatis.
- SSID SoftAP mengikuti pola `<DEVICE_ID> SSID`.
- Password SoftAP: `atomicadmin`.
- HTTP Basic Authentication: username `admin`, password `atomicadmin`.
- Header halaman Admin menggunakan ikon fingerprint dan judul **A.T.O.M. — ATOMIC ADMIN PAGE**.
- Subtitle dan footer halaman Admin diperbarui.
- Device ID prototipe saat ini: `ATOMIC-001`.

## Catatan keamanan

Kredensial Admin Mode sesuai permintaan untuk prototipe. Ganti sebelum perangkat digunakan di lingkungan produksi.

## Status pengembangan

V1.8.5 masih dalam tahap pengembangan. Perubahan komunikasi MQTT, sinkronisasi operator, event akses, enrollment, dan sesi akan ditambahkan dan diuji bertahap.

Lakukan kompilasi dan pengujian bench pada ESP32-S3 sebelum penggunaan. Source yang tersimpan di repository belum berarti firmware telah lolos kompilasi atau pengujian perangkat.

# ATOMIC FIRMWARE

**A.T.O.M. — Access, Tracking & Operation Monitoring**

Firmware akses biometrik berbasis ESP32-S3.

## Versi

Branch ini adalah pengembangan **A.T.O.M. V1.8.0** dari baseline stabil V1.7.5.

File firmware: `ATOMIC_FIRMWARE_V1_8_0.ino`.

## Perubahan awal V1.8.0

- Identitas perangkat ditetapkan melalui konstanta `DEVICE_ID`.
- Menu INFORMATION menampilkan header **DEVICE ID**, nilai Device ID, nama sistem, dan versi firmware.
- Device ID prototipe saat ini: `ATOMIC-001`.
- V1.7.5 tetap dipertahankan sebagai branch baseline terpisah.

## Status pengembangan

V1.8.0 masih dalam tahap pengembangan. Perubahan komunikasi MQTT, sinkronisasi operator, event akses, enrollment, dan sesi akan ditambahkan dan diuji bertahap.

Lakukan kompilasi dan pengujian bench pada ESP32-S3 sebelum penggunaan. Source yang tersimpan di repository belum berarti firmware telah lolos kompilasi atau pengujian perangkat.

# ATOMIC FIRMWARE

**A.T.O.M. — Access, Tracking & Operation Monitoring**

Firmware akses biometrik berbasis ESP32-S3.

## Versi

Branch ini adalah pengembangan **A.T.O.M. V1.8.3** dari V1.8.2. Baseline stabil V1.7.5 tetap berada pada branch terpisah.

File firmware: `ATOMIC_FIRMWARE_V1_8_3.ino`.

## Perubahan V1.8.0 yang dipertahankan

- Identitas perangkat ditetapkan melalui konstanta `DEVICE_ID`.
- Menu INFORMATION menampilkan header **DEVICE ID**, nilai Device ID, nama sistem, dan versi firmware.
- Device ID prototipe saat ini: `ATOMIC-001`.
- V1.7.5 tetap dipertahankan sebagai branch baseline terpisah.

## Perubahan V1.8.1 (dipertahankan)


- Fingerprint ID 1–127 dapat digunakan jika slot sensor benar-benar kosong.
- Validasi enrollment memeriksa metadata NVS dan okupansi slot langsung pada sensor untuk mencegah ID ganda.
- Metadata operator ID 1–3 tidak lagi otomatis dibuat ulang saat boot.
- System Status menampilkan Device ID dan versi firmware.
- Captive DNS dan HTTP redirect ditambahkan agar perangkat yang mendukung captive-portal detection dapat membuka portal Admin secara otomatis setelah tersambung ke SoftAP.

## Perbaikan V1.8.2

- Pemeriksaan slot tidak lagi menggunakan `loadModel()`, karena perintah tersebut memuat template ke buffer dan dapat menghasilkan DBRANGEFAIL pada slot kosong.
- Enrollment membaca **Read Index Table (command 0x1F)** dari sensor untuk memeriksa bit okupansi ID secara langsung.
- ID yang terisi ditolak; ID kosong dapat digunakan kembali.
- Jika index table tidak dapat dibaca atau respons sensor tidak valid, enrollment dihentikan dengan pesan error. Firmware tidak menganggap slot kosong ketika pemeriksaan gagal.

## Perbaikan V1.8.3

- JavaScript pada Admin Page diubah dari `async`/`await` menjadi Promise (`.then()`/`.catch()`).
- Menghindari konflik Arduino sketch preprocessor yang dapat menggabungkan token `async function` menjadi `asyncfunction` dan menyebabkan compilation error.
- Alur refresh status, daftar operator, rename, delete, enrollment, polling status, dan cancel enrollment tetap menggunakan request asynchronous berbasis Promise.

## Status pengembangan

V1.8.3 belum dikompilasi atau diuji pada perangkat fisik. Captive portal otomatis bergantung pada dukungan dan perilaku deteksi captive portal di sistem operasi HP/komputer.

Lakukan kompilasi dan pengujian bench pada ESP32-S3 sebelum penggunaan. Source yang tersimpan di repository belum berarti firmware telah lolos kompilasi atau pengujian perangkat.

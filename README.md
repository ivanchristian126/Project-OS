# Time Sharing System

Aplikasi C untuk proyek time-sharing pada sistem operasi Linux.

Versi `0.1.0` menyediakan program awal yang menampilkan informasi aplikasi dan konfigurasi default. Penjadwalan proses, signal, timer, dan integrasi kernel belum diimplementasikan.

## Persyaratan

| Komponen | Kebutuhan |
| --- | --- |
| Sistem operasi | Linux |
| Compiler | GCC dengan dukungan C11 |
| Build tool | GNU Make |
| Shell | Bash |

Pada Ubuntu atau Debian, instal perangkat build melalui terminal:

```bash
sudo apt update
sudo apt install build-essential
```

## Kompilasi

Buka terminal di direktori proyek, kemudian jalankan:

```bash
bash build.sh
```

Executable akan dibuat di `build/timesharing`. Alternatifnya, gunakan `make` untuk melakukan kompilasi langsung.

## Menjalankan Aplikasi

```bash
./build/timesharing
```

Contoh output versi awal:

```text
Time Sharing System v0.1.0
Default configuration: 3 processes, 100 ms quantum.
Status: initial version. Scheduling is not yet available.
```

Nilai default yang ditampilkan merupakan konfigurasi awal; belum ada proses anak atau timer yang dijalankan.

Untuk menampilkan bantuan:

```bash
./build/timesharing --help
```

## Perintah Build

| Perintah | Fungsi |
| --- | --- |
| `bash build.sh` | Mengompilasi aplikasi melalui skrip build |
| `make` | Mengompilasi aplikasi secara langsung |
| `make run` | Mengompilasi dan menjalankan aplikasi |
| `make clean` | Menghapus direktori hasil build |

## Pemecahan Masalah

- **`gcc` atau `make` tidak ditemukan:** instal paket `build-essential`.
- **Executable belum tersedia:** jalankan `bash build.sh` dan pastikan proses kompilasi selesai tanpa error.
- **Perintah tidak berjalan di Windows:** gunakan terminal Linux, misalnya Ubuntu di mesin virtual. Executable hasil build ditujukan untuk Linux.

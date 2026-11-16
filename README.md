# Time Sharing System

Simulasi penjadwalan time sharing pada Linux. Aplikasi membuat beberapa
proses anak dengan `fork()`, memberi giliran kepada setiap proses secara
Round Robin, dan mengatur pergantian giliran memakai timer berbasis signal
serta `SIGSTOP` dan `SIGCONT`. Tersedia juga syscall kernel tambahan
bernama `ts_mark` untuk mencatat setiap pergantian giliran dari sisi
kernel.

Versi `1.0.0`.

## Fitur

| Bagian | Keterangan |
| --- | --- |
| Antrean proses | Antrean FIFO kapasitas tetap: menambah, mengambil, menghapus |
| Penjadwal | Round Robin dengan quantum yang dapat diatur |
| Proses anak | Satu proses per giliran, beban dibatasi waktu CPU sendiri |
| Pergantian giliran | Timer POSIX mengirim `SIGALRM`, proses ditahan `SIGSTOP` |
| Masukan | Jumlah proses, quantum, dan beban lewat prompt atau opsi |
| Penghentian | `Ctrl+C` menutup seluruh proses anak tanpa meninggalkan zombie |
| Integrasi kernel | Memanggil syscall `ts_mark` bila kernel sudah dimodifikasi |

## Persyaratan

| Komponen | Kebutuhan |
| --- | --- |
| Sistem operasi | Linux |
| Compiler | GCC dengan dukungan C11 |
| Build tool | GNU Make |
| Shell | Bash |

Pada Ubuntu atau Debian:

```bash
sudo apt update
sudo apt install build-essential
```

Bagian kernel memerlukan paket tambahan dan mesin virtual. Lihat
[docs/kernel-setup.md](docs/kernel-setup.md).

## Struktur Proyek

```text
include/            header publik seluruh modul
src/cli.c           pembacaan masukan dari terminal
src/main.c          integrasi simulasi dan laporan akhir
src/scheduler/      antrean proses dan Round Robin
src/runtime/        proses anak, signal, dan timer quantum
src/kernel_bridge/  pemanggilan syscall ts_mark
kernel/             sumber syscall, skrip patch, dan uji syscall
tests/              uji unit dan uji skenario
scripts/            pencatat lingkungan dan pembuat laporan
docs/               panduan kernel dan laporan hasil
```

## Kompilasi

```bash
bash build.sh
```

Executable dibuat di `build/timesharing`. Alternatifnya gunakan `make`.

## Menjalankan Aplikasi

Tanpa opsi, program menanyakan konfigurasi lewat terminal. Menekan Enter
memakai nilai default.

```bash
./build/timesharing
```

Konfigurasi dapat juga diberikan langsung:

```bash
./build/timesharing --processes 4 --quantum 50 --workload 300
```

| Opsi | Fungsi | Batas | Default |
| --- | --- | --- | --- |
| `-p`, `--processes` | Jumlah proses anak | 1 sampai 16 | 3 |
| `-q`, `--quantum` | Panjang satu giliran dalam ms | 10 sampai 2000 | 100 |
| `-w`, `--workload` | Beban tiap proses dalam ms | 50 sampai 10000 | 500 |
| `-v`, `--version` | Menampilkan versi | | |
| `-h`, `--help` | Menampilkan bantuan | | |

Masukan di luar batas, bukan angka, atau opsi yang tidak dikenal ditolak
dengan pesan yang menyebut nama nilai beserta batasnya, dan program keluar
dengan kode 2.

## Cara Kerja Penjadwalan

1. Program membaca konfigurasi, lalu memasang handler signal dan timer.
2. Setiap proses anak dibuat dengan `fork()` dan langsung ditahan memakai
   `SIGSTOP` sampai gilirannya datang.
3. Proses terdepan diambil dari antrean, dicatat ke kernel bila syscall
   tersedia, lalu dilanjutkan dengan `SIGCONT`.
4. Timer POSIX diatur sepanjang quantum. Giliran berakhir karena quantum
   habis, proses anak selesai, atau `Ctrl+C`.
5. Proses yang belum selesai ditahan kembali dan dikembalikan ke ekor
   antrean. Proses yang sudah selesai dikeluarkan dari antrean sehingga
   tidak mendapat giliran lagi.
6. Setelah antrean kosong, program menunggu seluruh proses anak,
   membersihkan sisa proses, lalu mencetak laporan.

Beban tiap proses anak dibatasi oleh waktu CPU proses itu sendiri, bukan
waktu dinding. Proses yang sedang ditahan `SIGSTOP` tidak menghabiskan
kuota bebannya.

## Susunan Keluaran

Keluaran terdiri dari tiga bagian: baris konfigurasi dan status fitur
kernel, baris giliran selama simulasi, lalu laporan akhir.

```text
Time Sharing System v1.0.0
Konfigurasi: <n> proses, quantum <n> ms, beban <n> ms per proses.
Fitur kernel: <status syscall ts_mark>
<n> proses anak dibuat, penjadwalan Round Robin dimulai.
Giliran <n>: proses <n> (pid <n>) jatah <n> ms, sisa <n> ms
    [worker <n>] pid <n> progres <n>%
  proses <n> selesai setelah <n> giliran

Laporan hasil simulasi
proses  pid      giliran  waktu(ms)  selesai(ms)  status
...
Total pergantian giliran: <n>
Proses selesai: <n> dari <n>
Proses gagal: <n>
Giliran tercatat kernel: <n>
Durasi simulasi: <n> ms
```

Keluaran sebenarnya beserta lingkungan pengujiannya ada di
[docs/report.md](docs/report.md).

## Pengujian

```bash
make test
```

Perintah di atas menjalankan uji unit antrean dan Round Robin: urutan
masuk dan keluar, penolakan pid duplikat dan antrean penuh, penghapusan
dari tengah antrean, perilaku buffer sirkular, kasus satu proses, kasus
banyak proses, serta proses yang selesai lebih awal dan harus dilewati.

```bash
make check
```

Perintah ini menjalankan uji unit lalu menjalankan aplikasi pada beberapa
skenario: satu proses, empat proses, quantum lebih besar dari beban,
jumlah proses maksimum, quantum minimum, dan sejumlah masukan tidak valid.

## Laporan Hasil

```bash
bash scripts/run_report.sh
```

Skrip menjalankan aplikasi dan seluruh uji, lalu menulis hasilnya ke
[docs/report.md](docs/report.md) bersama catatan lingkungan.

## Bagian Kernel

| Dokumen | Isi |
| --- | --- |
| [docs/kernel-setup.md](docs/kernel-setup.md) | Penyiapan VM, paket build, dan kompilasi kernel |
| [docs/kernel-syscall.md](docs/kernel-syscall.md) | Syscall `ts_mark`, patch, pengujian, dan integrasi |
| [docs/kernel-env.txt](docs/kernel-env.txt) | Catatan versi lingkungan yang dipakai |

## Pengumpulan

| Dokumen | Isi |
| --- | --- |
| [docs/submission.md](docs/submission.md) | Daftar periksa dan isi paket pengumpulan |
| [docs/demo-script.md](docs/demo-script.md) | Naskah video demo 5 sampai 10 menit |

Paket pengumpulan dibuat dengan:

```bash
bash scripts/package.sh
```

Skrip membangun ulang dari awal, menjalankan seluruh uji, menolak berjalan
bila laporan belum diisi, lalu menyusun arsip di direktori `dist`.

Tanpa kernel hasil modifikasi, aplikasi tetap berjalan penuh. Program
hanya menampilkan bahwa syscall `ts_mark` tidak tersedia dan melewati
pencatatan ke kernel.

## Perintah Build

| Perintah | Fungsi |
| --- | --- |
| `bash build.sh` | Mengompilasi aplikasi melalui skrip build |
| `make` | Mengompilasi aplikasi |
| `make run` | Mengompilasi dan menjalankan aplikasi |
| `make test` | Menjalankan uji unit |
| `make check` | Menjalankan uji unit dan uji skenario |
| `make syscall-test` | Mengompilasi program uji syscall |
| `make clean` | Menghapus direktori hasil build |

## Pemecahan Masalah

- **`gcc` atau `make` tidak ditemukan:** instal paket `build-essential`.
- **Executable belum tersedia:** jalankan `bash build.sh` dan pastikan
  kompilasi selesai tanpa error.
- **Perintah tidak berjalan di Windows:** gunakan terminal Linux,
  misalnya Ubuntu di mesin virtual. Executable hasil build ditujukan
  untuk Linux.
- **Program langsung memakai nilai default:** masukan tidak berasal dari
  terminal, misalnya karena keluaran dialihkan. Berikan konfigurasi lewat
  opsi baris perintah.
- **Baris `Fitur kernel` menyebut syscall tidak ada:** kernel yang sedang
  berjalan belum dimodifikasi. Ikuti
  [docs/kernel-syscall.md](docs/kernel-syscall.md).

## Pembagian Tugas

| Tahap | Pelaksana | Pekerjaan |
| --- | --- | --- |
| 1 | Ivan Christian | Struktur proyek, README, skrip build |
| 2 | Thomas Seowinata | Antrean proses: menambah, mengambil, menghapus |
| 3 | Stephen Christopher | Proses anak dengan `fork()` dan workload terbatas |
| 4 | Nicko Richardo | Penyiapan Linux dan panduan build kernel |
| 5 | Ivan Christian | Masukan jumlah proses dan quantum dari terminal |
| 6 | Thomas Seowinata | Algoritma Round Robin |
| 7 | Stephen Christopher | `SIGSTOP`, `SIGCONT`, dan timer berbasis signal |
| 8 | Nicko Richardo | Modifikasi syscall, patch kernel, uji syscall |
| 9 | Ivan Christian | Integrasi masukan, penjadwal, proses, dan timer |
| 10 | Thomas Seowinata | Pengujian urutan giliran dan variasi jumlah proses |
| 11 | Stephen Christopher | Penanganan `Ctrl+C`, kegagalan proses, dan cleanup |
| 12 | Nicko Richardo | Menghubungkan syscall termodifikasi ke aplikasi |
| 13 | Ivan Christian | Laporan hasil dan pembaruan README |
| 14 | Semua | Pengujian akhir, video demo, dan paket pengumpulan |

# Time Sharing System

Time Sharing System v1.0.0 adalah simulasi Round Robin dalam bahasa C
untuk Linux. Aplikasi membuat proses anak dengan `fork()`, memberi
giliran berdasarkan antrean, dan memakai timer POSIX serta `SIGSTOP` dan
`SIGCONT` untuk mengatur jalannya proses.

| Bagian | Status |
| --- | --- |
| Aplikasi dan pengujian otomatis | Berhasil dibangun di Ubuntu WSL. Hasil `make check` terbaru yang dijalankan tim menunjukkan 14 dari 14 skenario lulus. |
| Integrasi kernel | Source dan program uji `ts_mark` tersedia. Bukti boot dan hasil uji pada kernel modifikasi masih perlu dicatat. |

## Mulai Cepat

Buka terminal Linux di folder proyek. Di Windows, buka Ubuntu WSL atau
gunakan Ubuntu di mesin virtual. Jika WSL membuka distro lain, cek nama
Ubuntu lewat `wsl -l -v` di PowerShell, lalu jalankan `wsl -d Ubuntu`
dengan nama yang terdaftar.

Jika GCC dan GNU Make belum tersedia di Ubuntu atau Debian, instal
`build-essential` terlebih dahulu.

```bash
sudo apt update
sudo apt install build-essential
```

```bash
bash build.sh
./build/timesharing -p 3 -q 50 -w 200
make check
```

Contoh tersebut menjalankan 3 proses, dengan giliran maksimum 50 ms dan
beban CPU 200 ms per proses. Pada keluaran, perhatikan urutan giliran
`1 → 2 → 3` dan ringkasan `Proses selesai: 3 dari 3`.

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
| Pengujian | GNU coreutils (`timeout`) dan GNU linker (`--wrap`) |

## Opsi Aplikasi

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

Kode keluar 0 menandakan simulasi berhasil. Kegagalan pembuatan proses,
timer, atau penjadwalan menghasilkan kode 1; proses anak yang masih hidup
dibersihkan saat simulasi dibatalkan. Penghentian lewat signal menghasilkan
kode 130.

## Cara Kerja Penjadwalan

1. Program membaca konfigurasi, lalu memasang handler signal dan timer.
2. Setiap proses anak dibuat dengan `fork()` dan langsung ditahan memakai
   `SIGSTOP` sampai gilirannya datang. Anak menghentikan dirinya sebelum
   memulai workload; parent menunggu konfirmasi `waitpid(..., WUNTRACED)`
   sebelum memasukkannya ke antrean.
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

| Baris | Cara membacanya |
| --- | --- |
| `Fitur kernel` | Menunjukkan apakah pencatatan lewat `ts_mark` tersedia. |
| `Giliran ...` | Menunjukkan proses aktif, PID, dan jatah waktu. |
| `[worker ...] progres` | Menunjukkan perkembangan beban CPU proses tersebut. |
| `Proses selesai` dan `Proses gagal` | Pada simulasi normal, semua proses selesai dan jumlah gagal bernilai 0. |
| `Giliran tercatat kernel` | Jumlah panggilan pencatatan giliran yang berhasil. Nilai 0 wajar ketika `ts_mark` tidak tersedia. |

Contoh keluaran lengkap ada di [laporan hasil eksekusi](docs/report.md).

## Pengujian

```bash
make test
```

Perintah di atas menjalankan uji unit antrean dan Round Robin: urutan
masuk dan keluar, penolakan pid duplikat dan antrean penuh, penghapusan
dari tengah antrean, perilaku buffer sirkular, kasus satu proses, kasus
banyak proses, serta proses yang selesai lebih awal dan harus dilewati.
Selain itu, uji worker memeriksa status berhenti sebelum giliran pertama,
penyelesaian setelah `SIGCONT`, dan cleanup tanpa zombie.

```bash
make check
```

Perintah ini menjalankan uji unit lalu menjalankan aplikasi pada beberapa
skenario: satu proses, empat proses, quantum lebih besar dari beban,
jumlah proses maksimum, quantum minimum, dan sejumlah masukan tidak valid.
Pengujian juga menyimulasikan kegagalan timer pada executable khusus untuk
memastikan kode keluar 1 dan tidak ada proses anak tersisa. Injeksi error
ini hanya ditautkan ke program uji, bukan ke `build/timesharing`.

Setiap kasus uji dibatasi 20 detik; proses yang belum berhenti diberi
`SIGKILL` setelah tambahan 2 detik. Keluaran kasus yang gagal ditampilkan
agar penyebabnya dapat diperiksa.

Untuk mengecek cleanup secara manual, jalankan simulasi dengan beban
panjang. Setelah baris `Giliran` muncul, tekan **Ctrl+C** lalu jalankan
`echo $?`. Aplikasi akan melaporkan bahwa proses anak sudah dibersihkan
dan mengembalikan kode `130`.

```bash
./build/timesharing -p 3 -q 100 -w 10000
```

## Laporan Hasil

Laporan yang tersimpan memuat log eksekusi 16 November 2026. Untuk
merekam keluaran lengkap dari versi saat ini, jalankan skrip berikut di
Linux.

```bash
bash scripts/run_report.sh
```

Skrip menjalankan aplikasi dan seluruh uji, lalu menulis hasilnya ke
[docs/report.md](docs/report.md) bersama catatan lingkungan.

## Bagian Kernel

Round Robin berjalan di aplikasi. Kernel modifikasi menyediakan syscall
`ts_mark` untuk mencatat giliran yang diberikan aplikasi. Setelah boot
ke kernel hasil build, jalankan uji syscall dan aplikasi dari folder proyek.

```bash
uname -r
make syscall-test
./build/ts_mark_test
./build/timesharing -p 3 -q 50 -w 200
sudo dmesg | grep ts_mark | tail -10
```

Hasil yang dicatat untuk demo adalah versi kernel, pesan
`Semua pemeriksaan syscall lulus`, jumlah giliran tercatat yang lebih
dari 0, dan entri `ts_mark` pada log kernel.

| Dokumen | Isi |
| --- | --- |
| [docs/kernel-setup.md](docs/kernel-setup.md) | Penyiapan VM, paket build, dan kompilasi kernel |
| [docs/kernel-syscall.md](docs/kernel-syscall.md) | Syscall `ts_mark`, patch, pengujian, dan integrasi |
| [docs/kernel-env.txt](docs/kernel-env.txt) | Catatan versi lingkungan yang dipakai |


Paket pengumpulan dibuat dengan:

```bash
bash scripts/package.sh
```

Skrip membangun ulang aplikasi dan menjalankan pengujian. Setelah
laporan dan catatan lingkungan diisi, skrip dapat membuat arsip di
direktori `dist`.

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
- **Perintah tidak berjalan di Windows.** Gunakan terminal Ubuntu WSL
  atau terminal Linux di mesin virtual.
- **Program langsung memakai nilai default.** Jika stdin berasal dari
  pipe atau file, tentukan konfigurasi lewat opsi baris perintah.
- **Baris `Fitur kernel` menyebut syscall tidak ada:** kernel yang sedang
  berjalan belum dimodifikasi. Ikuti
  [docs/kernel-syscall.md](docs/kernel-syscall.md).



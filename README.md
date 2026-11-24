# Time Sharing System

Simulasi Round Robin dalam bahasa C untuk Linux. Aplikasi membuat proses anak dengan `fork()` dan memberi tiap proses giliran selama satu *quantum*. Saat timer habis, aplikasi mengirim `SIGSTOP` ke proses aktif dan melanjutkan proses berikutnya dengan `SIGCONT`. Pada kernel proyek, syscall `ts_mark` mencatat PID dan jatah waktu setiap giliran.

## 1. Siapkan Ubuntu di Windows

Panduan ini memakai PC Windows 11 x86-64 (Intel atau AMD), WSL 2, koneksi internet, dan akun yang bisa menjalankan PowerShell sebagai Administrator. Gunakan **PowerShell** untuk perintah Windows dan **terminal Ubuntu** untuk perintah Bash.

Buka PowerShell sebagai Administrator. Jika Ubuntu belum terpasang, jalankan:

```powershell
wsl --install -d Ubuntu
```

Restart Windows jika diminta. Buka **Ubuntu** dari menu Start, lalu buat username dan password Linux saat diminta. Saat mengetik password, layar memang tidak menampilkan karakter. Di PowerShell, periksa distro:

```powershell
wsl -l -v
```

Pastikan baris Ubuntu menunjukkan VERSION **2**. Jika masih 1, jalankan `wsl --set-version Ubuntu 2` dan tunggu sampai selesai. Jika nama distro berbeda, gunakan nama yang muncul di daftar. Untuk membuka Ubuntu lagi, jalankan `wsl -d Ubuntu`.

## 2. Ambil proyek di terminal Ubuntu

```bash
sudo apt update && sudo apt install -y git build-essential
cd ~
git clone https://github.com/ivanchristian126/Project-OS.git
cd ~/Project-OS
ls build.sh Makefile
```

Kalau sudah punya salinan proyek, cukup masuk ke foldernya. Misalnya untuk folder Windows `C:\Projects\Project-OS`, gunakan `cd "/mnt/c/Projects/Project-OS"`. Perintah `ls` di atas harus menampilkan kedua file sebelum lanjut.

## 3. Pasang kernel yang menyediakan ts_mark

Aplikasi memanggil syscall nomor **454**. Pada kernel lain nomor itu bisa memiliki fungsi berbeda. Di Ubuntu, jalankan `uname -r`. Jika kamu sudah menggunakan kernel proyek yang menyediakan `ts_mark` pada nomor 454, lanjut ke langkah 4.

Untuk instalasi baru, ikuti [panduan pemasangan kernel proyek](docs/kernel-wsl.md) mulai dari folder proyek. Ikuti sampai image dan modul selesai dibangun, konfigurasi Windows disimpan, dan WSL dibuka kembali. Ganti nama profil Windows contoh `chris` dengan nama folder profil milikmu. Pengaturan kernel berlaku untuk semua distro WSL 2 di komputer tersebut.

Setelah restart WSL, periksa lagi `uname -r` di Ubuntu. Contoh hasil build proyek adalah `6.6.87.2-microsoft-standard-WSL2-tsmark+`. Jika masih tampil kernel bawaan, periksa kembali path image di `.wslconfig` sebelum menjalankan aplikasi.

## 4. Build dan jalankan simulasi pertama

Di terminal Ubuntu yang baru dibuka, masuk lagi ke folder proyek (sesuaikan perintah pertama jika tadi memakai folder lain):

```bash
cd ~/Project-OS
bash build.sh -B all syscall-test &&
./build/ts_mark_test &&
./build/timesharing -p 3 -q 50 -w 200
```

Program uji harus menampilkan `Semua pemeriksaan syscall lulus.` Setelah itu simulasi otomatis menjalankan **3 proses**, masing-masing dengan beban **200 ms waktu CPU** dan jatah **50 ms per giliran**. Simulasi berhasil jika ringkasannya menunjukkan `Proses selesai: 3 dari 3`, `Proses gagal: 0`, dan jumlah giliran tercatat kernel sesuai total giliran. Jika pengujian berhenti sebelum simulasi dimulai, periksa pesan pertama yang gagal dan pastikan langkah 3 sudah selesai.

Untuk menjalankan lagi dari folder proyek, cukup gunakan `./build/timesharing -p 3 -q 50 -w 200`.

## Mengatur simulasi

Jalankan `./build/timesharing` tanpa argumen di terminal untuk mengisi konfigurasi secara interaktif. Tekan Enter untuk menerima nilai bawaan.

| Opsi | Fungsi | Rentang | Bawaan |
| --- | --- | --- | --- |
| `-p`, `--processes` | Jumlah proses anak | 1–16 | 3 |
| `-q`, `--quantum` | Jatah waktu per giliran (ms) | 10–2000 | 100 |
| `-w`, `--workload` | Beban CPU per proses (ms) | 50–10000 | 500 |

Gunakan `--help` untuk bantuan atau `--version` untuk melihat versi. Tekan **Ctrl+C** untuk menghentikan simulasi dan membersihkan proses anak. Kode keluar dapat diperiksa dengan `echo $?` segera setelah program berhenti: 0 untuk sukses, 1 untuk kegagalan saat berjalan, 2 untuk input tidak valid, dan 130 untuk interupsi.

## Membaca hasil

Baris `Giliran` menunjukkan proses yang dipilih, PID-nya, dan jatah waktu. Untuk tiga proses, giliran awal berputar dari proses 1 ke 2, lalu 3, kemudian kembali ke 1 selama semuanya masih memiliki pekerjaan.

Cuplikan hasil dari contoh di atas pada kernel proyek:

```text
Total pergantian giliran: 12
Proses selesai: 3 dari 3
Proses gagal: 0
Giliran tercatat kernel: 12
Durasi simulasi: 602 ms
```

PID, jumlah giliran, dan durasi bisa berubah antar-eksekusi. Kolom `waktu(ms)` menjumlahkan lama giliran yang terukur oleh penjadwal. Kolom `selesai(ms)` menunjukkan waktu sejak simulasi dimulai sampai proses terdeteksi selesai.

## Menguji aplikasi dan syscall

Dari folder proyek, jalankan pengujian aplikasi:

```bash
make check
```

Hasil yang diharapkan adalah `14 dari 14 skenario lulus`. Untuk menguji syscall pada kernel proyek, bangun ulang kedua executable agar memakai nomor pada header saat ini:

```bash
make -B all syscall-test &&
./build/ts_mark_test &&
./build/timesharing -p 3 -q 50 -w 200
sudo dmesg | grep 'ts_mark:' | tail -10
```

Program uji menampilkan `Semua pemeriksaan syscall lulus.` Pada keluaran aplikasi, periksa `Giliran tercatat kernel`. Cocokkan PID dan jatah waktu di baris `Giliran` dengan `target` dan `slice` di log kernel. [Penjelasan syscall dan penghitung](docs/kernel-syscall.md).

Untuk memperbarui catatan lingkungan dan laporan dari mesin yang sedang digunakan:

```bash
bash scripts/collect-env.sh
bash scripts/run_report.sh
```

Hasilnya tersimpan di [docs/kernel-env.txt](docs/kernel-env.txt) dan [docs/report.md](docs/report.md).

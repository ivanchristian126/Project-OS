# Time Sharing System

Simulasi Round Robin dalam bahasa C untuk Linux. Aplikasi membuat proses anak dengan `fork()` dan memberi tiap proses giliran selama satu *quantum*. Saat timer habis, aplikasi mengirim `SIGSTOP` ke proses aktif dan melanjutkan proses berikutnya dengan `SIGCONT`. Pada kernel proyek, syscall `ts_mark` mencatat PID dan jatah waktu setiap giliran.

## Menjalankan di Ubuntu WSL 2

Di PowerShell, buka distro Ubuntu dengan `wsl -d Ubuntu`. Jika nama distronya berbeda, lihat daftar dengan `wsl -l -v`.

Di terminal Ubuntu, masuk ke folder yang berisi `build.sh`. Contoh untuk proyek yang tersimpan di drive C:

```bash
cd "/mnt/c/Users/chris/Documents/Project Cawu 6/OS"
uname -r
```

Kernel yang digunakan untuk panduan ini adalah `6.6.87.2-microsoft-standard-WSL2-tsmark+`, dengan `ts_mark` terdaftar di nomor **454**. Aplikasi memanggil nomor dalam [ts_syscall.h](include/ts_syscall.h) saat mulai berjalan. Jika memakai kernel lain, periksa pemetaan nomornya dahulu karena nomor yang sama dapat menunjuk ke syscall berbeda. Untuk memasang kernel proyek di WSL baru, ikuti [panduan kernel WSL 2](docs/kernel-wsl.md).

Bangun aplikasi dan jalankan simulasi pertama:

```bash
bash build.sh
./build/timesharing -p 3 -q 50 -w 200
```

Contoh ini menjalankan **3 proses**, masing-masing dengan beban **200 ms waktu CPU**. Setiap giliran mendapat jatah hingga **50 ms**. Jika GCC atau Make belum terpasang, jalankan `sudo apt update && sudo apt install build-essential`, lalu ulangi build.

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

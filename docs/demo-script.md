# Naskah Video Demo

Target durasi 5 sampai 10 menit. Rekam di VM Linux yang sudah memakai
kernel hasil modifikasi. Gunakan satu jendela terminal ukuran besar agar
teks terbaca.

## Persiapan Rekaman

```bash
bash build.sh
make test
clear
```

Perekam yang dapat dipakai: SimpleScreenRecorder, OBS Studio, atau
`gnome-screen-recorder`. Resolusi 1280x720 sudah cukup. Pastikan mikrofon
aktif bila narasi dibawakan langsung.

## Susunan Isi

### 1. Pembukaan, 0:00 sampai 0:40

Sebutkan nama kelompok dan anggota, judul proyek, serta tujuan: simulasi
time sharing dengan proses nyata, Round Robin, timer signal, dan syscall
kernel tambahan. Tampilkan `README.md` sekilas.

### 2. Struktur Proyek, 0:40 sampai 1:30

```bash
ls include src src/scheduler src/runtime src/kernel_bridge kernel tests
```

Jelaskan pembagian modul: antrean proses, penjadwal Round Robin, proses
anak dan signal, penghubung syscall. Sebutkan siapa mengerjakan bagian
mana sesuai tabel pembagian tugas di README.

### 3. Kompilasi dan Pengujian, 1:30 sampai 2:40

```bash
bash build.sh
make check
```

Tunjukkan semua baris `PASS`. Sebutkan cakupan pengujian: urutan giliran,
kasus satu proses, banyak proses, proses selesai lebih awal, serta
penolakan masukan tidak valid.

### 4. Masukan dari Terminal, 2:40 sampai 3:30

Jalankan tanpa opsi, isi jumlah proses dan quantum lewat prompt:

```bash
./build/timesharing
```

Lanjutkan dengan masukan yang salah untuk menunjukkan validasi:

```bash
./build/timesharing -p 0
./build/timesharing -q abc
```

### 5. Simulasi Round Robin, 3:30 sampai 5:00

```bash
./build/timesharing -p 4 -q 50 -w 300
```

Tunjuk baris `Giliran` dan jelaskan bahwa setiap proses dilanjutkan dengan
`SIGCONT`, lalu ditahan kembali dengan `SIGSTOP` saat quantum habis.
Tunjuk baris progres tiap worker, lalu bahas tabel laporan akhir: jumlah
giliran, waktu yang diberikan, dan urutan selesai.

Tambahan singkat untuk memperlihatkan pengaruh quantum:

```bash
./build/timesharing -p 4 -q 200 -w 300
```

Bandingkan jumlah pergantian giliran pada kedua konfigurasi.

### 6. Penghentian dan Cleanup, 5:00 sampai 6:00

Jalankan simulasi panjang, tekan `Ctrl+C` di tengah jalan:

```bash
./build/timesharing -p 8 -q 100 -w 2000
```

Setelah program berhenti, buktikan tidak ada proses tertinggal:

```bash
ps -ef | grep timesharing | grep -v grep
```

### 7. Syscall Kernel, 6:00 sampai 7:30

```bash
uname -r
make syscall-test
./build/ts_mark_test
sudo dmesg | tail -20
```

Tunjukkan bahwa `uname -r` memakai kernel hasil modifikasi, program uji
lulus, dan log kernel memuat baris `ts_mark`. Jalankan aplikasi sekali
lagi, lalu tunjuk baris `Fitur kernel` dan `Giliran tercatat kernel` pada
laporan, dan cocokkan angkanya dengan `dmesg`.

Sebutkan juga perilaku pada kernel biasa: aplikasi tetap jalan dan hanya
memberi pesan bahwa syscall tidak tersedia.

### 8. Penutup, 7:30 sampai 8:00

Tampilkan `docs/report.md` sebagai bukti hasil eksekusi, sebutkan isi
paket pengumpulan, lalu tutup dengan pembagian tugas kelompok.

## Catatan

- Jangan memotong bagian kompilasi, biarkan terlihat tanpa error.
- Bila satu perintah gagal saat rekaman, ulangi dari bagian itu.
- Simpan hasil rekaman di luar repository.

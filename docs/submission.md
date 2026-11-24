# Daftar Periksa Pengumpulan

Jalankan langkah berikut dari folder proyek di Ubuntu setelah boot ke
kernel proyek. Untuk penyiapan awal, ikuti [panduan WSL 2](kernel-wsl.md).

## 1. Catatan Lingkungan

```bash
bash scripts/collect-env.sh
```

Isi bagian `KERNEL_SOURCE` pada `docs/kernel-env.txt` dengan URL dan versi
source kernel yang diunduh, lalu commit file tersebut.

## 2. Build dan Pengujian

```bash
bash build.sh
make check
```

Seluruh baris harus menunjukkan `PASS`. Perbaiki lebih dulu bila ada
`FAIL`, jangan dilanjutkan.

## 3. Bagian Kernel

Periksa versi yang aktif dan jalankan [uji syscall](kernel-syscall.md):

```bash
uname -r
make -B all syscall-test
./build/ts_mark_test
sudo dmesg | tail -20
```

Commit file patch yang terbentuk di `kernel/patches/` dan perubahan
`TS_MARK_SYSCALL_NR` bila nomornya berubah.

## 4. Laporan Hasil

Jalankan pada kernel hasil modifikasi agar baris pencatatan kernel ikut
terekam:

```bash
bash scripts/run_report.sh
```

Periksa `docs/report.md`, pastikan tidak ada lagi tulisan BELUM DIISI,
lalu commit.

## 5. Video Demo

Durasi 5 sampai 10 menit mengikuti [docs/demo-script.md](demo-script.md).
Simpan di luar repository dan unggah sesuai ketentuan tugas.

## 6. Paket Pengumpulan

```bash
bash scripts/package.sh
```

Skrip menolak membuat paket bila ada uji yang gagal atau laporan belum
diisi. Hasilnya berupa `dist/time-sharing-system-<versi>-<tanggal>.tar.gz`
beserta `.zip` bila perintah `zip` tersedia.

## Isi Paket

| Bagian | Keterangan |
| --- | --- |
| `include`, `src` | Kode sumber aplikasi |
| `kernel` | Sumber syscall, skrip patch, program uji syscall |
| `tests`, `scripts` | Uji unit, uji skenario, skrip pendukung |
| `docs` | Panduan kernel, catatan lingkungan, laporan hasil |
| `build/timesharing` | Executable Linux hasil kompilasi |
| `build/ts_mark_test` | Executable uji syscall |
| `Makefile`, `build.sh`, `README.md` | Build dan dokumentasi |

## Pemeriksaan Terakhir

- [ ] `make check` lulus seluruhnya
- [ ] `docs/report.md` berisi hasil eksekusi nyata
- [ ] `docs/kernel-env.txt` berisi versi kernel dan compiler yang dipakai
- [ ] `kernel/patches/` memuat file patch hasil `apply-ts-mark.sh`
- [ ] `./build/ts_mark_test` lulus pada kernel hasil modifikasi
- [ ] Tidak ada proses anak tertinggal setelah `Ctrl+C`, periksa dengan
      `ps -ef | grep timesharing`
- [ ] Video demo berdurasi 5 sampai 10 menit
- [ ] Paket `dist` dibuat ulang setelah commit terakhir

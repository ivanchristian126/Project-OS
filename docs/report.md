# Laporan Hasil Eksekusi

File ini dibuat oleh scripts/run_report.sh dari eksekusi nyata.
Jalankan ulang skrip setelah ada perubahan kode.

Baris pencatatan kernel hanya terisi bila skrip dijalankan pada
kernel hasil modifikasi. Lihat docs/kernel-syscall.md.

## Lingkungan Pengujian

| Keterangan | Nilai |
| --- | --- |
| Waktu | 2026-11-16T11:42:57+07:00 |
| Host | DESKTOP-8F3F4VV |
| Kernel | 6.6.87.2-microsoft-standard-WSL2 |
| Arsitektur | x86_64 |
| Jumlah inti | 12 |
| Compiler | gcc (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0 |
| Versi aplikasi | Time Sharing System v1.0.0 |

## Skenario Simulasi

### Satu proses

Perintah:

```bash
./build/timesharing -p 1 -q 100 -w 200
```

Keluaran:

```text
Time Sharing System v1.0.0
Konfigurasi: 1 proses, quantum 100 ms, beban 200 ms per proses.
Fitur kernel: syscall ts_mark nomor 462 tidak ada pada kernel ini
Pencatatan ke kernel dilewati, simulasi tetap berjalan.
1 proses anak dibuat, penjadwalan Round Robin dimulai.
Tekan Ctrl+C untuk menghentikan simulasi lebih awal.
Giliran 1: proses 1 (pid 386) jatah 100 ms, sisa 200 ms
    [worker 1] pid 386 mulai, beban 200 ms
    [worker 1] pid 386 progres 20%
    [worker 1] pid 386 progres 40%
Giliran 2: proses 1 (pid 386) jatah 100 ms, sisa 100 ms
    [worker 1] pid 386 progres 60%
    [worker 1] pid 386 progres 80%
Giliran 3: proses 1 (pid 386) jatah 100 ms, sisa 0 ms
    [worker 1] pid 386 progres 100%
    [worker 1] pid 386 beban selesai
  proses 1 selesai setelah 3 giliran

Laporan hasil simulasi
proses  pid      giliran  waktu(ms)  selesai(ms)  status
1       386      3        203        205          selesai
Total pergantian giliran: 3
Proses selesai: 1 dari 1
Proses gagal: 0
Giliran tercatat kernel: 0
Durasi simulasi: 206 ms
```

Kode keluar: 0

### Tiga proses dengan nilai default

Perintah:

```bash
./build/timesharing -p 3 -q 100 -w 500
```

Keluaran:

```text
Time Sharing System v1.0.0
Konfigurasi: 3 proses, quantum 100 ms, beban 500 ms per proses.
Fitur kernel: syscall ts_mark nomor 462 tidak ada pada kernel ini
Pencatatan ke kernel dilewati, simulasi tetap berjalan.
3 proses anak dibuat, penjadwalan Round Robin dimulai.
Tekan Ctrl+C untuk menghentikan simulasi lebih awal.
Giliran 1: proses 1 (pid 388) jatah 100 ms, sisa 500 ms
    [worker 1] pid 388 mulai, beban 500 ms
    [worker 1] pid 388 progres 20%
Giliran 2: proses 2 (pid 389) jatah 100 ms, sisa 500 ms
    [worker 2] pid 389 mulai, beban 500 ms
Giliran 3: proses 3 (pid 390) jatah 100 ms, sisa 500 ms
    [worker 3] pid 390 mulai, beban 500 ms
Giliran 4: proses 1 (pid 388) jatah 100 ms, sisa 400 ms
    [worker 1] pid 388 progres 40%
Giliran 5: proses 2 (pid 389) jatah 100 ms, sisa 400 ms
    [worker 2] pid 389 progres 20%
Giliran 6: proses 3 (pid 390) jatah 100 ms, sisa 400 ms
    [worker 3] pid 390 progres 20%
Giliran 7: proses 1 (pid 388) jatah 100 ms, sisa 300 ms
    [worker 1] pid 388 progres 60%
Giliran 8: proses 2 (pid 389) jatah 100 ms, sisa 300 ms
    [worker 2] pid 389 progres 40%
Giliran 9: proses 3 (pid 390) jatah 100 ms, sisa 300 ms
    [worker 3] pid 390 progres 40%
Giliran 10: proses 1 (pid 388) jatah 100 ms, sisa 200 ms
    [worker 1] pid 388 progres 80%
Giliran 11: proses 2 (pid 389) jatah 100 ms, sisa 200 ms
    [worker 2] pid 389 progres 60%
Giliran 12: proses 3 (pid 390) jatah 100 ms, sisa 200 ms
    [worker 3] pid 390 progres 60%
Giliran 13: proses 1 (pid 388) jatah 100 ms, sisa 100 ms
    [worker 1] pid 388 progres 100%
Giliran 14: proses 2 (pid 389) jatah 100 ms, sisa 100 ms
    [worker 2] pid 389 progres 80%
Giliran 15: proses 3 (pid 390) jatah 100 ms, sisa 100 ms
    [worker 3] pid 390 progres 80%
Giliran 16: proses 1 (pid 388) jatah 100 ms, sisa 0 ms
    [worker 1] pid 388 beban selesai
  proses 1 selesai setelah 6 giliran
Giliran 17: proses 2 (pid 389) jatah 100 ms, sisa 0 ms
    [worker 2] pid 389 progres 100%
    [worker 2] pid 389 beban selesai
  proses 2 selesai setelah 6 giliran
Giliran 18: proses 3 (pid 390) jatah 100 ms, sisa 0 ms
    [worker 3] pid 390 progres 100%
    [worker 3] pid 390 beban selesai
  proses 3 selesai setelah 6 giliran

Laporan hasil simulasi
proses  pid      giliran  waktu(ms)  selesai(ms)  status
1       388      6        501        1512         selesai
2       389      6        502        1514         selesai
3       390      6        503        1517         selesai
Total pergantian giliran: 18
Proses selesai: 3 dari 3
Proses gagal: 0
Giliran tercatat kernel: 0
Durasi simulasi: 1517 ms
```

Kode keluar: 0

### Empat proses dengan quantum kecil

Perintah:

```bash
./build/timesharing -p 4 -q 20 -w 200
```

Keluaran:

```text
Time Sharing System v1.0.0
Konfigurasi: 4 proses, quantum 20 ms, beban 200 ms per proses.
Fitur kernel: syscall ts_mark nomor 462 tidak ada pada kernel ini
Pencatatan ke kernel dilewati, simulasi tetap berjalan.
4 proses anak dibuat, penjadwalan Round Robin dimulai.
Tekan Ctrl+C untuk menghentikan simulasi lebih awal.
Giliran 1: proses 1 (pid 394) jatah 20 ms, sisa 200 ms
    [worker 1] pid 394 mulai, beban 200 ms
Giliran 2: proses 2 (pid 395) jatah 20 ms, sisa 200 ms
    [worker 2] pid 395 mulai, beban 200 ms
Giliran 3: proses 3 (pid 396) jatah 20 ms, sisa 200 ms
    [worker 3] pid 396 mulai, beban 200 ms
Giliran 4: proses 4 (pid 397) jatah 20 ms, sisa 200 ms
    [worker 4] pid 397 mulai, beban 200 ms
Giliran 5: proses 1 (pid 394) jatah 20 ms, sisa 180 ms
Giliran 6: proses 2 (pid 395) jatah 20 ms, sisa 180 ms
Giliran 7: proses 3 (pid 396) jatah 20 ms, sisa 180 ms
Giliran 8: proses 4 (pid 397) jatah 20 ms, sisa 180 ms
    [worker 4] pid 397 progres 20%
Giliran 9: proses 1 (pid 394) jatah 20 ms, sisa 160 ms
    [worker 1] pid 394 progres 20%
Giliran 10: proses 2 (pid 395) jatah 20 ms, sisa 160 ms
    [worker 2] pid 395 progres 20%
Giliran 11: proses 3 (pid 396) jatah 20 ms, sisa 160 ms
    [worker 3] pid 396 progres 20%
Giliran 12: proses 4 (pid 397) jatah 20 ms, sisa 160 ms
Giliran 13: proses 1 (pid 394) jatah 20 ms, sisa 140 ms
    [worker 1] pid 394 progres 40%
Giliran 14: proses 2 (pid 395) jatah 20 ms, sisa 140 ms
Giliran 15: proses 3 (pid 396) jatah 20 ms, sisa 137 ms
    [worker 3] pid 396 progres 40%
Giliran 16: proses 4 (pid 397) jatah 20 ms, sisa 140 ms
    [worker 4] pid 397 progres 40%
Giliran 17: proses 1 (pid 394) jatah 20 ms, sisa 120 ms
Giliran 18: proses 2 (pid 395) jatah 20 ms, sisa 120 ms
    [worker 2] pid 395 progres 40%
Giliran 19: proses 3 (pid 396) jatah 20 ms, sisa 117 ms
Giliran 20: proses 4 (pid 397) jatah 20 ms, sisa 120 ms
Giliran 21: proses 1 (pid 394) jatah 20 ms, sisa 100 ms
    [worker 1] pid 394 progres 60%
Giliran 22: proses 2 (pid 395) jatah 20 ms, sisa 100 ms
Giliran 23: proses 3 (pid 396) jatah 20 ms, sisa 97 ms
    [worker 3] pid 396 progres 60%
Giliran 24: proses 4 (pid 397) jatah 20 ms, sisa 100 ms
    [worker 4] pid 397 progres 60%
Giliran 25: proses 1 (pid 394) jatah 20 ms, sisa 80 ms
Giliran 26: proses 2 (pid 395) jatah 20 ms, sisa 80 ms
    [worker 2] pid 395 progres 60%
Giliran 27: proses 3 (pid 396) jatah 20 ms, sisa 77 ms
Giliran 28: proses 4 (pid 397) jatah 20 ms, sisa 80 ms
Giliran 29: proses 1 (pid 394) jatah 20 ms, sisa 60 ms
    [worker 1] pid 394 progres 80%
Giliran 30: proses 2 (pid 395) jatah 20 ms, sisa 60 ms
Giliran 31: proses 3 (pid 396) jatah 20 ms, sisa 57 ms
    [worker 3] pid 396 progres 80%
Giliran 32: proses 4 (pid 397) jatah 20 ms, sisa 60 ms
    [worker 4] pid 397 progres 80%
Giliran 33: proses 1 (pid 394) jatah 20 ms, sisa 40 ms
Giliran 34: proses 2 (pid 395) jatah 20 ms, sisa 40 ms
    [worker 2] pid 395 progres 80%
Giliran 35: proses 3 (pid 396) jatah 20 ms, sisa 37 ms
Giliran 36: proses 4 (pid 397) jatah 20 ms, sisa 40 ms
Giliran 37: proses 1 (pid 394) jatah 20 ms, sisa 20 ms
    [worker 1] pid 394 progres 100%
Giliran 38: proses 2 (pid 395) jatah 20 ms, sisa 20 ms
Giliran 39: proses 3 (pid 396) jatah 17 ms, sisa 17 ms
Giliran 40: proses 4 (pid 397) jatah 20 ms, sisa 20 ms
    [worker 4] pid 397 progres 100%
Giliran 41: proses 1 (pid 394) jatah 20 ms, sisa 0 ms
    [worker 1] pid 394 beban selesai
  proses 1 selesai setelah 11 giliran
Giliran 42: proses 2 (pid 395) jatah 20 ms, sisa 0 ms
    [worker 2] pid 395 progres 100%
    [worker 2] pid 395 beban selesai
  proses 2 selesai setelah 11 giliran
Giliran 43: proses 3 (pid 396) jatah 20 ms, sisa 0 ms
    [worker 3] pid 396 progres 100%
    [worker 3] pid 396 beban selesai
  proses 3 selesai setelah 11 giliran
Giliran 44: proses 4 (pid 397) jatah 20 ms, sisa 0 ms
    [worker 4] pid 397 beban selesai
  proses 4 selesai setelah 11 giliran

Laporan hasil simulasi
proses  pid      giliran  waktu(ms)  selesai(ms)  status
1       394      11       202        829          selesai
2       395      11       203        832          selesai
3       396      11       203        834          selesai
4       397      11       202        835          selesai
Total pergantian giliran: 44
Proses selesai: 4 dari 4
Proses gagal: 0
Giliran tercatat kernel: 0
Durasi simulasi: 835 ms
```

Kode keluar: 0

### Enam belas proses

Perintah:

```bash
./build/timesharing -p 16 -q 20 -w 60
```

Keluaran:

```text
Time Sharing System v1.0.0
Konfigurasi: 16 proses, quantum 20 ms, beban 60 ms per proses.
Fitur kernel: syscall ts_mark nomor 462 tidak ada pada kernel ini
Pencatatan ke kernel dilewati, simulasi tetap berjalan.
16 proses anak dibuat, penjadwalan Round Robin dimulai.
Tekan Ctrl+C untuk menghentikan simulasi lebih awal.
Giliran 1: proses 1 (pid 399) jatah 20 ms, sisa 60 ms
    [worker 1] pid 399 mulai, beban 60 ms
    [worker 1] pid 399 progres 20%
Giliran 2: proses 2 (pid 400) jatah 20 ms, sisa 60 ms
    [worker 2] pid 400 mulai, beban 60 ms
    [worker 2] pid 400 progres 20%
Giliran 3: proses 3 (pid 401) jatah 20 ms, sisa 60 ms
    [worker 3] pid 401 mulai, beban 60 ms
    [worker 3] pid 401 progres 20%
Giliran 4: proses 4 (pid 402) jatah 20 ms, sisa 60 ms
    [worker 4] pid 402 mulai, beban 60 ms
    [worker 4] pid 402 progres 20%
Giliran 5: proses 5 (pid 403) jatah 20 ms, sisa 60 ms
    [worker 5] pid 403 mulai, beban 60 ms
    [worker 5] pid 403 progres 20%
Giliran 6: proses 6 (pid 404) jatah 20 ms, sisa 60 ms
    [worker 6] pid 404 mulai, beban 60 ms
    [worker 6] pid 404 progres 20%
Giliran 7: proses 7 (pid 405) jatah 20 ms, sisa 60 ms
    [worker 7] pid 405 mulai, beban 60 ms
    [worker 7] pid 405 progres 20%
Giliran 8: proses 8 (pid 406) jatah 20 ms, sisa 60 ms
    [worker 8] pid 406 mulai, beban 60 ms
    [worker 8] pid 406 progres 20%
Giliran 9: proses 9 (pid 407) jatah 20 ms, sisa 60 ms
    [worker 9] pid 407 mulai, beban 60 ms
    [worker 9] pid 407 progres 20%
Giliran 10: proses 10 (pid 408) jatah 20 ms, sisa 60 ms
    [worker 10] pid 408 mulai, beban 60 ms
    [worker 10] pid 408 progres 20%
Giliran 11: proses 11 (pid 409) jatah 20 ms, sisa 60 ms
    [worker 11] pid 409 mulai, beban 60 ms
    [worker 11] pid 409 progres 20%
Giliran 12: proses 12 (pid 410) jatah 20 ms, sisa 60 ms
    [worker 12] pid 410 mulai, beban 60 ms
    [worker 12] pid 410 progres 20%
Giliran 13: proses 13 (pid 411) jatah 20 ms, sisa 60 ms
    [worker 13] pid 411 mulai, beban 60 ms
    [worker 13] pid 411 progres 20%
Giliran 14: proses 14 (pid 412) jatah 20 ms, sisa 60 ms
    [worker 14] pid 412 mulai, beban 60 ms
    [worker 14] pid 412 progres 20%
Giliran 15: proses 15 (pid 413) jatah 20 ms, sisa 60 ms
    [worker 15] pid 413 mulai, beban 60 ms
    [worker 15] pid 413 progres 20%
Giliran 16: proses 16 (pid 414) jatah 20 ms, sisa 60 ms
    [worker 16] pid 414 mulai, beban 60 ms
    [worker 16] pid 414 progres 20%
Giliran 17: proses 1 (pid 399) jatah 20 ms, sisa 40 ms
    [worker 1] pid 399 progres 40%
    [worker 1] pid 399 progres 60%
Giliran 18: proses 2 (pid 400) jatah 20 ms, sisa 40 ms
    [worker 2] pid 400 progres 40%
    [worker 2] pid 400 progres 60%
Giliran 19: proses 3 (pid 401) jatah 20 ms, sisa 40 ms
    [worker 3] pid 401 progres 40%
    [worker 3] pid 401 progres 60%
Giliran 20: proses 4 (pid 402) jatah 20 ms, sisa 40 ms
    [worker 4] pid 402 progres 40%
    [worker 4] pid 402 progres 60%
Giliran 21: proses 5 (pid 403) jatah 20 ms, sisa 40 ms
    [worker 5] pid 403 progres 40%
    [worker 5] pid 403 progres 60%
Giliran 22: proses 6 (pid 404) jatah 20 ms, sisa 40 ms
    [worker 6] pid 404 progres 40%
    [worker 6] pid 404 progres 60%
Giliran 23: proses 7 (pid 405) jatah 20 ms, sisa 40 ms
    [worker 7] pid 405 progres 40%
    [worker 7] pid 405 progres 60%
Giliran 24: proses 8 (pid 406) jatah 20 ms, sisa 40 ms
    [worker 8] pid 406 progres 40%
    [worker 8] pid 406 progres 60%
Giliran 25: proses 9 (pid 407) jatah 20 ms, sisa 40 ms
    [worker 9] pid 407 progres 40%
    [worker 9] pid 407 progres 60%
Giliran 26: proses 10 (pid 408) jatah 20 ms, sisa 40 ms
    [worker 10] pid 408 progres 40%
    [worker 10] pid 408 progres 60%
Giliran 27: proses 11 (pid 409) jatah 20 ms, sisa 40 ms
    [worker 11] pid 409 progres 40%
    [worker 11] pid 409 progres 60%
Giliran 28: proses 12 (pid 410) jatah 20 ms, sisa 40 ms
    [worker 12] pid 410 progres 40%
    [worker 12] pid 410 progres 60%
Giliran 29: proses 13 (pid 411) jatah 20 ms, sisa 40 ms
    [worker 13] pid 411 progres 40%
    [worker 13] pid 411 progres 60%
Giliran 30: proses 14 (pid 412) jatah 20 ms, sisa 40 ms
    [worker 14] pid 412 progres 40%
    [worker 14] pid 412 progres 60%
Giliran 31: proses 15 (pid 413) jatah 20 ms, sisa 40 ms
    [worker 15] pid 413 progres 40%
    [worker 15] pid 413 progres 60%
Giliran 32: proses 16 (pid 414) jatah 20 ms, sisa 40 ms
    [worker 16] pid 414 progres 40%
    [worker 16] pid 414 progres 60%
Giliran 33: proses 1 (pid 399) jatah 20 ms, sisa 20 ms
    [worker 1] pid 399 progres 80%
Giliran 34: proses 2 (pid 400) jatah 20 ms, sisa 20 ms
    [worker 2] pid 400 progres 80%
Giliran 35: proses 3 (pid 401) jatah 20 ms, sisa 20 ms
    [worker 3] pid 401 progres 80%
Giliran 36: proses 4 (pid 402) jatah 20 ms, sisa 20 ms
    [worker 4] pid 402 progres 80%
Giliran 37: proses 5 (pid 403) jatah 20 ms, sisa 20 ms
    [worker 5] pid 403 progres 80%
Giliran 38: proses 6 (pid 404) jatah 20 ms, sisa 20 ms
    [worker 6] pid 404 progres 80%
Giliran 39: proses 7 (pid 405) jatah 20 ms, sisa 20 ms
    [worker 7] pid 405 progres 80%
Giliran 40: proses 8 (pid 406) jatah 20 ms, sisa 20 ms
    [worker 8] pid 406 progres 80%
Giliran 41: proses 9 (pid 407) jatah 20 ms, sisa 20 ms
    [worker 9] pid 407 progres 80%
Giliran 42: proses 10 (pid 408) jatah 20 ms, sisa 20 ms
    [worker 10] pid 408 progres 80%
Giliran 43: proses 11 (pid 409) jatah 20 ms, sisa 20 ms
    [worker 11] pid 409 progres 80%
Giliran 44: proses 12 (pid 410) jatah 20 ms, sisa 20 ms
    [worker 12] pid 410 progres 80%
Giliran 45: proses 13 (pid 411) jatah 20 ms, sisa 20 ms
    [worker 13] pid 411 progres 80%
Giliran 46: proses 14 (pid 412) jatah 20 ms, sisa 20 ms
    [worker 14] pid 412 progres 80%
Giliran 47: proses 15 (pid 413) jatah 20 ms, sisa 20 ms
    [worker 15] pid 413 progres 80%
Giliran 48: proses 16 (pid 414) jatah 20 ms, sisa 20 ms
    [worker 16] pid 414 progres 80%
Giliran 49: proses 1 (pid 399) jatah 20 ms, sisa 0 ms
    [worker 1] pid 399 progres 100%
    [worker 1] pid 399 beban selesai
  proses 1 selesai setelah 4 giliran
Giliran 50: proses 2 (pid 400) jatah 20 ms, sisa 0 ms
    [worker 2] pid 400 progres 100%
    [worker 2] pid 400 beban selesai
  proses 2 selesai setelah 4 giliran
Giliran 51: proses 3 (pid 401) jatah 20 ms, sisa 0 ms
    [worker 3] pid 401 progres 100%
    [worker 3] pid 401 beban selesai
  proses 3 selesai setelah 4 giliran
Giliran 52: proses 4 (pid 402) jatah 20 ms, sisa 0 ms
    [worker 4] pid 402 progres 100%
    [worker 4] pid 402 beban selesai
  proses 4 selesai setelah 4 giliran
Giliran 53: proses 5 (pid 403) jatah 20 ms, sisa 0 ms
    [worker 5] pid 403 progres 100%
    [worker 5] pid 403 beban selesai
  proses 5 selesai setelah 4 giliran
Giliran 54: proses 6 (pid 404) jatah 20 ms, sisa 0 ms
    [worker 6] pid 404 progres 100%
    [worker 6] pid 404 beban selesai
  proses 6 selesai setelah 4 giliran
Giliran 55: proses 7 (pid 405) jatah 20 ms, sisa 0 ms
    [worker 7] pid 405 progres 100%
    [worker 7] pid 405 beban selesai
  proses 7 selesai setelah 4 giliran
Giliran 56: proses 8 (pid 406) jatah 20 ms, sisa 0 ms
    [worker 8] pid 406 progres 100%
    [worker 8] pid 406 beban selesai
  proses 8 selesai setelah 4 giliran
Giliran 57: proses 9 (pid 407) jatah 20 ms, sisa 0 ms
    [worker 9] pid 407 progres 100%
    [worker 9] pid 407 beban selesai
  proses 9 selesai setelah 4 giliran
Giliran 58: proses 10 (pid 408) jatah 20 ms, sisa 0 ms
    [worker 10] pid 408 progres 100%
    [worker 10] pid 408 beban selesai
  proses 10 selesai setelah 4 giliran
Giliran 59: proses 11 (pid 409) jatah 20 ms, sisa 0 ms
    [worker 11] pid 409 progres 100%
    [worker 11] pid 409 beban selesai
  proses 11 selesai setelah 4 giliran
Giliran 60: proses 12 (pid 410) jatah 20 ms, sisa 0 ms
    [worker 12] pid 410 progres 100%
    [worker 12] pid 410 beban selesai
  proses 12 selesai setelah 4 giliran
Giliran 61: proses 13 (pid 411) jatah 20 ms, sisa 0 ms
    [worker 13] pid 411 progres 100%
    [worker 13] pid 411 beban selesai
  proses 13 selesai setelah 4 giliran
Giliran 62: proses 14 (pid 412) jatah 20 ms, sisa 0 ms
    [worker 14] pid 412 progres 100%
    [worker 14] pid 412 beban selesai
  proses 14 selesai setelah 4 giliran
Giliran 63: proses 15 (pid 413) jatah 20 ms, sisa 0 ms
    [worker 15] pid 413 progres 100%
    [worker 15] pid 413 beban selesai
  proses 15 selesai setelah 4 giliran
Giliran 64: proses 16 (pid 414) jatah 20 ms, sisa 0 ms
    [worker 16] pid 414 progres 100%
    [worker 16] pid 414 beban selesai
  proses 16 selesai setelah 4 giliran

Laporan hasil simulasi
proses  pid      giliran  waktu(ms)  selesai(ms)  status
1       399      4        63         989          selesai
2       400      4        63         991          selesai
3       401      4        63         994          selesai
4       402      4        62         997          selesai
5       403      4        63         1000         selesai
6       404      4        62         1002         selesai
7       405      4        62         1005         selesai
8       406      4        62         1007         selesai
9       407      4        63         1010         selesai
10      408      4        62         1013         selesai
11      409      4        63         1016         selesai
12      410      4        62         1018         selesai
13      411      4        63         1021         selesai
14      412      4        62         1023         selesai
15      413      4        62         1026         selesai
16      414      4        62         1028         selesai
Total pergantian giliran: 64
Proses selesai: 16 dari 16
Proses gagal: 0
Giliran tercatat kernel: 0
Durasi simulasi: 1029 ms
```

Kode keluar: 0

## Penolakan Masukan Tidak Valid

### Jumlah proses nol

Perintah:

```bash
./build/timesharing -p 0
```

Keluaran:

```text
Jumlah proses tidak valid: 0. Harus bilangan bulat 1-16.
Time Sharing System v1.0.0
```

Kode keluar: 2

### Quantum bukan angka

Perintah:

```bash
./build/timesharing -q abc
```

Keluaran:

```text
Quantum (ms) tidak valid: abc. Harus bilangan bulat 10-2000.
Time Sharing System v1.0.0
```

Kode keluar: 2

### Opsi tidak dikenal

Perintah:

```bash
./build/timesharing --tidak-ada
```

Keluaran:

```text
Opsi tidak dikenal: --tidak-ada.
Jalankan ./build/timesharing --help untuk daftar opsi.
Time Sharing System v1.0.0
```

Kode keluar: 2

## Hasil Pengujian

### Uji unit antrean dan penjadwal

Perintah:

```bash
make --no-print-directory test
```

Keluaran:

```text
./build/test_queue
Uji antrean proses dan penjadwalan Round Robin
PASS antrean memuat tiga proses
PASS proses keluar sesuai urutan masuk
PASS antrean kosong setelah semua diambil
PASS pid valid diterima
PASS pid duplikat ditolak
PASS pid nol ditolak
PASS antrean terisi penuh
PASS antrean penuh menolak proses baru
PASS proses di tengah dapat dihapus
PASS pid yang sudah dihapus tidak ditemukan
PASS pid terhapus tidak ada di antrean
PASS ukuran antrean berkurang satu
PASS urutan proses lain tetap terjaga
PASS antrean sirkular terisi kembali setelah berputar
PASS entri terdepan benar setelah berputar
PASS satu proses masuk antrean siap
PASS beban 250 ms dengan quantum 100 ms butuh 3 giliran
PASS antrean kosong setelah proses tunggal selesai
PASS satu proses tercatat selesai
PASS giliran berputar sesuai urutan antrean
PASS dua belas pergantian giliran tercatat
PASS empat proses masih menunggu giliran
PASS giliran kedua dan ketiga sesuai urutan
PASS proses yang selesai tidak kembali ke antrean
PASS dua proses tersisa di antrean
PASS proses selesai dilewati pada putaran berikutnya
PASS sisa kecil memakai jatah sisa
PASS sisa besar memakai satu quantum
PASS sisa waktu tidak pernah negatif

29 dari 29 pemeriksaan lulus
```

Kode keluar: 0

### Uji skenario aplikasi

Perintah:

```bash
bash tests/run_tests.sh
```

Keluaran:

```text
Membangun aplikasi dan program uji

Uji unit antrean dan penjadwal
PASS uji unit antrean dan round robin

Skenario aplikasi
PASS satu proses
PASS empat proses bergiliran
PASS quantum lebih besar dari beban
PASS jumlah proses maksimum
PASS quantum minimum

Skenario masukan tidak valid
PASS jumlah proses nol ditolak
PASS jumlah proses di atas batas ditolak
PASS quantum bukan angka ditolak
PASS opsi tanpa nilai ditolak
PASS opsi tidak dikenal ditolak
PASS bantuan tersedia

12 dari 12 skenario lulus
```

Kode keluar: 0


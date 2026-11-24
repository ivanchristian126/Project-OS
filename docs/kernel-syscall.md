# Syscall ts_mark

Aplikasi memanggil `ts_mark(pid, slice_ms)` pada awal setiap giliran. Kernel menambah penghitung global dan dapat menulis pesan ke log. Penjadwalan Round Robin dan timer dijalankan oleh aplikasi.

| Bagian | Arti |
| --- | --- |
| `pid` | PID target yang dilaporkan aplikasi. Harus positif. |
| `slice_ms` | Jatah giliran dalam milidetik, dari 1 sampai 10000. |
| Berhasil | Mengembalikan nilai penghitung kernel setelah bertambah. |
| Argumen di luar batas | Panggilan dari C mengembalikan -1 dengan `errno == EINVAL`. |

Nomor **454** berlaku untuk build x86-64 WSL proyek ini. Sebelum memakai binary pada build lain, cocokkan [nomor di aplikasi](../include/ts_syscall.h) dengan entri `ts_mark` di tabel syscall source kernel yang akan di-boot. Penghubung aplikasi memeriksa nilai balik dari nomor tersebut, sehingga nilai balik saja tidak membuktikan identitas syscall pada kernel lain.

## Memeriksa satu sesi

Setelah [boot kernel WSL proyek](kernel-wsl.md), jalankan perintah dari folder aplikasi:

```bash
uname -r
make -B all syscall-test &&
./build/ts_mark_test &&
./build/timesharing -p 3 -q 50 -w 200
sudo dmesg | grep 'ts_mark:' | tail -10
```

Contoh pasangan baris dari sesi yang sama:

```text
Giliran 1: proses 1 (pid 510) jatah 50 ms, sisa 200 ms
[  113.495100] ts_mark: caller=865 target=510 slice=50 ms total=4
```

Di sini, `target=510` dan `slice=50 ms` cocok dengan giliran pertama worker. Penghitung kernel sudah mencapai 4 karena dua panggilan uji yang valid dan satu pemeriksaan awal aplikasi terjadi sebelum giliran pertama.

`Giliran tercatat kernel` menghitung panggilan giliran yang berhasil selama satu simulasi. Nilai `total` mencakup semua panggilan valid sejak boot. Kernel membatasi frekuensi pesan `dmesg`, sehingga jumlah baris yang terlihat bisa lebih kecil daripada jumlah panggilan yang berhasil.

[Implementasi syscall](../kernel/ts_mark.c) · [Penghubung aplikasi](../src/kernel_bridge/kernel_bridge.c) · [Program uji](../kernel/tests/ts_mark_test.c)

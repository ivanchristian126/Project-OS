# Modifikasi Syscall ts_mark

Dokumen ini menjelaskan syscall tambahan yang dipakai proyek, cara
memasangnya ke kernel, dan cara mengujinya.

## Tujuan

Aplikasi time sharing memberi tahu kernel setiap kali satu giliran
diberikan ke proses. Pencatatan dilakukan oleh syscall baru bernama
`ts_mark` sehingga pergantian giliran dapat diperiksa dari log kernel,
bukan hanya dari keluaran aplikasi.

## Antarmuka

```c
long ts_mark(pid_t pid, unsigned int slice_ms);
```

| Bagian | Keterangan |
| --- | --- |
| `pid` | PID proses yang mendapat giliran, harus lebih dari 0 |
| `slice_ms` | Panjang giliran dalam ms, 1 sampai 10000 |
| Nilai balik | Jumlah total giliran yang tercatat sejak boot |
| Error | `-EINVAL` bila `pid` atau `slice_ms` di luar batas |

Sumber kernel ada di [kernel/ts_mark.c](../kernel/ts_mark.c). Nomor
syscall yang dipakai aplikasi ada di
[include/ts_syscall.h](../include/ts_syscall.h).

## Memasang ke Source Kernel

Siapkan source kernel lebih dulu sesuai
[docs/kernel-setup.md](kernel-setup.md), lalu jalankan:

```bash
bash kernel/patches/apply-ts-mark.sh ~/kernel/linux-6.8.12
```

Skrip melakukan empat perubahan:

1. menyalin `kernel/ts_mark.c` ke source kernel,
2. menambahkan baris `ts_mark` ke
   `arch/x86/entry/syscalls/syscall_64.tbl` memakai nomor bebas berikutnya,
3. menambahkan prototipe `sys_ts_mark` ke `include/linux/syscalls.h`,
4. menambahkan `obj-y += ts_mark.o` ke `kernel/Makefile`.

Skrip juga menyelaraskan `TS_MARK_SYSCALL_NR` di `include/ts_syscall.h`
dengan nomor yang benar-benar dipakai. Jika nilainya berubah, commit
perubahan tersebut agar aplikasi memanggil nomor yang tepat.

Skrip aman dijalankan dua kali: perubahan yang sudah ada tidak diulang.

## Membuat File Patch

Jika source kernel berupa repository git, skrip langsung menulis
`kernel/patches/0001-add-ts_mark-syscall.patch`. Untuk source dari
tarball, simpan salinan bersih sebelum menjalankan skrip:

```bash
cd ~/kernel
cp -a linux-6.8.12 linux-6.8.12.orig
bash ~/Project-OS/kernel/patches/apply-ts-mark.sh ~/kernel/linux-6.8.12
diff -Naur linux-6.8.12.orig linux-6.8.12 \
  > ~/Project-OS/kernel/patches/0001-add-ts_mark-syscall.patch
```

Patch dapat dipasang ulang di mesin lain dengan:

```bash
cd ~/kernel/linux-6.8.12
patch -p1 < ~/Project-OS/kernel/patches/0001-add-ts_mark-syscall.patch
```

## Kompilasi dan Boot

```bash
cd ~/kernel/linux-6.8.12
make -j"$(nproc)"
sudo make modules_install
sudo make install
sudo update-grub
sudo reboot
```

Setelah reboot, pilih kernel hasil modifikasi dari GRUB, lalu periksa:

```bash
uname -r
```

## Pengujian Syscall

Bangun dan jalankan program uji dari direktori proyek:

```bash
make syscall-test
./build/ts_mark_test
```

Program uji memeriksa empat hal: panggilan normal berhasil, penghitung
bertambah pada panggilan berikutnya, `slice_ms` bernilai 0 ditolak dengan
`EINVAL`, dan `pid` tidak valid ditolak dengan `EINVAL`.

Jika kernel yang sedang berjalan belum dimodifikasi, program uji berhenti
dengan pesan bahwa syscall belum tersedia dan kode keluar 1. Itu bukan
error kompilasi, melainkan tanda bahwa kernel yang dipakai masih kernel
lama.

Catatan dari sisi kernel dapat dilihat dengan:

```bash
sudo dmesg | tail -20
```

Baris yang dicari berbentuk:

```text
ts_mark: caller=1234 target=1235 slice=100 ms total=7
```

## Integrasi dengan Aplikasi

Aplikasi memanggil `ts_mark` setiap kali satu giliran diberikan ke proses
anak. Penghubungnya ada di
[src/kernel_bridge/kernel_bridge.c](../src/kernel_bridge/kernel_bridge.c).

Pemeriksaan dilakukan sekali saat program mulai, lalu hasilnya dipakai
untuk seluruh sesi:

| Keadaan kernel | Yang dilakukan aplikasi |
| --- | --- |
| syscall tersedia | mencatat setiap giliran, jumlahnya muncul di laporan |
| syscall tidak ada (`ENOSYS`) | menampilkan pesan dan berjalan tanpa pencatatan |
| nomor syscall dipakai fitur lain | pencatatan dimatikan agar tidak salah panggil |
| syscall gagal di tengah sesi | pencatatan dimatikan, simulasi tetap lanjut |

Contoh baris status pada kernel yang belum dimodifikasi:

```text
Fitur kernel: syscall ts_mark nomor 462 tidak ada pada kernel ini
Pencatatan ke kernel dilewati, simulasi tetap berjalan.
```

Pada kernel hasil modifikasi, laporan akhir memuat baris:

```text
Giliran tercatat kernel: 12
```

Angka tersebut dapat dicocokkan dengan `total` pada keluaran
`sudo dmesg | tail -20`.

## Pengembalian

Kernel lama tetap ada di menu GRUB. Jika kernel hasil modifikasi gagal
boot, pilih kernel lama lewat Advanced options, lalu ikuti bagian
pengembalian pada [docs/kernel-setup.md](kernel-setup.md).

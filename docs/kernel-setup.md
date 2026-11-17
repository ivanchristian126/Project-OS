# Penyiapan Lingkungan Kernel Linux

Dokumen ini mencatat lingkungan yang dipakai untuk pekerjaan kernel pada
proyek ini dan langkah membangun kernel dari source.

## Lingkungan yang Digunakan

Pekerjaan kernel tidak boleh dilakukan pada mesin utama. Gunakan mesin
virtual agar kernel yang gagal boot tidak mengganggu sistem kerja.

| Komponen | Nilai yang dipakai |
| --- | --- |
| Hypervisor | VirtualBox 7.x, VMware Workstation, atau KVM |
| Distribusi | Ubuntu 24.04 LTS (desktop atau server) |
| vCPU | minimal 4 inti |
| RAM | minimal 8 GB |
| Disk | minimal 40 GB kosong |
| Hak akses | akun dengan sudo |

Kompilasi kernel memakai sekitar 25-30 GB ruang disk dan 30-90 menit
tergantung jumlah inti. Snapshot VM sebelum reboot ke kernel baru.

## Paket Build

```bash
sudo apt update
sudo apt install -y build-essential libncurses-dev bison flex libssl-dev \
  libelf-dev dwarves bc rsync zstd cpio kmod fakeroot git
```

## Mencatat Versi Lingkungan

Versi tidak boleh ditulis manual dari ingatan. Jalankan skrip berikut di
VM Linux, lalu commit hasilnya.

```bash
bash scripts/collect-env.sh
```

Skrip menulis hasil `uname`, versi distribusi, gcc, make, dan binutils ke
`docs/kernel-env.txt`. File tersebut adalah catatan resmi lingkungan yang
dipakai kelompok.

## Mengambil Source Kernel

Pakai versi yang sama dengan kernel yang sedang berjalan agar konfigurasi
bawaan distribusi dapat dipakai ulang. Periksa dulu versinya:

```bash
uname -r
```

Unduh source stabil dari kernel.org, ganti `6.8.12` dengan versi yang
sesuai:

```bash
mkdir -p ~/kernel && cd ~/kernel
curl -LO https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.8.12.tar.xz
tar -xf linux-6.8.12.tar.xz
cd linux-6.8.12
```

Catat URL dan versi yang benar-benar diunduh ke `docs/kernel-env.txt`
pada bagian `KERNEL_SOURCE`.

## Konfigurasi

Mulai dari konfigurasi kernel distribusi yang sudah terbukti boot:

```bash
cp /boot/config-"$(uname -r)" .config
make olddefconfig
```

Matikan info debug dan kunci modul bawaan Ubuntu supaya build lebih cepat
dan tidak meminta kunci yang tidak tersedia:

```bash
scripts/config --disable DEBUG_INFO
scripts/config --disable DEBUG_INFO_DWARF5
scripts/config --disable SYSTEM_TRUSTED_KEYS
scripts/config --disable MODULE_SIG_KEY
scripts/config --set-str SYSTEM_TRUSTED_KEYS ""
scripts/config --set-str MODULE_SIG_KEY ""
```

## Kompilasi dan Instalasi

```bash
make -j"$(nproc)"
sudo make modules_install
sudo make install
sudo update-grub
```

Reboot, lalu pilih kernel baru dari menu GRUB (Advanced options for
Ubuntu). Setelah masuk, verifikasi:

```bash
uname -r
```

## Pengembalian Jika Gagal Boot

1. Tahan Shift atau Esc saat VM menyala untuk membuka GRUB.
2. Pilih Advanced options, lalu kernel lama.
3. Setelah masuk, hapus kernel bermasalah:

```bash
sudo apt remove --purge linux-image-<versi-gagal>
sudo update-grub
```

Jika VM tetap tidak bisa masuk, pulihkan snapshot yang dibuat sebelum
reboot.

## Catatan Lanjutan

Modifikasi syscall dan program pengujiannya dibahas pada
[docs/kernel-syscall.md](kernel-syscall.md).

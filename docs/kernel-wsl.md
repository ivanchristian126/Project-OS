# Memasang kernel proyek di WSL 2

Panduan ini menggunakan Ubuntu WSL 2 pada x86-64 dan [source Microsoft versi 6.6.87.2](https://github.com/microsoft/WSL2-Linux-Kernel/tree/linux-msft-wsl-6.6.87.2). Jika `uname -r` sudah menampilkan kernel proyek dengan akhiran `-tsmark+`, langsung ikuti [cara menjalankan aplikasi](../README.md).

## Mengambil source dan memasang syscall

Di Ubuntu, mulai dari folder proyek yang berisi `build.sh`. Simpan source kernel di filesystem Linux agar build berlangsung di direktori home.

```bash
project_dir="$PWD"
tree="$HOME/kernel/wsl-6.6.87.2"
sudo apt update
sudo apt install build-essential git flex bison dwarves libssl-dev libelf-dev bc cpio qemu-utils rsync kmod
mkdir -p "$HOME/kernel"
git clone --depth 1 --branch linux-msft-wsl-6.6.87.2 https://github.com/microsoft/WSL2-Linux-Kernel.git "$tree"
bash "$project_dir/kernel/patches/apply-ts-mark.sh" "$tree"
git -C "$tree" diff --cached --stat
```

Pada source bersih di atas, skrip memasang syscall nomor 454, menyelaraskan header aplikasi, dan menghasilkan [patch empat file](../kernel/patches/0001-add-ts_mark-syscall.patch). Untuk source yang sudah dipatch, lanjutkan dari tahap build.

## Membangun image

```bash
cd "$tree"
cp Microsoft/config-wsl .config
./scripts/config --set-str LOCALVERSION "-microsoft-standard-WSL2-tsmark"
make olddefconfig && make -j4
```

Jika toolchain baru menampilkan salah satu error berikut, jalankan perbaikan yang sesuai **dari folder source kernel** lalu ulangi `make -j4`.

- `libbpf.c: assignment discards 'const' qualifier` di `resolve_full_path`. Samakan tipe pointer dengan nilai yang dibaca:
  ```bash
  sed -i '/^static int resolve_full_path(/,/^}/s/^\([[:space:]]*\)char \*next_path;$/\1const char *next_path;/' tools/lib/bpf/libbpf.c
  ```
- `cannot use keyword 'false' as enumeration constant` ketika membangun `wakemain.c`. Pilih GNU C11 untuk real mode:
  ```bash
  sed -i 's/^REALMODE_CFLAGS += -ffreestanding$/REALMODE_CFLAGS += -ffreestanding -std=gnu11/' arch/x86/Makefile
  ```

Setelah build selesai, pastikan image tersedia dan simbol syscall sudah tertaut:

```bash
make -s kernelrelease
ls -lh arch/x86/boot/bzImage
nm vmlinux | grep -w __x64_sys_ts_mark
```

## Memilih kernel dari Windows

Di Ubuntu, buat image modul yang sesuai dengan kernel. Contoh menggunakan profil Windows `chris`. Sesuaikan kedua path profil bila memakai akun lain. Skrip pembuat VHDX memerlukan nama file tujuan yang belum ada.

```bash
dest="/mnt/c/Users/chris/wsl-kernels/tsmark"
mkdir -p "$dest" &&
make INSTALL_MOD_PATH="$PWD/modules" modules_install &&
sudo bash Microsoft/scripts/gen_modules_vhdx.sh "$PWD/modules" "$(make -s kernelrelease)" "$dest/modules.vhdx" &&
cp arch/x86/boot/bzImage "$dest/bzImage"
```

Di PowerShell, buka `notepad "$env:USERPROFILE\.wslconfig"`. Tambahkan pengaturan ini ke bagian `[wsl2]` yang sudah ada, atau buat bagian tersebut. Simpan nilai lama jika sebelumnya memakai kernel khusus.

```ini
[wsl2]
kernel=C:\\Users\\chris\\wsl-kernels\\tsmark\\bzImage
kernelModules=C:\\Users\\chris\\wsl-kernels\\tsmark\\modules.vhdx
```

Simpan pekerjaan pada semua distro WSL 2. Pengaturan ini berlaku secara global. Di PowerShell:

```powershell
wsl --shutdown
wsl -d Ubuntu
```

Di Ubuntu, jalankan `uname -r`. Pada build yang diuji hasilnya `6.6.87.2-microsoft-standard-WSL2-tsmark+`. Lanjut ke [pengujian syscall](kernel-syscall.md). Untuk kembali ke kernel bawaan, hapus dua pengaturan path tersebut (atau pulihkan nilai sebelumnya), lalu ulangi langkah restart WSL.

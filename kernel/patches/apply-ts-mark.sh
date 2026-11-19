#!/usr/bin/env bash
# Memasang syscall ts_mark ke source kernel, lalu membuat file patch.
# Pemakaian: bash kernel/patches/apply-ts-mark.sh <direktori-source-kernel>
set -euo pipefail

project_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)"
tree="${1:-}"

if [[ -z "$tree" || ! -d "$tree" ]]; then
    printf 'Pemakaian: bash kernel/patches/apply-ts-mark.sh <direktori-source-kernel>\n' >&2
    exit 1
fi

tbl="$tree/arch/x86/entry/syscalls/syscall_64.tbl"
hdr="$tree/include/linux/syscalls.h"
kmake="$tree/kernel/Makefile"

for required in "$tbl" "$hdr" "$kmake"; do
    if [[ ! -f "$required" ]]; then
        printf 'Bukan source kernel yang valid, file tidak ditemukan: %s\n' "$required" >&2
        exit 1
    fi
done

install -m 644 "$project_root/kernel/ts_mark.c" "$tree/kernel/ts_mark.c"
printf 'Menyalin kernel/ts_mark.c ke source kernel\n'

if grep -q 'ts_mark' "$tbl"; then
    number="$(awk '$3 == "ts_mark" { print $1 }' "$tbl" | head -1)"
    printf 'Tabel syscall sudah memuat ts_mark pada nomor %s\n' "$number"
else
    number="$(awk '$1 ~ /^[0-9]+$/ && $1 + 0 < 512 { if ($1 + 0 > n) n = $1 + 0 } END { print n + 1 }' "$tbl")"
    awk -v nr="$number" '
        { line[NR] = $0; if ($1 ~ /^[0-9]+$/ && $1 + 0 < 512) last = NR }
        END {
            for (i = 1; i <= NR; i++) {
                print line[i]
                if (i == last)
                    printf "%d\tcommon\tts_mark\t\t\tsys_ts_mark\n", nr
            }
        }
    ' "$tbl" > "$tbl.ts-mark"
    mv "$tbl.ts-mark" "$tbl"
    printf 'Menambahkan ts_mark ke tabel syscall pada nomor %s\n' "$number"
fi

if grep -q 'sys_ts_mark' "$hdr"; then
    printf 'Prototipe sys_ts_mark sudah ada\n'
else
    sed -i '/^asmlinkage long sys_ni_syscall(void);/a asmlinkage long sys_ts_mark(pid_t pid, unsigned int slice_ms);' "$hdr"
    if ! grep -q 'sys_ts_mark' "$hdr"; then
        printf 'Baris acuan sys_ni_syscall tidak ada di %s. Tambahkan prototipe manual.\n' "$hdr" >&2
        exit 1
    fi
    printf 'Menambahkan prototipe ke include/linux/syscalls.h\n'
fi

if grep -q 'ts_mark.o' "$kmake"; then
    printf 'kernel/Makefile sudah memuat ts_mark.o\n'
else
    printf '\n# Syscall tambahan proyek Time Sharing System\nobj-y += ts_mark.o\n' >> "$kmake"
    printf 'Menambahkan obj-y += ts_mark.o ke kernel/Makefile\n'
fi

app_header="$project_root/include/ts_syscall.h"
current="$(awk '/^#define TS_MARK_SYSCALL_NR/ { print $3 }' "$app_header")"
if [[ "$current" != "$number" ]]; then
    sed -i "s/^#define TS_MARK_SYSCALL_NR .*/#define TS_MARK_SYSCALL_NR $number/" "$app_header"
    printf 'TS_MARK_SYSCALL_NR di include/ts_syscall.h diubah menjadi %s. Commit perubahan ini.\n' "$number"
fi

patch_file="$project_root/kernel/patches/0001-add-ts_mark-syscall.patch"
if git -C "$tree" rev-parse --is-inside-work-tree >/dev/null 2>&1; then
    git -C "$tree" add -A arch/x86/entry/syscalls/syscall_64.tbl \
        include/linux/syscalls.h kernel/Makefile kernel/ts_mark.c
    git -C "$tree" diff --cached > "$patch_file"
    printf 'Patch ditulis ke %s\n' "$patch_file"
else
    printf 'Source kernel bukan repository git sehingga patch tidak dibuat.\n'
    printf 'Ikuti docs/kernel-syscall.md bagian membuat patch dari tarball.\n'
fi

printf 'Selesai. Lanjutkan kompilasi kernel sesuai docs/kernel-syscall.md.\n'

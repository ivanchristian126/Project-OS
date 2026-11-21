#!/usr/bin/env bash
# Membangun, menguji, lalu membuat paket pengumpulan di direktori dist.
set -euo pipefail

cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.."

if [[ "$(uname -s)" != "Linux" ]]; then
    printf '%s\n' 'Buat paket di Linux agar executable sesuai target.' >&2
    exit 1
fi

printf 'Membangun ulang dari awal\n'
make --no-print-directory clean >/dev/null
make --no-print-directory all >/dev/null
make --no-print-directory syscall-test >/dev/null

printf 'Menjalankan uji unit\n'
make --no-print-directory test >/dev/null

printf 'Menjalankan uji skenario\n'
bash tests/run_tests.sh >/dev/null

if grep -q 'BELUM DIISI' docs/report.md; then
    printf 'docs/report.md belum berisi hasil nyata.\n' >&2
    printf 'Jalankan: bash scripts/run_report.sh\n' >&2
    exit 1
fi
if grep -q 'BELUM DIISI' docs/kernel-env.txt; then
    printf 'docs/kernel-env.txt belum berisi catatan lingkungan.\n' >&2
    printf 'Jalankan: bash scripts/collect-env.sh\n' >&2
    exit 1
fi

version="$(./build/timesharing --version | head -1 | awk '{ print $NF }')"
name="time-sharing-system-${version#v}-$(date +%Y%m%d)"
stage="dist/$name"

rm -rf dist
mkdir -p "$stage/build"

cp -r include src kernel tests scripts docs Makefile build.sh README.md \
    .gitignore .gitattributes "$stage/"
cp build/timesharing "$stage/build/"
if [[ -f build/ts_mark_test ]]; then
    cp build/ts_mark_test "$stage/build/"
fi

tar -czf "dist/$name.tar.gz" -C dist "$name"
if command -v zip >/dev/null 2>&1; then
    (cd dist && zip -qr "$name.zip" "$name")
fi

printf '\nPaket siap:\n'
ls -1 dist/*.tar.gz dist/*.zip 2>/dev/null || ls -1 dist/*.tar.gz
printf '\nSidik berkas:\n'
sha256sum dist/*.tar.gz dist/*.zip 2>/dev/null || sha256sum dist/*.tar.gz
printf '\nVideo demo tidak ikut dipaketkan. Unggah terpisah sesuai\n'
printf 'ketentuan tugas, lihat docs/submission.md.\n'

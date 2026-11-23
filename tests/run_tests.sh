#!/usr/bin/env bash
# Menjalankan uji unit dan skenario aplikasi untuk berbagai jumlah proses.
set -uo pipefail

cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.."

if [[ "$(uname -s)" != "Linux" ]]; then
    printf '%s\n' 'Jalankan pengujian di Linux, misalnya Ubuntu VM.' >&2
    exit 1
fi

if ! command -v timeout >/dev/null 2>&1; then
    printf '%s\n' 'Pengujian memerlukan timeout dari GNU coreutils.' >&2
    exit 1
fi

total=0
failed=0
app=./build/timesharing

run_case() {
    local name="$1"
    local expected_exit="$2"
    local pattern="$3"
    shift 3

    local output status
    total=$((total + 1))
    output="$(timeout --kill-after=2s 20s "$@" 2>&1)"
    status=$?

    if [[ "$status" -ne "$expected_exit" ]]; then
        printf 'FAIL %s (exit %d, diharapkan %d)\n' "$name" "$status" "$expected_exit"
        printf '%s\n' "$output"
        failed=$((failed + 1))
        return
    fi
    if [[ -n "$pattern" ]] && ! grep -Fq -- "$pattern" <<< "$output"; then
        printf 'FAIL %s (keluaran tidak memuat: %s)\n' "$name" "$pattern"
        printf '%s\n' "$output"
        failed=$((failed + 1))
        return
    fi
    printf 'PASS %s\n' "$name"
}

printf 'Membangun aplikasi dan program uji\n'
if ! make --no-print-directory all >/dev/null; then
    printf 'Kompilasi aplikasi gagal.\n' >&2
    exit 1
fi
if ! make --no-print-directory build/test_queue build/test_worker build/test_timer_failure >/dev/null; then
    printf 'Kompilasi uji unit gagal.\n' >&2
    exit 1
fi

printf '\nUji unit antrean dan penjadwal\n'
run_case "uji unit antrean dan round robin" 0 "pemeriksaan lulus" ./build/test_queue
run_case "worker menunggu giliran dan dibersihkan" 0 "Pemeriksaan worker lulus" ./build/test_worker
run_case "timer gagal menghasilkan exit 1 tanpa anak tersisa" 1 \
    "Cleanup setelah kegagalan timer lulus" ./build/test_timer_failure -p 3 -w 10000

printf '\nSkenario aplikasi\n'
run_case "satu proses" 0 "Proses selesai: 1 dari 1" \
    "$app" -p 1 -q 100 -w 200
run_case "empat proses bergiliran" 0 "Proses selesai: 4 dari 4" \
    "$app" -p 4 -q 50 -w 150
run_case "quantum lebih besar dari beban" 0 "Proses selesai: 3 dari 3" \
    "$app" -p 3 -q 1000 -w 100
run_case "jumlah proses maksimum" 0 "Proses selesai: 16 dari 16" \
    "$app" -p 16 -q 20 -w 60
run_case "quantum minimum" 0 "Proses selesai: 2 dari 2" \
    "$app" -p 2 -q 10 -w 100

printf '\nSkenario masukan tidak valid\n'
run_case "jumlah proses nol ditolak" 2 "Jumlah proses tidak valid" \
    "$app" -p 0
run_case "jumlah proses di atas batas ditolak" 2 "Jumlah proses tidak valid" \
    "$app" -p 99
run_case "quantum bukan angka ditolak" 2 "tidak valid" \
    "$app" -q abc
run_case "opsi tanpa nilai ditolak" 2 "membutuhkan satu nilai" \
    "$app" -p
run_case "opsi tidak dikenal ditolak" 2 "Opsi tidak dikenal" \
    "$app" --tidak-ada
run_case "bantuan tersedia" 0 "Penggunaan" "$app" --help

printf '\n%d dari %d skenario lulus\n' "$((total - failed))" "$total"
if [[ "$failed" -ne 0 ]]; then
    exit 1
fi

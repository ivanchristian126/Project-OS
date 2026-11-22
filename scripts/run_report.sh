#!/usr/bin/env bash
# Menjalankan aplikasi dan uji, lalu menulis hasil nyata ke docs/report.md.
set -uo pipefail

cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.."

if [[ "$(uname -s)" != "Linux" ]]; then
    printf '%s\n' 'Jalankan skrip ini di Linux, misalnya Ubuntu VM.' >&2
    exit 1
fi

if ! make --no-print-directory all >/dev/null; then
    printf 'Kompilasi gagal, laporan tidak dibuat.\n' >&2
    exit 1
fi

out=docs/report.md
app=./build/timesharing

{
    printf '# Laporan Hasil Eksekusi\n\n'
    printf 'File ini dibuat oleh scripts/run_report.sh dari eksekusi nyata.\n'
    printf 'Jalankan ulang skrip setelah ada perubahan kode.\n\n'
    printf 'Baris pencatatan kernel hanya terisi bila skrip dijalankan pada\n'
    printf 'kernel hasil modifikasi. Lihat docs/kernel-syscall.md.\n\n'
    printf '## Lingkungan Pengujian\n\n'
    printf '| Keterangan | Nilai |\n'
    printf '| --- | --- |\n'
    printf '| Waktu | %s |\n' "$(date -Is)"
    printf '| Host | %s |\n' "$(uname -n)"
    printf '| Kernel | %s |\n' "$(uname -r)"
    printf '| Arsitektur | %s |\n' "$(uname -m)"
    printf '| Jumlah inti | %s |\n' "$(nproc)"
    printf '| Compiler | %s |\n' "$(gcc --version | head -1)"
    printf '| Versi aplikasi | %s |\n\n' "$("$app" --version | head -1)"
} > "$out"

run_scenario() {
    local title="$1"
    shift

    {
        printf '### %s\n\n' "$title"
        printf 'Perintah:\n\n'
        printf '```bash\n%s\n```\n\n' "$*"
        printf 'Keluaran:\n\n'
        printf '```text\n'
    } >> "$out"

    "$@" >> "$out" 2>&1
    local status=$?

    {
        printf '```\n\n'
        printf 'Kode keluar: %d\n\n' "$status"
    } >> "$out"
}

printf '## Skenario Simulasi\n\n' >> "$out"
run_scenario "Satu proses" "$app" -p 1 -q 100 -w 200
run_scenario "Tiga proses dengan nilai default" "$app" -p 3 -q 100 -w 500
run_scenario "Empat proses dengan quantum kecil" "$app" -p 4 -q 20 -w 200
run_scenario "Enam belas proses" "$app" -p 16 -q 20 -w 60

printf '## Penolakan Masukan Tidak Valid\n\n' >> "$out"
run_scenario "Jumlah proses nol" "$app" -p 0
run_scenario "Quantum bukan angka" "$app" -q abc
run_scenario "Opsi tidak dikenal" "$app" --tidak-ada

printf '## Hasil Pengujian\n\n' >> "$out"
run_scenario "Uji unit antrean dan penjadwal" make --no-print-directory test
run_scenario "Uji skenario aplikasi" bash tests/run_tests.sh

printf 'Laporan ditulis ke %s\n' "$out"

#!/usr/bin/env bash
# Mencatat versi lingkungan build ke docs/kernel-env.txt.
set -euo pipefail

cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.."

if [[ "$(uname -s)" != "Linux" ]]; then
    printf '%s\n' 'Jalankan skrip ini di Linux, misalnya Ubuntu VM.' >&2
    exit 1
fi

output="docs/kernel-env.txt"
source_note="<url dan versi source kernel yang diunduh>"
if [[ -f "$output" ]]; then
    existing="$(sed -n 's/^KERNEL_SOURCE=//p' "$output" | head -1 || true)"
    if [[ -n "${existing:-}" ]]; then
        source_note="$existing"
    fi
fi

version_of() {
    if command -v "$1" >/dev/null 2>&1; then
        "$@" 2>&1 | head -1
    else
        printf '%s\n' "tidak tersedia"
    fi
}

{
    printf '%s\n' '# Catatan lingkungan kernel proyek Time Sharing System'
    printf '%s\n' "# Dibuat oleh scripts/collect-env.sh pada $(date -Is)"
    printf '\n'
    printf 'HOSTNAME=%s\n' "$(uname -n)"
    printf 'KERNEL_RELEASE=%s\n' "$(uname -r)"
    printf 'KERNEL_VERSION=%s\n' "$(uname -v)"
    printf 'ARCH=%s\n' "$(uname -m)"
    printf 'CPU_CORES=%s\n' "$(nproc)"
    printf 'DISTRO=%s\n' "$(. /etc/os-release 2>/dev/null && printf '%s' "${PRETTY_NAME:-tidak diketahui}")"
    printf 'GCC=%s\n' "$(version_of gcc --version)"
    printf 'MAKE=%s\n' "$(version_of make --version)"
    printf 'BINUTILS=%s\n' "$(version_of ld --version)"
    printf 'KERNEL_SOURCE=%s\n' "$source_note"
} > "$output"

printf 'Catatan lingkungan ditulis ke %s\n' "$output"
cat "$output"

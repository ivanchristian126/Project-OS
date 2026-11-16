#define _DEFAULT_SOURCE

#include "ts_syscall.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

static long call_ts_mark(pid_t pid, unsigned int slice_ms)
{
    return syscall((long)TS_MARK_SYSCALL_NR, (long)pid, (long)slice_ms);
}

int main(void)
{
    printf("Uji syscall ts_mark nomor %d\n", TS_MARK_SYSCALL_NR);

    errno = 0;
    const long first = call_ts_mark(getpid(), 100U);

    if (first < 0) {
        if (errno == ENOSYS) {
            puts("GAGAL: kernel yang berjalan belum memiliki syscall ts_mark.");
            puts("Boot kernel hasil modifikasi lalu jalankan ulang uji ini.");
            return 1;
        }
        printf("GAGAL: panggilan pertama error: %s\n", strerror(errno));
        return 1;
    }
    printf("OK: panggilan pertama mengembalikan %ld\n", first);

    const long second = call_ts_mark(getpid(), 250U);

    if (second <= first) {
        printf("GAGAL: penghitung tidak bertambah (%ld lalu %ld)\n",
               first, second);
        return 1;
    }
    printf("OK: penghitung bertambah menjadi %ld\n", second);

    errno = 0;
    if (call_ts_mark(getpid(), 0U) != -1 || errno != EINVAL) {
        puts("GAGAL: slice 0 seharusnya ditolak dengan EINVAL.");
        return 1;
    }
    puts("OK: slice 0 ditolak dengan EINVAL");

    errno = 0;
    if (call_ts_mark(-1, 100U) != -1 || errno != EINVAL) {
        puts("GAGAL: pid tidak valid seharusnya ditolak dengan EINVAL.");
        return 1;
    }
    puts("OK: pid tidak valid ditolak dengan EINVAL");

    errno = 0;
    if (call_ts_mark(getpid(), TS_MARK_MAX_SLICE_MS + 1U) != -1 ||
        errno != EINVAL) {
        puts("GAGAL: slice di atas batas seharusnya ditolak dengan EINVAL.");
        return 1;
    }
    puts("OK: slice di atas batas ditolak dengan EINVAL");

    puts("Semua pemeriksaan syscall lulus.");
    puts("Catatan kernel dapat dilihat dengan: sudo dmesg | tail -20");
    return 0;
}

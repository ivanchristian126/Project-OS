#ifndef TS_SYSCALL_H
#define TS_SYSCALL_H

/*
 * Nomor syscall ts_mark pada kernel hasil modifikasi. Nilainya harus sama
 * dengan nomor di arch/x86/entry/syscalls/syscall_64.tbl. Skrip
 * kernel/patches/apply-ts-mark.sh memperbarui nilai ini otomatis.
 */
#ifndef TS_MARK_SYSCALL_NR
#define TS_MARK_SYSCALL_NR 462
#endif

/* Batas panjang slice yang diterima syscall. */
#define TS_MARK_MAX_SLICE_MS 10000U

#endif

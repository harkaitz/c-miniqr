#ifndef MINIQR_H
#define MINIQR_H

#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>

typedef struct miniqr {
    unsigned flags;
    bool     closed;
    int      pid1;
    int      pid2;
} miniqr;

bool miniqr_library_init   (const char *_opts[]);
void miniqr_library_deinit (void);

bool miniqr_create_v(miniqr **_m, int _o_fd, unsigned _flags, const char *_url_fmt, va_list va);
bool miniqr_create  (miniqr **_m, int _o_fd, unsigned _flags, const char *_url_fmt, ...);
bool miniqr_wait    (miniqr  *_m, int _o_fd);
void miniqr_destroy (miniqr  *_m);

#define MINIQR_WRAP_WITH_HTML 0x01
#define MINIQR_BASE64         0x02

#endif

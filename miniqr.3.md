# NAME

`struct miniqr`, `miniqr_library_init()`, `miniqr_library_deinit()`,
`miniqr_create[_v]()`, `miniqr_wait()`, `miniqr_destroy()`

# SYNOPSIS

    #include <miniqr.h>
    
    typedef struct miniqr miniqr;
    
    bool miniqr_library_init   (const char *_opts[]);
    void miniqr_library_deinit (void);
    
    bool miniqr_create_v(miniqr **_m, int _o_fd, unsigned _flags, const char *_url_fmt, va_list va);
    bool miniqr_create  (miniqr **_m, int _o_fd, unsigned _flags, const char *_url_fmt, ...);
    bool miniqr_wait    (miniqr  *_m, int _o_fd);
    void miniqr_destroy (miniqr  *_m);
    
    #define MINIQR_WRAP_WITH_HTML 0x01
    #define MINIQR_BASE64         0x02

# DESCRIPTION

Small QR generation library. It uses *qrencode(1)* and *base64(1)*.

# RETURN VALUE

True on success false on error.

# COLLABORATING

For making bug reports, feature requests and donations visit one of the
following links:

1. [gemini://harkadev.com/oss/](gemini://harkadev.com/oss/)
2. [https://harkadev.com/oss/](https://harkadev.com/oss/)

# SEE ALSO

**QRENCODE(1)**, **BASE64(1)**

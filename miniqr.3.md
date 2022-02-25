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

You can collaborate with this project either by making bug reports,
making pull requests or making a donation.

- Bug reports, pull requests: Harkaitz Agirre <harkaitz.aguirre@gmail.com>
- *Bitcoin* : _1C1ZzDje7vHhF23mxqfcACE8QD4nqxywiV_
- *Binance* : _bnb194ay2cy83jjp644hdz8vjgjxrj5nmmfkngfnul_
- *Monero* : _88JP1c94kDEbyddN4NGU574vwXHB6r3FqcFX9twmxBkGNSnf64c5wjE89YaRVUk7vBbdnecWSXJmRhFWUcLcXUTFJVddZti_

# SEE ALSO

**QRENCODE(1)**, **BASE64(1)**

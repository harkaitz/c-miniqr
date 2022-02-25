#include "miniqr.h"
#include <string.h>
#include <libgen.h>

int main (int _argc, char *_argv[]) {
    _argv[0] = basename(_argv[0]);
        
    int         opt,res;
    unsigned    flags  = 0;
    const char *url    = NULL;
    miniqr     *m      = NULL;
    int         retval = 1;

    if (_argc==1 || !strcmp(_argv[1], "-h") || !strcmp(_argv[1], "--help")) {
        fprintf(stdout, "Usage: %s [-bt] URL\n", _argv[0]);
        return 0;
    }
    
    while((opt = getopt (_argc, _argv, "bt")) != -1) {
        switch (opt) {
        case 'b': flags |= MINIQR_BASE64; break;
        case 't': flags |= MINIQR_WRAP_WITH_HTML; break;
        case '?':
        default:
            return 1;
        }
    }
    
    if (optind >= _argc/*err*/) goto cleanup_missing_url;
    url = _argv[optind];

    res = miniqr_library_init(NULL);
    if (!res/*err*/) goto cleanup;

    res = miniqr_create(&m, 1, flags, "%s", url);
    if (!res/*err*/) goto cleanup;

    res = miniqr_wait(m, 1);
    if (!res/*err*/) goto cleanup;

    retval = true;
    goto cleanup;
 cleanup_missing_url:
    fprintf(stderr, "%s: error: Missing URL.", _argv[0]);
    goto cleanup;
 cleanup:
    if (m) miniqr_destroy(m);
    miniqr_library_deinit();
    return retval;
}

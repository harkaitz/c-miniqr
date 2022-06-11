#include "miniqr.h"
#include <sys/pathsearch.h>
#include <io/fcopy.h>
#include <io/slog.h>
#include <fcntl.h>
#include <stdarg.h>
#include <signal.h>
#include <sys/wait.h>

static struct {
    bool  inited;
    char *qrencode_m;
    char *base64_m;
} g_miniqr = {0};

bool miniqr_library_init (const char *_opts[]) {
    if (g_miniqr.inited) return true;
    const char *path;
    int         e;
    path = getenv("PATH");
    e = pathsearch(path, PATH_SEP, "qrencode", &g_miniqr.qrencode_m);
    if (!e/*err*/) goto cleanup;
    e = pathsearch(path, PATH_SEP, "base64", &g_miniqr.base64_m);
    if (!e/*err*/) goto cleanup;
    g_miniqr.inited = true;
    return true;
 cleanup:
    miniqr_library_deinit();
    return false;
}

void miniqr_library_deinit (void) {
    free(g_miniqr.qrencode_m);
    free(g_miniqr.base64_m);
    memset(&g_miniqr, 0, sizeof(g_miniqr));
}

bool miniqr_create_v(miniqr **_m, int _o_fd, unsigned _flags, const char *_url_fmt, va_list va) {

    bool     retval     = false;
    pid_t    pid1       = -1;
    pid_t    pid2       = -1;
    int      p1[2]      = {-1,-1};
    FILE    *url_fp     = NULL;
    char    *url_m      = NULL;
    size_t   url_msz    = 0;
    miniqr  *m          = NULL;
    int      e;
    
    /* Forge URL. */
    url_fp = open_memstream(&url_m, &url_msz);
    if (!url_fp/*err*/) goto cleanup_errno;
    e = vfprintf(url_fp, _url_fmt, va)!=-1 &&
        fputc('\0', url_fp)!=EOF &&
        fflush(url_fp)!=EOF;
    if (!e/*err*/) goto cleanup_errno;
    
    /* When base64 is needed, create pipe. */
    bool use_base64 = (_flags & (MINIQR_WRAP_WITH_HTML|MINIQR_BASE64))?true:false;
    if (use_base64) {
        e =
            pipe(p1)!=-1 &&
            fcntl(p1[0], F_SETFD, FD_CLOEXEC)!=-1 &&
            fcntl(p1[1], F_SETFD, FD_CLOEXEC)!=-1;
        if (!e/*err*/) goto cleanup_errno;
    }

    /* Open html. */
    if (_flags & MINIQR_WRAP_WITH_HTML) {
        e = dprintf(_o_fd, "<a href=\"%s\"><img src=\"data:image/png;base64,\n", url_m);
        if (e<0/*err*/) goto cleanup_errno;
        fsync(_o_fd);
    }
    
    /* Fork qrencode. */
    if ((pid1 = fork())==0) {
        if (use_base64) {
            dup2(p1[1], 1);
            close(0);
        } else if (_o_fd!=1) {
            dup2(_o_fd, 1);
            close(_o_fd);
        }
        execl(g_miniqr.qrencode_m, g_miniqr.qrencode_m, "-o", "-", "-t", "png", url_m, NULL);
        perror("Can't execute qrencode");
    }
    if (pid1==-1/*err*/) goto cleanup_errno;
    
    /* Fork base64 when needed. */
    if (use_base64) {
        if ((pid2 = fork())==0) {
            dup2(p1[0], 0);
            if (_o_fd!=1) {
                dup2(_o_fd, 1);
                close(_o_fd);
            }
            execl(g_miniqr.base64_m, g_miniqr.base64_m, NULL);
            perror("Can't execute base64");
        }
        if (pid2==-1/*err*/) goto cleanup_errno;
    }
    
    /* Return. */
    m = malloc(sizeof(struct miniqr));
    if (!m/*err*/) goto cleanup_errno;
    m->flags = _flags;
    m->closed = false;
    m->pid1 = pid1; pid1 = -1;
    m->pid2 = pid2; pid2 = -1;
    *_m = m; m = NULL;
    retval = true;
    goto cleanup;
 cleanup_errno:
    syslog(LOG_ERR, "%s", strerror(errno));
    goto cleanup;
 cleanup:
    if (url_fp)    fclose(url_fp);
    if (url_m)     free(url_m);
    if (m)         free(m);
    if (p1[0]!=-1) close(p1[0]);
    if (p1[1]!=-1) close(p1[1]);
    if (pid1!=-1)  kill(pid1, SIGINT);
    if (pid2!=-1)  kill(pid2, SIGINT);
    if (pid1!=-1)  waitpid(pid1, NULL, 0);
    if (pid2!=-1)  waitpid(pid2, NULL, 0);
    return retval;
}

bool miniqr_create(miniqr **_m, int _o_fd, unsigned _flags, const char *_url_fmt, ...) {
    va_list va; bool e;
    va_start(va, _url_fmt);
    e = miniqr_create_v(_m, _o_fd, _flags, _url_fmt, va);
    va_end(va);
    return e;
}

bool miniqr_wait(miniqr *_m, int _o_fd) {
    int  s;
    bool success = true;
    if (_m->pid1!=-1) {
        if (waitpid(_m->pid1, &s, 0)==-1 || !WIFEXITED(s) || WEXITSTATUS(s)) {
            success = false;
        }
        _m->pid1 = -1;
    }
    if (_m->pid2!=-1) {
        if (waitpid(_m->pid2, &s, 0)==-1 || !WIFEXITED(s) || WEXITSTATUS(s)) {
            success = false;
        }
        _m->pid2 = -1;
    }
    if (!_m->closed) {
        if (_m->flags & MINIQR_WRAP_WITH_HTML) {
            static const char s[] = "\"></a>\n";
            write(_o_fd, s, sizeof(s)-1);
        }
        _m->closed = true;
    }
    return success;
}

void miniqr_destroy(miniqr *_m) {
    if (_m) {
        if (_m->pid1!=-1) kill(_m->pid1, SIGINT);
        if (_m->pid2!=-1) kill(_m->pid2, SIGINT);
        if (_m->pid1!=-1) waitpid(_m->pid1, NULL, 0);
        if (_m->pid2!=-1) waitpid(_m->pid2, NULL, 0);
        free(_m);
    }
}

bool miniqr_printf_v(FILE *_fp, unsigned _flags, const char *_fmt, va_list va) {
    bool           retval         = false;
    miniqr        *miniqr         = NULL;
    int            p[2]           = {-1,-1};
    int            e;
    e = pipe(p);
    if (e==-1/*err*/) goto cleanup_errno;
    e = miniqr_create_v(&miniqr, p[1], _flags, _fmt, va);
    if (!e/*err*/) goto cleanup;
    close(p[1]); p[1] = -1;
    e = fcopy_fd(_fp, p[0], 1024, NULL);
    if (e<0/*err*/) goto cleanup_errno;
    e = miniqr_wait(miniqr, -1);
    if (!e/*err*/) goto cleanup;
    close(p[0]); p[0] = -1;
    retval = true;
    cleanup:
    if (p[0]==-1) close(p[0]);
    if (p[1]==-1) close(p[1]);
    if (miniqr)   miniqr_destroy(miniqr);
    return retval;
    cleanup_errno:
    error("%s", strerror(errno));
    goto cleanup;
}

bool miniqr_printf  (FILE *_fp, unsigned _flags, const char *_fmt, ...) {
    va_list        va;
    bool           r;
    va_start(va, _fmt);
    r = miniqr_printf_v(_fp, _flags, _fmt, va);
    va_end(va);
    return r;
}

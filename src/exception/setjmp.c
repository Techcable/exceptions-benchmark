#include <setjmp.h>
#include <stdlib.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdarg.h>

#include "exception.h"

const char *EXCEPTION_BACKEND_NAME = "setjmp";

static jmp_buf current_ctx;
static Exception* current_exc;

struct exception {
    const char *msg;
};

ExceptionResult try_catch_exception(
    void* (*func_ptr)(void* ctx),
    void* ctx
) {
    ExceptionResult res = {}; // zero-initialize
    if (setjmp(current_ctx)) {
        // Failure
        assert(current_exc != NULL);
        res.exception = current_exc;
    } else {
        // Original try catch
        res.success = (*func_ptr)(ctx);
        // success
        assert(res.exception == NULL);
    }
    return res;
}

_ATTR_NORETURN void throw_exception(Exception *exc) {
    assert(exc != NULL);
    current_exc = exc;
    longjmp(current_ctx, 17);
    assert(false);
}

Exception *create_exceptionf(const char *fmt, ...) {
    // TODO: Assume non-windows here ;)
    char *msg = NULL;
    va_list args;
    va_start(args, fmt);
    int code = vasprintf(&msg, fmt, args);
    va_end(args);
    if (code < 0) {
        throw_exception(create_simple_exception("Failed to format exception"));
    }
    assert(msg != NULL);
    return create_simple_exception((const char*) msg);
}

Exception *create_simple_exception(const char *msg) {
    // I believe C++ has to allocate for this, so let's try to get some parity
    Exception *exc = try_malloc(sizeof(Exception));
    exc->msg = msg;
    return exc;
}
static const Exception OOM_EXCEPTION= {
    .msg = "Out of memory (malloc probably failed)"
};
Exception *oom_exception() {
    return (Exception*) &OOM_EXCEPTION;
}

const char *get_exception_msg(Exception *e) {
    return e->msg;
}

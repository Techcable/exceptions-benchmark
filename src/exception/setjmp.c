#include <setjmp.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "exception.h"

static jmp_buf current_ctx;
static Exception* current_exc;

struct exception {
    const char *msg;
};

ExceptionResult try_catch(
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

Exception *create_simple_exception(const char *msg) {
    // I believe C++ has to allocate for this, so let's try to get some parity
    Exception *exc = malloc(sizeof(Exception));
    exc->msg = msg;
    return exc;
}

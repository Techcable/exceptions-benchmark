/**
 * An API for supporting "exceptions" in C.
 *
 * This is intended only for benchmarking and prototyping.
 * You have been warned ;)
 */


#ifdef __cplusplus
extern "C" {
#endif

extern const char *EXCEPTION_BACKEND_NAME;

#if __STDC_VERSION__ >= 201112L
    #define _ATTR_NORETURN _Noreturn
#else
    #error "Unsupported compiler"
#endif
#if defined(__GNUC__) || defined(__clang__)
    #define _ATTR_FORMAT_STRING(fmt, arg) __attribute__((format( printf, fmt, arg )))
#else
    // nop is fine
    #define _ATTR_FORMAT_STRING(fmt, arg)
#endif

typedef struct exception Exception;

typedef struct exception_result {
    /**
     * The exception 
     */
    Exception *exception;
    /**
     * The successful result
     *
     * Undefined if `exception` is set
     */
    void* success;
} ExceptionResult;


ExceptionResult try_catch_exception(
    void* (*func_ptr)(void* ctx),
    void* ctx
);

static inline ExceptionResult try_catch_exception_nop(
    void* (*func_ptr)(void* ctx),
    void* ctx
) {
    ExceptionResult res = {};
    res.success = (*func_ptr)(ctx);
    return res;
}

_ATTR_NORETURN void throw_exception(Exception *exc);

_ATTR_FORMAT_STRING(1, 2) Exception* create_exceptionf(const char *fmt, ...);

Exception* create_simple_exception(const char *msg);

Exception* oom_exception();

const char *get_exception_msg(Exception *e);

static inline void* try_malloc(size_t size) {
    void *res = malloc(size);
    if (res == NULL) throw_exception(oom_exception());
    return res;
}

#ifdef __cplusplus
} // extern "C"
#endif

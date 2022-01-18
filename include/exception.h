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

Exception* create_simple_exception(const char *msg);

const char *get_exception_msg(Exception *e);



#ifdef __cplusplus
} // extern "C"
#endif

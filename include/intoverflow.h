/**
 * "Portable" integer overflow checking
 *
 * Uses compiler builtins where possible.
 * TODO: Implement fallbacks
 *
 * Eventualluy these should be type generic (in theory).
 *
 * Define CINT_FORCE_FALLBACK to force usage of fallbacks
 */


#if (defined(__clang__) || defined(__GNUC__)) && !defined(CINT_FORCE_FALLBACK)
    #define ADD_OVERFLOW(a, b, res) __builtin_add_overflow(a, b, res)
    #define SUB_OVERFLOW(a, b, res) __builtin_sub_overflow(a, b, res)
#else
    #error "Unsupported compiler"
#endif



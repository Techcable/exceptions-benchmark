#include <stdlib.h>
#include <stdbool.h>

#include "exception.h"
#include "intoverflow.h"

/**
 * We use fibonnaci function here because
 *
 * 1. It's simple and fast (using iterative version).
 *    - However, it should not be simple enough for compiler to optimize away completely
 * 2. It overflows int64_t relatively quickly. Specifically fibonnaci(93) is the smallest n where fibonnaci(n)<2**63-1
 */
struct fib_ctx {
    int64_t target;
    bool throw_error;
};

void* run_fibonnaci(void *raw_ctx) {
    int64_t *res_box = malloc(sizeof(int64_t));
    int64_t a = 1, b = 1;
    struct fib_ctx *ctx = (struct fib_ctx*) raw_ctx;
    int64_t target = ctx->target;
    bool throw_error = ctx->throw_error;
    /*
     * Use fast iterative calculation. Algorithm:
     * def fibonnaci(n):
     *     a, b = 1, 1
     *     for k in range(1, n):
     *         a, b = b, a + b
     *     return a
     */
    for (int64_t k = 1; k < target; k++) {
        int64_t new_b = 0;
        if (ADD_OVERFLOW(a, b, &new_b)) {
            if (throw_error) {
                throw_exception(create_simple_exception("Integer overflow"));
            } else {
                abort();
            }
        }
        a = b;
        b = new_b;
    }
    *res_box = a;
    return res_box;
}

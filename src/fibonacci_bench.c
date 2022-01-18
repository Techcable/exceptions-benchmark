#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>

#include "exception.h"
#include "intoverflow.h"
#include "idiot_argparse.h"

/**
 * We use fibonacci function here because
 *
 * 1. It's simple and fast (using iterative version).
 *    - However, it should not be simple enough for compiler to optimize away completely (hopefully)
 * 2. It overflows int64_t relatively quickly. Specifically fib(93) is the smallest n where fibonacci(n)<2**63-1
 */
struct fib_ctx {
    int64_t target;
    bool abort_on_error;
};

void* run_fibonacci(void *raw_ctx) {
    int64_t *res_box = malloc(sizeof(int64_t));
    int64_t a = 1, b = 1;
    struct fib_ctx *ctx = (struct fib_ctx*) raw_ctx;
    int64_t target = ctx->target;
    bool abort_on_error = ctx->abort_on_error;
    assert(target > 0);
    /*
     * Use fast iterative calculation. Algorithm:
     * def fibonacci(n):
     *     a, b = 1, 1
     *     for k in range(1, n):
     *         a, b = b, a + b
     *     return a
     */
    for (int64_t k = 1; k < target; k++) {
        int64_t new_b = 0;
        if (ADD_OVERFLOW(a, b, &new_b)) {
            if (abort_on_error) {
                abort();
            } else {
                throw_exception(create_simple_exception("Integer overflow"));
            }
        }
        a = b;
        b = new_b;
    }
    *res_box = a;
    return res_box;
}

struct parsed_flags {
    bool abort_on_error;
    bool dont_catch;
};


int main(int argc, char *argv[]) {
    struct arg_parser parser = init_args(argc, argv);
    struct parsed_flags flags = {};
    int64_t *targets = malloc(argc * sizeof(int));
    int num_targets = 0;
    while (has_flag_args(&parser)) {
        static const char *ABORT_ALIASES[] = {"abort", NULL};
        static const struct arg_config ABORT_CONFIG = {.flag = true, .short_name = "a", .aliases = ABORT_ALIASES};
        static const struct arg_config DEFAULT_FLAG_CONFIG = {.flag = true};
        if (match_arg(&parser, "abort-on-error", &ABORT_CONFIG)) {
            flags.abort_on_error = true;
        } else if (match_arg(&parser, "dont-catch", &DEFAULT_FLAG_CONFIG)) {
            flags.dont_catch = true;
        } else {
            fprintf(stderr, "Unknown flag %s\n", current_arg(&parser));
            return 1;
        }
    }
    // NOTE: We reuse fib context
    struct fib_ctx ctx = {.abort_on_error = flags.abort_on_error};
    while (has_args(&parser)) {
        char *target_str = consume_arg(&parser);
        errno = 0;
        int64_t target = strtol(target_str, NULL, 0);
        if (target == 0 && errno != 0) {
            fprintf(stderr, "Unable to parse %dth integer: %s\n", num_targets + 1, target_str);
            perror("Invalid value");
            return 1;
        }
        if (target <= 0) {
            fprintf(stderr, "Integer must be > 0: %lld\n", target);
            return 1;
        }
        targets[num_targets++] = target;
    }
    if (num_targets == 0) {
        fprintf(stderr, "Must specify at least one target argument\n");
        return 1;
    }
    for (int target_idx = 0; target_idx < num_targets; target_idx++) {
        int64_t target = targets[target_idx];
        Exception *exc = NULL;
        // Begin bench: Keep all IO outside of this section
        clock_t start = clock();
        assert(((int64_t) start) != -1);
        void *res = NULL;
        if (flags.dont_catch) {
            res = run_fibonacci(&ctx);
        } else {
            ExceptionResult exc_res = try_catch_exception(
                run_fibonacci,
                &ctx
            );
            if (exc_res.exception != NULL) {
                exc = exc_res.exception;
                res = NULL;
            } else {
                res = exc_res.success;;
            }
        }
        clock_t end = clock();
        // End bench
        if (res == NULL) {
            assert(exc != NULL);
            fprintf(stderr, "fib(%lld) threw exception: %s\n", target, get_exception_msg(exc));
        } else {
            int64_t res_int = *((int64_t*) res);
            printf("fib(%lld) -> %lld\n", target, res_int);
        }
        int64_t diff = ((int64_t) end) - ((int64_t) start);
        double millis = (((double) diff) / CLOCKS_PER_SEC) * 1000;
        if (res == NULL) free(res); // Be a good citizen
        printf("Time for fib(%lld): %f.3 millis\n", target, millis);
    }
}



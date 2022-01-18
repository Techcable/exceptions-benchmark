exceptions-benchmark
=====================
This was an *unsientific* benchmark to try and benchmark the overhead of using `setjmp` for exceptions (in C).


## Conclusions
Any conclusions I draw here are marred by the imperfections of the test :(

I would say `setjmp` is fairly low overhead but defintely not zero. In my more reliable (iterative) tests, it added about 7 nanoseconds of ovrhead (relative to baseline of about 20).

Throwing w/ `longjmp` is within the same order of magnitude of normal execution. It added maybe 2x overhead at most (so 40 ns vs 20 ns).

This is in noticeable contrast to C++ "zero-cost" exception handling.

All of this can be expected based on the traditional implemention strategy for setjmp/longjmp. That simply saves callee-saved registers and does a jmp (as opposed to C++ stack unwinding).

See musl libc:
- for setjmp: [x86-64](https://git.musl-libc.org/cgit/musl/tree/src/setjmp/x86_64/setjmp.s?h=v1.2.2) moves 8 words [ARM64](https://git.musl-libc.org/cgit/musl/tree/src/setjmp/aarch64/setjmp.s?h=v1.2.2) moves 20 words
- longjmp does the same thing, just in reverse then does a jmp

AFAIK, all unixes behave this way by default. The windows implementation does *NOT*. It uses a similar method to C++ exception unwinding, just done in a way that doesn't call destructors (they call it "forced unwinding").

Also `libunwind` (available for most POSIX systems) offers a [unwinding-based setjmp](https://www.nongnu.org/libunwind/man/libunwind-setjmp(3).html), further blurring the distinction ;)

### Methodology
I regret this, but here we go:

I used `clock()` instead of a real benchmarking framework (and compiled with optimizations disabled). 

The current benchmark is based upon iterative computation of the fibonnaci sequence. On my Apple M1, each invocation of Fib(90) took about 1 or 2 *nanoseconds*.

Calling `setjmp` didn't add any appreciateble overhead for a single iteration. (maybe 2x at worst), `longjmp` maybe added 2x.

Then I iterated in a loop of about 90 iterations for Fib(90).

In this case, each individual invocation of Fibonnaci. The overall thing took about 20-30 ns and `setjmp` added the noted +7 ns.

However, in this case, actually throwing the exception w/ `longjmp` didn't make much of a difference


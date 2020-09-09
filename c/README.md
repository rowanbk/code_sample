Compiled using: aarch64-linux-gnu-gcc -O3 -mcpu=cortex-a53 -flax-vector-conversions -static intrinsic_implementation.c -o intrinsic_out
Ran with: qemu-aarch64 intrinsic_out -v

Mean over 10000 runs with an 128x128 matrix using C implementation: 48ms

Mean over 10000 runs with a 128x128 matrix using intrinsic implementation: 34ms

Speedup = 14ms (29%)

Difference between previous answer and this on:
- Changed matrix size to 128x128 to mimic ML applications
- Average across 10000 trials
- Loop unrolling in both for major speedup
- Inlining in C implementation (not a major improvement)
- Looked for places to use other optimization techniques such as if/else, nothing I tried offered further improvement
- Tested for buffer padding issues using -Wpadded, none found

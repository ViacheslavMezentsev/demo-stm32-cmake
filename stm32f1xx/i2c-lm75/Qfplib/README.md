# Qfplib-M0-tiny for RT-Thread

## A free ARM Cortex-M0 floating-point library in 1 kbyte


## Introduction

Qfplib-M0-tiny is a library of IEEE 754 single-precision floating-point arithmetic routines for microcontrollers based on the ARM Cortex-M0 core (ARMv6-M architecture). It should also run on Cortex-M3 and Cortex-M4 microcontrollers and will give reasonable performance, but it is not optimised for these devices.

Many Cortex-M0 microcontrollers have very little program memory available, and so the primary design goal was to minimise the code size of the library without sacrificing too much in speed or in usefulness. To that end it provides correctly rounded (to nearest, even-on-tie) addition, subtraction, multiplication and division operations, and sine, cosine, tangent, arctangent, logarithm, exponential and square root functions that give a high degree of accuracy. There are also conversion functions between floating-point values and signed or unsigned integer or fixed-point values. The library fits in 1 kbyte of program memory.

If you can afford the luxury of an additional 200 bytes in code size fast divide and square root functions (that do not guarantee correctly rounded results) are also available.

## How to Obtain

```
 RT-Thread online packages  --->
    system packages  --->
        acceleration: Assembly language or algorithmic acceleration packages  --->
            [*] Qfplib-M0-tiny: a free ARM Cortex-M0 floating-point library in 1 kbyte
```

## Licence

Qfplib-M0-tiny is open source, licensed under version 2 of the [GNU GPL](http://www.gnu.org/licenses/). Use at your own risk. If you wish to enquire about alternative licensing please use the e-mail address on the [home page](https://www.quinapalus.com/index.html).

## Builds

Qfplib-M0-tiny can be built in different ways depending on which functions you need. The functions included are controlled by the symbols `include_faster`, `include_conversions` and `include_scientific` in the source code.

The build always includes the four basic arithmetic operations `qfp_fadd`, `qfp_fsub`, `qfp_fmul` and `qfp_fdiv`, plus the comparison function `qfp_fcmp`.

If the symbol `include_conversions` is set to 1 then conversion routines between floating-point values and integers or fixed-point values are included. These are `qfp_float2int`, `qfp_float2fix`, `qfp_int2float`, `qfp_fix2float`, `qfp_float2uint`, `qfp_float2ufix`, `qfp_uint2float` and `qfp_ufix2float`.

If the symbol `include_scientific` is set to 1 (which implies setting `include_conversions` to 1), then the following functions are included: `qfp_fcos`, `qfp_fsin`, `qfp_ftan`, `qfp_fatan2`, `qfp_fexp`, `qfp_fln` and `qfp_fsqrt`.

If the symbol `include_faster` is set to 1 then a faster but less accurate floating-point division routine `qfp_fdiv_fast` is also included, as is a fast square root routine `qfp_fsqrt_fast`.

## Code size

| Variant                                  | Directives                                                   | Functions provided                                           | Code size        |
| ---------------------------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ---------------- |
| Basic                                    | `.equ include_faster,0` `.equ include_conversions,0` `.equ include_scientific,0` | `qfp_fadd` `qfp_fsub` `qfp_fmul` `qfp_fdiv` `qfp_fcmp`       | 0x17e=382 bytes  |
| Basic plus fast division and square root | `.equ include_faster,1` `.equ include_conversions,0` `.equ include_scientific,0` | `qfp_fadd` `qfp_fsub` `qfp_fmul` `qfp_fdiv` `qfp_fdiv_fast` `qfp_fcmp` `qfp_fsqrt_fast` | 0x244=580 bytes  |
| Basic plus conversions                   | `.equ include_faster,0` `.equ include_conversions,1` `.equ include_scientific,0` | `qfp_fadd` `qfp_fsub` `qfp_fmul` `qfp_fdiv` `qfp_fcmp` `qfp_float2int` `qfp_float2fix` `qfp_int2float` `qfp_fix2float` `qfp_float2uint` `qfp_float2ufix` `qfp_uint2float` `qfp_ufix2float` | 0x1e2=482 bytes  |
| All functions                            | `.equ include_faster,0` `.equ include_conversions,1` `.equ include_scientific,1` | `qfp_fadd` `qfp_fsub` `qfp_fmul` `qfp_fdiv` `qfp_fcmp` `qfp_float2int` `qfp_float2fix` `qfp_int2float` `qfp_fix2float` `qfp_float2uint` `qfp_float2ufix` `qfp_uint2float` `qfp_ufix2float` `qfp_fcos` `qfp_fsin` `qfp_ftan` `qfp_fatan2` `qfp_fexp` `qfp_fln` `qfp_fsqrt` | 0x3f8=1016 bytes |
| De luxe                                  | `.equ include_faster,1` `.equ include_conversions,1` `.equ include_scientific,1` | `qfp_fadd` `qfp_fsub` `qfp_fmul` `qfp_fdiv` `qfp_fdiv_fast` `qfp_fcmp` `qfp_float2int` `qfp_float2fix` `qfp_int2float` `qfp_fix2float` `qfp_float2uint` `qfp_float2ufix` `qfp_uint2float` `qfp_ufix2float` `qfp_fcos` `qfp_fsin` `qfp_ftan` `qfp_fatan2` `qfp_fexp` `qfp_fln` `qfp_fsqrt` `qfp_fsqrt_fast` | 0x4c0=1216 bytes |

Qfplib-M0-tiny does not use any static storage. Stack use is parsimonious and statically analysable; recursion is not used.

## Code size comparison against other embedded libraries

The standard floating-point library routines that come with the GCC cross-compiler for the Cortex-M0 core occupy about 2700 bytes *for the four basic arithmetic functions alone*; a trivial program that does nothing but call `cosf` compiles to 7.5 kbyte of code.

Texas Instruments has released information giving the code size for a number of simple benchmarks compiled for a range of microcontrollers in Table A-4 of [document SLAA205C](http://www.ti.com/litv/pdf/slaa205c). It appears that the sizes given do not include start-up code (look for example at the compiled size of the ‘8-bit 2-dim matrix.c’ benchmark, which includes a 64 byte constant array).

The ‘floating-point math.c’ benchmark exercises floating-point addition, multiplication and division with very little overhead, and this allows us to make a meaningful comparison with the ‘Basic’ version of Qfplib-M0-tiny.

GCC compiles the body of this benchmark to a rather profligate 54 bytes and the total code size when linked with the ‘Basic’ version of Qfplib-M0-tiny is 382+54=436 bytes.

On page 26 of [a presentation on LPC1100 series microcontrollers](http://www.nxp.com/wcm_documents/techzones/microcontrollers-techzone/Presentations/cortex.m0.code.density.pdf) NXP claims that using the ARM ‘microlib’ library the same benchmark compiles to approximately 620 bytes. So even though microlib makes no attempt at IEEE 754 compliance, it is nevertheless about 50% larger than Qfplib-M0-tiny.

Information from the above sources is summarised in the following table.

| Processor/library                                     | Benchmark code size in bytes |
| ----------------------------------------------------- | ---------------------------- |
| Texas Instruments MSP430F5438                         | 1102                         |
| Microchip dsPIC                                       | 2020                         |
| Microchip PIC24                                       | 2020                         |
| Renesas H8/300H                                       | 1104                         |
| Maxim MAXQ20                                          | 1172                         |
| Freescale HCS12                                       | 2082                         |
| Atmel ATxmega64A1                                     | 1080                         |
| Generic ARM7TDMI (Thumb)                              | 1832                         |
| Intel MCS-51                                          | 2190                         |
| Microchip PIC18F242                                   | 1400                         |
| Atmel ATmega8                                         | 1088                         |
| NXP LPC1100 series ARM Cortex-M0 (microlib)           | 620                          |
| **NXP LPC1100 series ARM Cortex-M0 (Qfplib-M0-tiny)** | **436**                      |

The cross-platform fixed-point arithmetic library libfixmath can run on the Cortex-M0 core. According to [this page](https://code.google.com/p/libfixmath/wiki/Microcontroller_Usage) its implementation of the `atan2` function is about four times larger than the whole of Qfplib-M0-tiny.

ARM provides a range of floating-point arithmetic routines as part of its CMSIS library. Unfortunately, at least based on an inspection of the part that has been released under a non-proprietary licence (see [here](https://github.com/pfalcon/ARM-CMSIS-BSD)), the implementations are poor and do not appear to have been tested thoroughly.

ARM's floating-point cosine routine includes a table of constants, not shared with any other functions, that is already larger than the whole of Qfplib-M0-tiny. The routine produces results about half as accurate as those of Qfplib-M0-tiny.

## Speed

The following table compares cycle counts for Qfplib-M0-tiny against other libraries. Qfplib-M0-tiny and GCC library results are average values for non-exceptional arguments to the functions, include calling overhead, and are approximate. They were measured using an LPC11U68 microcontroller with single-cycle flash memory. Results for the Micro Digital ‘GoFast’ library—presumably optimised for speed rather than size, judging by its name—are inferred from the timings given on [this page](http://www.smxrtos.com/ussw/gofast/gofast_arm_gnu.htm) for an ARM7TDMI-based processor. The comparison here may not be not strictly fair to Qfplib-M0-tiny as it is not clear from their description whether Micro Digital’s library exploits features available on that processor but not on the Cortex-M0: for example, ARM mode is considerably faster and more flexible than Thumb mode, and the long multiply instructions can be used to advantage in several of the routines. Micro Digital do not appear to provide public information on the code size of their library. The implementation of the basic functions does not appear to be IEEE 754 compliant with regard to rounding.

| Function         | **Qfplib-M0-tiny cycles** | GCC library cycles | ‘GoFast’ library cycles |
| ---------------- | ------------------------- | ------------------ | ----------------------- |
| `qfp_fadd`       | **150**                   | 102                | 182                     |
| `qfp_fsub`       | **151**                   | 108                | 181                     |
| `qfp_fmul`       | **165**                   | 166                | 144                     |
| `qfp_fdiv`       | **323**                   | 475                | 799                     |
| `qfp_fdiv_fast`  | **187**                   | -                  | -                       |
| `qfp_fcmp`       | **27**                    | -                  | 103                     |
| `qfp_fcos`       | **579**                   | 3350               | 393                     |
| `qfp_fsin`       | **567**                   | 3300               | 394                     |
| `qfp_ftan`       | **748**                   | 6140               | 1090                    |
| `qfp_fatan2`     | **703**                   | 4930               | 2041                    |
| `qfp_fexp`       | **536**                   | 1930               | 372                     |
| `qfp_fln`        | **808**                   | 3960               | 1321                    |
| `qfp_fsqrt`      | **717**                   | 460                | 1590                    |
| `qfp_fsqrt_fast` | **161**                   | -                  | -                       |

The ARM CMSIS implementations of the scientific functions, despite their name ‘FastMath’, appear to be many times slower than Qfplib-M0-tiny. For example, the average execution time for ARM's cosine function (compiled using GCC) is about 3880 cycles, virtually independent of the optimisation flags used.

The libfixmath implementations of the basic arithmetic functions are much faster than the floating-point implementations in Qfplib-M0-tiny; the implementation of square root is of comparable speed; and the scientific functions appear to be much slower.

## Limitations and deviations from the IEEE 754 standard

Except as noted below, on input and output, NaNs are converted to infinities, denormals are flushed to zero, and negative zero is converted to positive zero. The result of the square root function is not always correctly rounded according to IEEE 754; see the next section for more on function accuracy.

## Function ranges and accuracy

Subject to the limitations and deviations mentioned above, the functions `qfp_fadd`, `qfp_fsub`, `qfp_fmul` and `qfp_fdiv` all produce correctly rounded (to nearest, even-on-tie) results. This has been verified on real hardware against the default library supplied with the GCC cross-compiler using the [Berkeley TestFloat](http://www.jhauser.us/arithmetic/TestFloat.html) suite, plus a further billion or so test cases, both random and contrived.

In the following table, ‘ulp’ means ‘unit in last place’. Where a relative accuracy is quoted (‘(R)’), this means the error in units of the least significant bit of the mantissa of the result. Where an absolute accuracy is quoted (‘(A)’), it means the error in units of 2–24.

| Function                                      | Valid argument range | Test                                                         | Mean signed (systematic) error | Mean unsigned error | RMS error      | Remarks                                                      |
| --------------------------------------------- | -------------------- | ------------------------------------------------------------ | ------------------------------ | ------------------- | -------------- | ------------------------------------------------------------ |
| `qfp_fdiv_fast`                               | Any                  | 10000000 random pairs *x*, *y* where 1≤*x*<2; 1≤*y*<2        | 0.001244 ulp (R)               | 0.2518 ulp (R)      | 0.2917 ulp (R) | Relative accuracy is independent of the exponents of the arguments; result is exact when divisor is a power of 2 |
| `qfp_fcos`                                    | –128<*x*<+128        | All values from –π to +π in steps of 2–22                    | 0.004518 ulp (A)               | 0.3905 ulp (A)      | 0.5054 ulp (A) | Relative accuracy is poor where the result is near zero; argument is clipped to valid range; `qfp_fcos(0)==1` |
| All values from –128 to +128 in steps of 2–17 | 0.007372 ulp (A)     | 0.6243 ulp (A)                                               | 0.7517 ulp (A)                 |                     |                |                                                              |
| `qfp_fsin`                                    | –128<*x*<+128        | All values from –π to +π in steps of 2–22                    | 0.2654 ulp (A)                 | 0.4541 ulp (A)      | 0.5713 ulp (A) | Relative accuracy is poor where the result is near zero; argument is clipped to valid range; `qfp_fsin(0)==0` |
| All values from –128 to +128 in steps of 2–17 | 0.2647 ulp (A)       | 0.6531 ulp (A)                                               | 0.7935 ulp (A)                 |                     |                |                                                              |
| `qfp_ftan`                                    | –128<*x*<+128        | All values from –1 to +1 in steps of 2–24                    | 0.1730 ulp (A)                 | 0.4314 ulp (A)      | 0.6083 ulp (A) | Relative accuracy is poor where the result is near zero; absolute accuracy is poor where the result approaches infinity; `qfp_ftan` is calculated by dividing the results of `qfp_fsin` and `qfp_fcos` using `qfp_fdiv_fast` (if available; otherwise `qfp_fdiv` is used); `qfp_ftan(0)==0` |
| All values from –1.5 to +1.5 in steps of 2–23 | –0.3961 ulp (A)      | 1.543 ulp (A)                                                | 4.259 ulp (A)                  |                     |                |                                                              |
| `qfp_fatan2`                                  | Any                  | 10000000 random pairs *x*, *y* where –2≤*x*<2; –2≤*y*<2, not both less than 0.25 in absolute value | 0.1705 ulp (A)                 | 0.6925 ulp (A)      | 0.8570 ulp (A) | Result is independent of any overall offset added to the exponents of both arguments; `qfp_fatan2(0,1)==0` |
| `qfp_fexp`                                    | Any                  | All values from –87 to +88 in steps of 2–17                  | –0.1207 ulp (R)                | 0.2936 ulp (R)      | 0.3571 ulp (R) | Returns zero for *x*≤–87.33655 and +infinity for x≥88.72284; `qfp_fexp(0)==1` |
| `qfp_fln`                                     | *x*>0                | All values from 2–4 to 24 in steps of 2–20                   | 0.03885 ulp (A)                | 0.8292 ulp (A)      | 1.053 ulp (A)  | Returns –infinity for *x*≤0                                  |
| `qfp_fsqrt`                                   | *x*≥0                | All representable values from 1 to 4                         | 0.3376 ulp (A)                 | 0.5889 ulp (A)      | 0.7152 ulp (A) | Relative accuracy is independent of even offsets to exponent; returns –infinity for *x*<0; `qfp_fsqrt(0)==0; qfp_fsqrt_fast(0)==0; qfp_fsqrt(1)==1; qfp_fsqrt_fast(1)==1` |
| `qfp_fsqrt_fast`                              | –0.04438 ulp (A)     | 0.6969 ulp (A)                                               | 0.8676 ulp (A)                 |                     |                |                                                              |

`qfp_fcmp` returns zero if its arguments are equal (negative zero is equal to positive zero) or plus or minus one if its first argument is respectively greater than or less than its second. Input denormals are not flushed to zero; and NaNs are compared respecting their signs and treating them as values beyond ±infinity.

`qfp_float2int(x)` is equivalent to `qfp_float2fix(x,0)`.

`qfp_float2fix(x,y)` converts a floating-point value *x* to a signed fixed point value, with *y* bits after the binary point. The result is rounded towards –infinity. *y* can be from –256 to +256. The result is clamped to the available (signed) output range.

`qfp_int2float(x)` is equivalent to `qfp_fix2float(x,0)`.

`qfp_fix2float(x,y)` converts a signed fixed point value *x* with *y* bits after the binary point to a floating-point value, correctly rounded (to nearest, even-on-tie). *y* can be from –256 to +256. If the result is outside the representable range, ±infinity is returned as appropriate.

`qfp_float2uint`, `qfp_float2ufix`, `qfp_uint2float` and `qfp_ufix2float` are the same as `qfp_float2int`, `qfp_float2fix`, `qfp_int2float` and `qfp_fix2float`, but work with unsigned fixed-point and integer values.

## Qfpio: string conversion functions

Qfpio, part of the Qfplib-M0-tiny download from release 20151029, includes two functions for converting between floating-point values and ASCII strings. The functions are `qfp_float2str(float f,char*s,unsigned int fmt)`, which converts a float to a string with flexible control of formatting, and `qfp_str2float(float*f,char*p,char**endptr)`, which performs the reverse conversion. Again, the emphasis is on compactness, without compromising too much in speed or accuracy: the total code size for the two functions is just over 800 bytes. Qfpio does not call any of the other functions in Qfplib-M0-tiny and so can be compiled independently.

## Files

- qfplib.s, the source code to qfplib. The GNU assembler syntax is used.

- qfplib.h, a C header file giving prototypes for the qfplib functions.

- qfpio.s, the source code to qfpio, routines for converting between
strings and floating-point values.

- qfpio.h, a C header file giving prototypes for the qfpio functions.

Visit http://www.quinapalus.com/qfplib.html for more information.

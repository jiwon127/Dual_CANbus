
                .extern   __text_end__
                .extern   __data_beg__
                .extern   __data_end__
                .extern   __bss_beg__
                .extern   __bss_end__
                .extern   __stack_end__
                .extern   __ctors_start__
                .extern   __ctors_end__
                .extern   __libc_init_array
                .extern   main

                .global   _start


                .thumb_func
Default_Handler:                        /* Default exception handler */
infinite_loop:  b infinite_loop         /* Hold registers for inspection */



                .thumb_func
_start:

/* Copy initialized data from flash to RAM */

copy_data:      ldr   r1, DATA_BEG
                ldr   r2, TEXT_END
                ldr   r3, DATA_END
                subs  r3, r3, r1        /* Length of initialized data */
                beq   zero_bss          /* Skip if none */

copy_data_loop: ldrb  r4, [r2], #1      /* Read byte from flash */
                strb  r4, [r1], #1      /* Store byte to RAM */
                subs  r3, r3, #1        /* Decrement counter */
                bgt   copy_data_loop    /* Repeat until done */

/* Zero uninitialized data (bss) */

zero_bss:       ldr   r1, BSS_BEG
                ldr   r3, BSS_END
                subs  r3, r3, r1        /* Length of uninitialized data */
                beq   call_system       /* Skip if none */

                mov   r2, #0

zero_bss_loop:  strb  r2, [r1], #1      /* Store zero */
                subs  r3, r3, #1        /* Decrement counter */
                bgt   zero_bss_loop     /* Repeat until done */

call_system:    bl    SystemInit        /* Call SystemInit() */

/* Call C++ constructors */
call_cxxctors:  ldr   r0, CTORS_BEG
                ldr   r1, CTORS_END
                subs  r1, r1, r0        /* Length of ctors section */
                beq   call_main         /* Skip if no constructors */

cxxctors_loop:  ldr   r2, [r0], #4      /* Load next constructor address */
                push  {r0,r1}           /* Save registers */
                blx   r2                /* Call constructor */
                pop   {r0,r1}           /* Restore registers */
                subs  r1, r1, #4        /* Decrement counter */
                bgt   cxxctors_loop     /* Repeat until done */

// call_ctors:     bl    __libc_init_array /* Call constructors */

call_main:      mov   r0, #0            /* argc=0 */
                mov   r1, #0            /* argv=NULL */
                bl    main              /* Call main() */

/* main() should never return, but if it does, just do nothing forever. */
/* Should probably put processor into sleep mode instead. */

                b     .

/* These are filled in by the linker */

                .align  4
TEXT_END:       .word __text_end__
DATA_BEG:       .word __data_beg__
DATA_END:       .word __data_end__
BSS_BEG:        .word __bss_beg__
BSS_END:        .word __bss_end__
CTORS_BEG:      .word __ctors_start__
CTORS_END:      .word __ctors_end__

/* libstdc++ needs this */
//
//                .bss
//                .align    4
//__dso_handle:   .word
//                .global   __dso_handle
//                .weak     __dso_handle
//
//                .end
/* Interrupt Vector for STM32F10X Connectivity-Line (CL) MCUs */

/* Copyright (C)2013, Philip Munts, President, Munts AM Corp.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE. */

                .section  .startup, "x"
                .global _vectors

/* Use Default_handler for all exceptions and interrupts, unless another
 * handler is provided elsewhere. */

                .macro    IRQ handler
                .word   \handler
                .weak   \handler
                .set    \handler, Default_Handler
                .endm

/* Exception vector table */

/* Hardware interrupts common to all Cortex-M3 */

_vectors:       .word   __stack_end__
                .word   _start
                IRQ     NMI_Handler
                IRQ     HardFault_Handler
                IRQ     MemManage_Handler
                IRQ     BusFault_Handler
                IRQ     UsageFault_Handler
                .word   0
                .word   0
                .word   0
                .word   0
                IRQ     SVC_Handler
                IRQ     DebugMon_Handler
                .word   0
                IRQ     PendSV_Handler
                IRQ     SysTick_Handler
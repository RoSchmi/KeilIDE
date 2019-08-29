/*****************************************************************************/
/* startup_LPC11xx.s: Startup file for LPC11xx device series                 */
/*****************************************************************************/

/* Changes of the template were made by RoSchmi to circumvent issues with the not defined symbol __START */
/* Solution: __Start was defined to 0x0000.0000 in the linker script and set to -start in this startup Code */ 

/* ------------------- */
/* Stack Configuration */
    .syntax unified
    .arch armv6-m

    .section .stack
    .align 3
#ifdef __STACK_SIZE
    .equ    Stack_Size, __STACK_SIZE
#else
    .equ    Stack_Size, 0xc00
#endif
    .globl    __StackTop
    .globl    __StackLimit
__StackLimit:
    .space    Stack_Size
    .size __StackLimit, . - __StackLimit
__StackTop:
    .size __StackTop, . - __StackTop

/* ------------------ */
/* Heap Configuration */

    .section .heap
    .align 3
#ifdef __HEAP_SIZE
    .equ    Heap_Size, __HEAP_SIZE
#else
    .equ    Heap_Size, 0
#endif
    .globl    __HeapBase
    .globl    __HeapLimit
__HeapBase:
    .if    Heap_Size
    .space    Heap_Size
    .endif
    .size __HeapBase, . - __HeapBase
__HeapLimit:
    .size __HeapLimit, . - __HeapLimit


/* ----------------------------------------------------- */
/* Vector Table */

    .section .isr_vector
    .align 2
    .globl __isr_vector
__isr_vector:
    .long    __StackTop            /* Top of Stack */
    .long    Reset_Handler         /* Reset Handler */
    .long    NMI_Handler           /* NMI Handler */
    .long    HardFault_Handler     /* Hard Fault Handler */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    SVC_Handler           /* SVCall Handler */
    .long    0                     /* Reserved */
    .long    0                     /* Reserved */
    .long    PendSV_Handler        /* PendSV Handler */
    .long    SysTick_Handler       /* SysTick Handler */

    /* External Interrupts */
    .long   WAKEUP_IRQHandler           /* 16+ 0: Wakeup PIO0.0          */
    .long   WAKEUP_IRQHandler           /* 16+ 1: Wakeup PIO0.1          */
    .long   WAKEUP_IRQHandler           /* 16+ 2: Wakeup PIO0.2          */
    .long   WAKEUP_IRQHandler           /* 16+ 3: Wakeup PIO0.3          */
    .long   WAKEUP_IRQHandler           /* 16+ 4: Wakeup PIO0.4          */
    .long   WAKEUP_IRQHandler           /* 16+ 5: Wakeup PIO0.5          */
    .long   WAKEUP_IRQHandler           /* 16+ 6: Wakeup PIO0.6          */
    .long   WAKEUP_IRQHandler           /* 16+ 7: Wakeup PIO0.7          */
    .long   WAKEUP_IRQHandler           /* 16+ 8: Wakeup PIO0.8          */
    .long   WAKEUP_IRQHandler           /* 16+ 9: Wakeup PIO0.9          */
    .long   WAKEUP_IRQHandler           /* 16+10: Wakeup PIO0.10         */
    .long   WAKEUP_IRQHandler           /* 16+11: Wakeup PIO0.11         */
    .long   WAKEUP_IRQHandler           /* 16+12: Wakeup PIO1.0          */
    .long   0                           /* 16+13: Reserved               */
    .long   SSP1_IRQHandler             /* 16+14: SSP1                   */
    .long   I2C_IRQHandler              /* 16+15: I2C                    */
    .long   TIMER16_0_IRQHandler        /* 16+16: 16-bit Counter-Timer 0 */
    .long   TIMER16_1_IRQHandler        /* 16+17: 16-bit Counter-Timer 1 */
    .long   TIMER32_0_IRQHandler        /* 16+18: 32-bit Counter-Timer 0 */
    .long   TIMER32_1_IRQHandler        /* 16+19: 32-bit Counter-Timer 1 */
    .long   SSP0_IRQHandler             /* 16+20: SSP                    */
    .long   UART_IRQHandler             /* 16+21: UART                   */
    .long   0                           /* 16+22: Reserved               */
    .long   0                           /* 16+23: Reserved               */
    .long   ADC_IRQHandler              /* 16+24: A/D Converter          */
    .long   WDT_IRQHandler              /* 16+25: Watchdog Timer         */
    .long   BOD_IRQHandler              /* 16+26: Brown Out Detect       */
    .long   0                           /* 16+27: Reserved               */
    .long   PIOINT3_IRQHandler          /* 16+28: PIO INT3               */
    .long   PIOINT2_IRQHandler          /* 16+29: PIO INT2               */
    .long   PIOINT1_IRQHandler          /* 16+30: PIO INT1               */
    .long   PIOINT0_IRQHandler          /* 16+31: PIO INT0               */


    .size    __isr_vector, . - __isr_vector

/* ----------------------------------------------------- */
/* Reset Handler */
    .text

    .thumb
    .thumb_func

    .align 2

    .globl    Reset_Handler

    .type     Reset_Handler, %function



Reset_Handler:
/*     Loop to copy data from read only memory to RAM. The ranges
 *      of copy from/to are specified by following symbols evaluated in 
 *      linker script.
 *      __etext: End of code section, i.e., begin of data sections to copy from.
 *      __data_start__/__data_end__: RAM address range that data should be
 *      copied to. Both must be aligned to 4 bytes boundary.  */

    ldr    r1, =__etext
    ldr    r2, =__data_start__
    ldr    r3, =__data_end__

    subs    r3, r2
    ble    .LC1    
.LC0:
    subs    r3, #4
    ldr    r0, [r1, r3]
    str    r0, [r2, r3]
    bgt    .LC0
.LC1:

#ifdef __STARTUP_CLEAR_BSS
/*     This part of work usually is done in C library startup code. Otherwise,
 *     define this macro to enable it in this startup.
 *
 *     Loop to zero out BSS section, which uses following symbols
 *     in linker script:
 *      __bss_start__: start of BSS section. Must align to 4
 *      __bss_end__: end of BSS section. Must align to 4
 */
    ldr r1, =__bss_start__
    ldr r2, =__bss_end__

    movs    r0, 0
.LC2:
    cmp     r1, r2
    bge     .LC2end
    str     r0, [r1]
    adds    r1, #4
    blt    .LC2
.LC2end:
#endif /* __STARTUP_CLEAR_BSS */

#ifndef __NO_SYSTEM_INIT
    bl    SystemInit
#endif

/* This was commented out by RoSchi */
/* #ifndef __START */
/* #define __START _start */
/* #endif */

/* The following line was added by RoSchmi */

__START = _start 

    bl    __START
    .pool
    .size Reset_Handler, . - Reset_Handler
    .section ".text"

/* --------------------------------------------------------
 *    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */

    .macro    def_default_handler    handler_name
    .align 1
    .thumb_func
    .weak    \handler_name
    .type    \handler_name, %function
\handler_name :
    b    .
    .size    \handler_name, . - \handler_name
    .endm
/* ----------------------------------------------------- */
/* Exception Handlers */

    def_default_handler    NMI_Handler
    def_default_handler    HardFault_Handler
    def_default_handler    SVC_Handler
    def_default_handler    DebugMon_Handler
    def_default_handler    PendSV_Handler
    def_default_handler    SysTick_Handler
    def_default_handler    Default_Handler

    def_default_handler    WAKEUP_IRQHandler
    def_default_handler    SSP1_IRQHandler
    def_default_handler    I2C_IRQHandler
    def_default_handler    TIMER16_0_IRQHandler
    def_default_handler    TIMER16_1_IRQHandler
    def_default_handler    TIMER32_0_IRQHandler
    def_default_handler    TIMER32_1_IRQHandler
    def_default_handler    SSP0_IRQHandler
    def_default_handler    UART_IRQHandler
    def_default_handler    ADC_IRQHandler
    def_default_handler    WDT_IRQHandler
    def_default_handler    BOD_IRQHandler
    def_default_handler    PIOINT3_IRQHandler
    def_default_handler    PIOINT2_IRQHandler
    def_default_handler    PIOINT1_IRQHandler
    def_default_handler    PIOINT0_IRQHandler

    .weak    DEF_IRQHandler

    .set    DEF_IRQHandler, Default_Handler

    .end

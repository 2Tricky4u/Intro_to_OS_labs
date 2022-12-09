/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>

#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)                                          \
        .globl name;            /* define global symbol for 'name' */   \
        .type name, @function;  /* symbol type is function */           \
        .align 2;               /* align function definition */         \
        name:                   /* function starts here */              \
        pushl $(num);                                                   \
        jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)                                     \
        .globl name;                                                    \
        .type name, @function;                                          \
        .align 2;                                                       \
        name:                                                           \
        pushl $0;                                                       \
        pushl $(num);                                                   \
        jmp _alltraps

.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */
TRAPHANDLER_NOEC(divide_by_zero, T_DIVIDE); //fault
TRAPHANDLER_NOEC(debug, T_DEBUG);//fault/trap
TRAPHANDLER_NOEC(non_maskable_int, T_NMI); //interrupt
TRAPHANDLER_NOEC(breakpoint_interrupt, T_BRKPT); //trap
TRAPHANDLER_NOEC(overflow, T_OFLOW);//trap
TRAPHANDLER_NOEC(bound_range_exceeded, T_BOUND);//fault
TRAPHANDLER_NOEC(invalid_opcode, T_ILLOP);//fault
TRAPHANDLER_NOEC(device_not_available, T_DEVICE);//fault
TRAPHANDLER(double_fault, T_DBLFLT);//abort
TRAPHANDLER(invalid_TSS, T_TSS); //fault
TRAPHANDLER(segment_not_present, T_SEGNP); //fault
TRAPHANDLER(stack_segment_fault, T_STACK); //fault
TRAPHANDLER(general_protection_fault, T_GPFLT);//fault
TRAPHANDLER(page_fault, T_PGFLT);//fault
TRAPHANDLER_NOEC(floating_point_exception, T_FPERR);//fault
TRAPHANDLER(alignement_check, T_ALIGN);//fault
TRAPHANDLER_NOEC(machine_check, T_MCHK); //abort
TRAPHANDLER_NOEC(simd_fp_exception, T_SIMDERR);//fault
TRAPHANDLER_NOEC(system_call, T_SYSCALL);
TRAPHANDLER_NOEC(irq_timer, IRQ_OFFSET + IRQ_TIMER);
TRAPHANDLER_NOEC(irq_kbd, IRQ_OFFSET + 1);
TRAPHANDLER_NOEC(irq_casq, IRQ_OFFSET + 2);
TRAPHANDLER_NOEC(irq_COMp2or4, IRQ_OFFSET + 3);
TRAPHANDLER_NOEC(irq_COMp1or3, IRQ_OFFSET + 4);
TRAPHANDLER_NOEC(irq_paraSound, IRQ_OFFSET + 5);
TRAPHANDLER_NOEC(irq_floppy, IRQ_OFFSET + 6);
TRAPHANDLER_NOEC(irq_parap1, IRQ_OFFSET + 7);
TRAPHANDLER_NOEC(irq_rtimeclock, IRQ_OFFSET + 8);
TRAPHANDLER_NOEC(irq_video, IRQ_OFFSET + 9);
TRAPHANDLER_NOEC(irq_open, IRQ_OFFSET + 10);
TRAPHANDLER_NOEC(irq_open2, IRQ_OFFSET + 11);
TRAPHANDLER_NOEC(irq_ps2mouse, IRQ_OFFSET + 12);
TRAPHANDLER_NOEC(irq_copro, IRQ_OFFSET + 13);
TRAPHANDLER_NOEC(irq_IDE, IRQ_OFFSET + 14);
TRAPHANDLER_NOEC(irq_IDE2, IRQ_OFFSET + 15);
TRAPHANDLER_NOEC(irq_error, IRQ_OFFSET + IRQ_ERROR);

/*
 * Lab 3: Your code here for _alltraps
 */
.globl _alltraps

_alltraps:
pushl %ds
pushl %es
/*pushl %fs
pushl %gs only in xv6*/
pushal
movw $GD_KD, %ax
movw %ax , %es
movw %ax, %ds
pushl %esp
call trap

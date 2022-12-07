/* See COPYRIGHT for copyright information. */

#ifndef JOS_KERN_TRAP_H
#define JOS_KERN_TRAP_H
#ifndef JOS_KERNEL
# error "This is a JOS kernel header; user programs should not #include it"
#endif

#include <inc/trap.h>
#include <inc/mmu.h>

/* The kernel's interrupt descriptor table */
extern struct Gatedesc idt[];
extern struct Pseudodesc idt_pd;

void trap_init(void);
void trap_init_percpu(void);
void print_regs(struct PushRegs *regs);
void print_trapframe(struct Trapframe *tf);
void page_fault_handler(struct Trapframe *);
void backtrace(struct Trapframe *);

extern void (divide_by_zero)();
extern void (debug)();
extern void (non_maskable_int)();
extern void (breakpoint_interrupt)();
extern void (overflow)();
extern void (bound_range_exceeded)();
extern void (invalid_opcode)();
extern void (device_not_available)();
extern void (double_fault)();
extern void (invalid_TSS)();
extern void (segment_not_present)();
extern void (stack_segment_fault)();
extern void (general_protection_fault)();
extern void (page_fault)();
extern void (floating_point_exception)();
extern void (alignement_check)();
extern void (machine_check)();
extern void (simd_fp_exception)();
extern void (system_call)();
extern void (irq_timer)();
extern void (irq_kbd)();
extern void (irq_casq)();
extern void (irq_COMp2or4)();
extern void (irq_COMp1or3)();
extern void (irq_paraSound)();
extern void (irq_floppy)();
extern void (irq_parap1)();
extern void (irq_rtimeclock)();
extern void (irq_video)();
extern void (irq_open)();
extern void (irq_open2)();
extern void (irq_ps2mouse)();
extern void (irq_copro)();
extern void (irq_IDE)();
extern void (irq_IDE2)();
extern void (irq_error)();

#endif /* JOS_KERN_TRAP_H */

/*
 * entry.S - Entry point to system mode from user mode
 */

#include <asm.h>
#include <segment.h>
#include <errno.h> //el nostre header dels errno

/**************************************************/
/**** Save & Restore ******************************/
/**                                              **/
/** When we change to privilege level 0 (kernel) **/
/** (through an interrupt, a system call, an     **/
/** exception ...) we must save the state of the **/
/** currently running task (save).               **/
/**                                              **/
/** Stack layout in 'systemCall':                **/
/**                                              **/
/**   0(%esp) - %ebx    \                        **/
/**   4(%esp) - %ecx     |                       **/
/**   8(%esp) - %edx     |                       **/
/**   C(%esp) - %esi     | Register saved        **/
/**  10(%esp) - %edi     |  by 'save'            **/
/**  14(%esp) - %ebp     |                       **/
/**  18(%esp) - %eax     |                       **/
/**  1C(%esp) - %ds      |                       **/
/**  20(%esp) - %es      |                       **/
/**  24(%esp) - %fs      |                       **/
/**  28(%esp) - %gs     /                        **/
/**  2C(%esp) - %eip    \                        **/
/**  30(%esp) - %cs      |                       **/
/**  34(%esp) - %eflags  |  Return context saved **/
/**  38(%esp) - %oldesp  |   by the processor.   **/
/**  3C(%esp) - %oldss  /                        **/
/**                                              **/
/**************************************************/

#define SAVE_ALL \
      pushl %gs; \
      pushl %fs; \
      pushl %es; \
      pushl %ds; \
      pushl %eax; \
      pushl %ebp; \
      pushl %edi; \
      pushl %esi; \
      pushl %edx; \
      pushl %ecx; \
      pushl %ebx; \
      movl $__KERNEL_DS, %edx;    \
      movl %edx, %ds;           \
      movl %edx, %es


#define RESTORE_ALL \
	popl %ebx; \
	popl %ecx; \
	popl %edx; \
 	popl %esi; \
 	popl %edi; \
  popl %ebp; \
  popl %eax; \
  popl %ds; \
  popl %es; \
  popl %fs; \
  popl %gs 

#define EOI \
	movb $0x20, %al; \
	outb %al, $0x20 ;

ENTRY(keyboard_handler)
	SAVE_ALL;
	EOI;
	call user_to_sys
	call keyboard_routine 
	call sys_to_user_ready
	RESTORE_ALL;
	iret;	
	
ENTRY(clock_handler)
	SAVE_ALL;
	EOI;
	call user_to_sys
	call clock_routine 
	call sys_to_user_ready
	RESTORE_ALL;
	iret;	

ENTRY(system_call_handler)
	SAVE_ALL
	cmpl $0, %EAX
	jl	err
	cmpl $MAX_SYSCALL, %EAX
	jg err   
	push %eax
	call user_to_sys
	pop %eax
	call *sys_call_table(, %EAX, 0x04) 
	push %eax
	call sys_to_user_ready
	pop %eax
	jmp fin 
err:
	movl $-ENOSYS, %EAX
fin:
	movl %EAX, 0x18(%esp)  
	RESTORE_ALL                      
	iret	

ENTRY(syscall_handler_sysenter)
	push $__USER_DS
	push %EBP// User stack address
	pushfl
	push $__USER_CS
	push 4(%EBP)        // User return address
	SAVE_ALL
	cmpl $0, %EAX
	jl sysenter_err
	cmpl $MAX_SYSCALL, %EAX
	jg sysenter_err
	push %eax
	call user_to_sys
	pop %eax
	call *sys_call_table(, %EAX, 0x04) 
	push %eax
	call sys_to_user_ready
	pop %eax
	jmp sysenter_fin
sysenter_err:
	movl $-ENOSYS, %EAX
sysenter_fin:
	movl %EAX, 0x18(%ESP)
	RESTORE_ALL
	movl (%ESP), %EDX// Return address
	movl 12(%ESP), %ECX// User stack address
	sti// Enable interrupts again
	sysexit


ENTRY(writeMSR)
	pushl %ebp
	movl %esp, %ebp
	movl 8(%ebp), %ecx //primer parametre, index
	movl 12(%ebp), %eax //segon parametre, valor a posar al MSR
	movl $0, %edx //es la part alta del registre msr que es de 64 bits
	wrmsr
	pop %ebp
	ret

ENTRY(task_switch)
	pushl %ebp
	movl %esp, %ebp
	pushl %esi
	pushl %edi
	pushl %ebx
	pushl 8(%ebp)
	call inner_task_switch
	addl $4, %esp
	popl %ebx
	popl %edi
	popl %esi
	popl %ebp
	ret


ENTRY(inner_task_switch)
	pushl %ebp
	movl %esp, %ebp
	call sys_to_user_ready  //poso a la cua el proces
	movl $tss, %eax // adressa de la tss
	//str %eax // adressa de la tss

	//movl $KERNEL_STACK_SIZE, %ecx
	movl $1024, %ecx
	movl 8(%ebp), %edx		// ebx = &t
	leal (%edx,%ecx,4), %ecx // ecx = &t->stack[KERNEL_STACK_SIZE]

	movl %ecx, 4(%eax)	// tss.esp0 = &t->stack[KERNEL_STACK_SIZE]

	push %ecx
	push $0x175
	call writeMSR // writeMSR(0x175, &t->stack[KERNEL_STACK_SIZE])
	addl $8, %esp


	movl 8(%ebp), %edx		// edx = &t
	pushl 4(%edx)	
	call set_cr3  //set_cr3(&t -> task.dir_pages_baseAddr)
	addl $4, %esp
	call current
	movl %ebp, 0x10(%eax) // current() -> kernel_esp = %ebp
	
	movl 8(%ebp), %edx		// edx = &t
	movl 0x10(%edx), %esp // esp = &t -> kernel_esp
	
	call ready_to_sys  //tenim dubtes de si això pot anar aquí. No sabem on posar-ho.
	popl %ebp
	ret
	
	




	




	
	
	
	
	







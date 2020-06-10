/*
 * wrapper.S - syscall wrappers declarations
 */

#include <asm.h>




ENTRY(write) //exemple usant int
	//enllaç dinàmic
  pushl %ebp
  movl %esp, %ebp
  pushl %ebx
  //paràmetres
	movl 8(%ebp), %ebx
  movl 12(%ebp), %ecx
  movl 16(%ebp), %edx
  movl $4, %eax
  //crida
	int $0x80
  cmpl $0, %eax
  jge no_error_write
  negl %eax
  movl %eax, errno
  movl $-1, %eax
no_error_write:
  popl %ebx
  popl %ebp
  ret
   
ENTRY(gettime) //exemple usant systenter
	pushl %ebp
	movl %esp, %ebp
	//identificador
	movl $10, %eax
	//empilar registres pel retorn
	pushl %ecx
	pushl %edx
	//guardem retorn
	pushl $return
	//nou enllaç dinamic
	push %ebp
	movl %esp, %ebp
	//crida
	sysenter 
return:
	//restaurem enllaç dinamici estat de la pila
	popl %ebp
	addl $4, %esp
	popl %edx
	popl %ecx
	cmpl $0, %eax
  jge no_error_gettime
  negl %eax
  movl %eax, errno
  movl $-1, %eax
no_error_gettime:
  popl %ebp
  ret

ENTRY(getpid)
	pushl %ebp
	movl %esp, %ebp
	//identificador de la syscall
	movl $20, %eax
	//empilar registres pel retorn (aquests cal guardarlos quan es fa un sysenter)
	pushl %ecx
	pushl %edx
	//guardem la etiqueta de retorn
	push $return_getpid
	//nou enllacç dinamic
	pushl %ebp
	movl %esp, %ebp
	sysenter
return_getpid:
	//restaurem enllaç dinamic
	popl %ebp
	//elimino l'adreça de retorn
	addl $4, %esp
	//restauro registres
	popl %edx
	popl %ecx
	//comporovo que no hi hagi errors
	cmpl $0, %eax
	jge no_error_getpid //salto a la resta de la rutina
	//tracto l'error
	negl %eax
	movl %eax, errno
	movl $-1, %eax
no_error_getpid:    //he d'execurtar sempre aixo en quaslevol cas per sortir de la rutina.  
	popl %ebp
	ret


ENTRY(fork)
	pushl %ebp
	movl %esp, %ebp
	//syscall ID
	movl $2, %eax
	//empilar registres pel retorn (aquests cal guardarlos quan es fa un sysenter)
	pushl %ecx
	pushl %edx
	//guardem la etiqueta de retorn
	push $return_fork
	//nou enllacç dinamic
	pushl %ebp
	movl %esp, %ebp
	sysenter
return_fork:
	//restaurem enllaç dinamic
	popl %ebp
	//elimino l'adreça de retorn
	addl $4, %esp
	//restauro registres
	popl %edx
	popl %ecx
	//comporovo que no hi hagi errors
	cmpl $0, %eax
	jge no_error_fork //salto a la resta de la rutina
	//tracto l'error
	negl %eax
	movl %eax, errno
	movl $-1, %eax
no_error_fork:    //he d'execurtar sempre aixo en quaslevol cas per sortir de la rutina.  
	popl %ebp
	ret

ENTRY(exit)
	pushl %ebp
	movl %esp, %ebp
	//syscall ID
	movl $1, %eax
	//empilar registres pel retorn (aquests cal guardarlos quan es fa un sysenter)
	pushl %ecx
	pushl %edx
	//guardem la etiqueta de retorn
	push $return_exit
	//nou enllacç dinamic
	pushl %ebp
	movl %esp, %ebp
	sysenter
return_exit:
	//restaurem enllaç dinamic
	popl %ebp
	//elimino l'adreça de retorn
	addl $4, %esp
	//restauro registres
	popl %edx
	popl %ecx
	popl %ebp
	ret


ENTRY(get_stats)
	pushl %ebp
	movl %esp, %ebp
	//syscall ID
	movl $35, %eax
	//empilar registres pel retorn (aquests cal guardarlos quan es fa un sysenter)
	pushl %ecx
	pushl %edx
	pushl %ebx
//paràmetres
	movl 8(%ebp), %ebx
  movl 12(%ebp), %ecx
	//guardem la etiqueta de retorn
	push $return_getstats
	//nou enllacç dinamic
	pushl %ebp
	movl %esp, %ebp
	sysenter
return_getstats:
	//restaurem enllaç dinamic
	popl %ebp
	//elimino l'adreça de retorn
	addl $4, %esp
	//restauro registres
	popl %ebx
	popl %edx
	popl %ecx
	//comporovo que no hi hagi errors
	cmpl $0, %eax
	jge no_error_getstats //salto a la resta de la rutina
	//tracto l'error
	negl %eax
	movl %eax, errno
	movl $-1, %eax
no_error_getstats:    //he d'execurtar sempre aixo en quaslevol cas per sortir de la rutina.  
	popl %ebp
	ret

ENTRY(fflush)
	pushl %ebp
	movl %esp, %ebp
	//syscall ID
	movl $18, %eax
	//empilar registres pel retorn (aquests cal guardarlos quan es fa un sysenter)
	pushl %ecx
	pushl %edx
	pushl %ebx
//paràmetres
	movl 8(%ebp), %ebx
  movl 12(%ebp), %ecx
	//guardem la etiqueta de retorn
	push $return_fflush
	//nou enllacç dinamic
	pushl %ebp
	movl %esp, %ebp
	sysenter
return_fflush:
	//restaurem enllaç dinamic
	popl %ebp
	//elimino l'adreça de retorn
	addl $4, %esp
	//restauro registres
	popl %ebx
	popl %edx
	popl %ecx
	//comporovo que no hi hagi errors
	cmpl $0, %eax
	jge no_error_fflush //salto a la resta de la rutina
	//tracto l'error
	negl %eax
	movl %eax, errno
	movl $-1, %eax
no_error_fflush:    //he d'execurtar sempre aixo en quaslevol cas per sortir de la rutina.  
	popl %ebp
	ret



ENTRY(get_key)
	pushl %ebp
	movl %esp, %ebp
	//syscall ID
	movl $15, %eax
	//empilar registres pel retorn (aquests cal guardarlos quan es fa un sysenter)
	pushl %ecx
	pushl %edx
	pushl %ebx
//paràmetres
	movl 8(%ebp), %ebx
	//guardem la etiqueta de retorn
	push $return_get_key
	//nou enllacç dinamic
	pushl %ebp
	movl %esp, %ebp
	sysenter
return_get_key:
	//restaurem enllaç dinamic
	popl %ebp
	//elimino l'adreça de retorn
	addl $4, %esp
	//restauro registres
	popl %ebx
	popl %edx
	popl %ecx
	//comporovo que no hi hagi errors
	cmpl $0, %eax
	jge no_error_get_key //salto a la resta de la rutina
	//tracto l'error
	negl %eax
	movl %eax, errno
	movl $-1, %eax
no_error_get_key:    //he d'execurtar sempre aixo en quaslevol cas per sortir de la rutina.  
	popl %ebp
	ret



ENTRY(put_screen)
	pushl %ebp
	movl %esp, %ebp
	//syscall ID
	movl $16, %eax
	//empilar registres pel retorn (aquests cal guardarlos quan es fa un sysenter)
	pushl %ecx
	pushl %edx
	pushl %ebx
//paràmetres
	movl 8(%ebp), %ebx
	//guardem la etiqueta de retorn
	push $return_put_screen
	//nou enllacç dinamic
	pushl %ebp
	movl %esp, %ebp
	sysenter
return_put_screen:
	//restaurem enllaç dinamic
	popl %ebp
	//elimino l'adreça de retorn
	addl $4, %esp
	//restauro registres
	popl %ebx
	popl %edx
	popl %ecx
	//comporovo que no hi hagi errors
	cmpl $0, %eax
	jge no_error_put_screen //salto a la resta de la rutina
	//tracto l'error
	negl %eax
	movl %eax, errno
	movl $-1, %eax
no_error_put_screen:    //he d'execurtar sempre aixo en quaslevol cas per sortir de la rutina.  
	popl %ebp
	ret



ENTRY(set_fps)
	pushl %ebp
	movl %esp, %ebp
	//syscall ID
	movl $17, %eax
	//empilar registres pel retorn (aquests cal guardarlos quan es fa un sysenter)
	pushl %ecx
	pushl %edx
	pushl %ebx
//paràmetres
	movl 8(%ebp), %ebx
	//guardem la etiqueta de retorn
	push $return_set_fps
	//nou enllacç dinamic
	pushl %ebp
	movl %esp, %ebp
	sysenter
return_set_fps:
	//restaurem enllaç dinamic
	popl %ebp
	//elimino l'adreça de retorn
	addl $4, %esp
	//restauro registres
	popl %ebx
	popl %edx
	popl %ecx
	//comporovo que no hi hagi errors
	cmpl $0, %eax
	jge no_error_set_fps //salto a la resta de la rutina
	//tracto l'error
	negl %eax
	movl %eax, errno
	movl $-1, %eax
no_error_set_fps:    //he d'execurtar sempre aixo en quaslevol cas per sortir de la rutina.  
	popl %ebp
	ret


ENTRY(sbrk)
	pushl %ebp
	movl %esp, %ebp
	//syscall ID
	movl $19, %eax
	//empilar registres pel retorn (aquests cal guardarlos quan es fa un sysenter)
	pushl %ecx
	pushl %edx
	pushl %ebx
//paràmetres
	movl 8(%ebp), %ebx
	//guardem la etiqueta de retorn
	push $return_sbrk
	//nou enllacç dinamic
	pushl %ebp
	movl %esp, %ebp
	sysenter
return_sbrk:
	//restaurem enllaç dinamic
	popl %ebp
	//elimino l'adreça de retorn
	addl $4, %esp
	//restauro registres
	popl %ebx
	popl %edx
	popl %ecx
	//comporovo que no hi hagi errors
	cmpl $-1, %eax
	jne no_error_sbrk //salto a la resta de la rutina
	//tracto l'error
	movl $12, errno //ENOMEM
no_error_sbrk:    //he d'execurtar sempre aixo en quaslevol cas per sortir de la rutina.  
	popl %ebp
	ret


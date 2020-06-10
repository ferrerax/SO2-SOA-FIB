/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>
#include <utils.h>

union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));

#if 1  //Hem descomentat això
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}
#endif

extern struct list_head blocked;
extern int sys_put_screen(char *s);
//int spf_quantum;   //es la variable que diu quants ticks han de passar entre cada put screen
int spf_PID = 0;
int quantum_ticks;
int spf_ticks; //Per contar els segons (18 tics en cada segon).
int spf_sem;   //variable que es decrementa segons els frames que s'han mostrat
int gfps; //variable que conté els fps a mostrar
char *spf_last_screen;
struct list_head freequeue, readyqueue, priorityqueue;
struct task_struct * idle_task;
void writeMSR(int index_MSR, int value_MSR);

/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void init_stat(struct task_struct *t)
{

	t->estat.user_ticks = 0;
	t->estat.system_ticks = 0;
	t->estat.blocked_ticks = 0;
	t->estat.ready_ticks = 0;
	t->estat.elapsed_total_ticks = get_ticks();
	t->estat.total_trans = 0;
	t->estat.remaining_ticks = t->quantum; // a l'inici del proces 

}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");
	while(1)
	{
	;
	}
}

void init_idle (void)
{
	struct list_head *task_head = list_first(&freequeue);  //prenem l'element de la llista de fre
	list_del(task_head); //l'eliminem de la llista de free pq ja no esta free
	struct task_struct *idle_pcb = list_head_to_task_struct(task_head);
	idle_pcb->PID=0;	
	union task_union* utask = (union task_union *) idle_pcb; //agafem el task_union corresponent al PCB del idle
	utask->stack[KERNEL_STACK_SIZE - 1] = (unsigned long) cpu_idle; //empilem la adressa de retorn
	utask->stack[KERNEL_STACK_SIZE - 2] = (unsigned long) 0;// empilem el fals ebp
	utask->task.kernel_esp = &(utask->stack[KERNEL_STACK_SIZE - 2]);	
	 
	allocate_DIR(idle_pcb); //assignem directori de pagines
	idle_task = idle_pcb;
	
}

void init_task1(void)
{
	
	struct list_head *task_head = list_first(&freequeue);  //prenem l'element de la llista de fre
	list_del(task_head); //l'eliminem de la llista de free pq ja no esta free

	struct task_struct *task1_pcb = list_head_to_task_struct(task_head);
	task1_pcb->PID=1;	
	task1_pcb->brk=(void *)L_HEAP_START;
	set_quantum(task1_pcb,QUANTUM);  //els altres processos l'heredaran
  task1_pcb->nice=1;	
	allocate_DIR(task1_pcb); // assignem directori al proces
	set_user_pages(task1_pcb); // assignem l'espai d'adresses del proces
	
	tss.esp0 = KERNEL_ESP((union task_union *)task1_pcb);	
	task1_pcb->kernel_esp = (unsigned long *) KERNEL_ESP((union task_union *)task1_pcb);	
	writeMSR(0x175, KERNEL_ESP((union task_union *)task1_pcb));
	set_cr3(task1_pcb->dir_pages_baseAddr);
	init_stat(task1_pcb);

}

void init_sched()
{
	INIT_LIST_HEAD( &freequeue );
	INIT_LIST_HEAD( &readyqueue );
	INIT_LIST_HEAD( &priorityqueue );
	quantum_ticks = -1;
	spf_ticks = -1;
	spf_sem = -1;
	int i;
	for(i=0; i<NR_TASKS; i++)
		list_add( &(task[i].task.list), &freequeue );

}


int get_quantum (struct task_struct *t){
	return t->quantum;
}

void set_quantum(struct task_struct *t, int new_quantum){
	t->quantum = new_quantum;
}

void update_sched_data_rr (void){
	--quantum_ticks;
	
}

int needs_sched_rr(void){
	if (quantum_ticks > 0) return 0;
	if (list_empty(&readyqueue) && list_empty(&priorityqueue)){  //Si la llista es buida no canviis pero compta trans.
		quantum_ticks = current()->quantum;
		current()->estat.total_trans++;
		return 0;
	}
	return 1;
}

void update_process_state_rr (struct task_struct *t, struct list_head *dst_queue)
{
	struct list_head *llista = &t->list; //obtinc el camp llista
	if (!(llista->prev == NULL && llista->next == NULL)) //Soc a una llista cal que m'esborri d'on soc
	{	
		list_del(llista); //m'he esborrat
	}
	if (dst_queue)  					//si no ha de ser running l'afegeixo a la llista que em passen (perque si has de ser running ja va be deixarlo lliure)
		list_add_tail(llista, dst_queue);
}

void sched_next_rr(void){
	struct list_head *list_nou;
	int nice = current()-> nice;
	if (nice) { // nice=1  --> Proces de prioritat baixa
		if (list_empty(&priorityqueue)) list_nou = list_first(&readyqueue);
		else list_nou = list_first(&priorityqueue);
	}
	else 	list_nou = list_first(&readyqueue);// nice=0 --> process de prioritat alta
	list_del(list_nou);
	union task_union *nova_taska = (union task_union *) list_head_to_task_struct (list_nou);
	int final_quantum = ((struct task_struct *)nova_taska)->quantum; // nova taska == nou quantum
	quantum_ticks = (!nice)? final_quantum: final_quantum * 5; 
	task_switch(nova_taska); 
}

void schedule (void)
{
	update_sched_data_rr();
	if (needs_sched_rr())
	{
		struct task_struct *curr = current();
		if(curr->nice)	update_process_state_rr(curr, &readyqueue);   //temps esgotat
		else update_process_state_rr(curr, &priorityqueue);
		sched_next_rr();
	}
}

void schedule_fps(void){
	if (spf_ticks!=-1){
		--spf_ticks;
		if (spf_ticks <= 0 && spf_PID == current()->PID){ // sino comprovem que estiguem en lespai dadresse que toca ens mengem un page fault
			spf_ticks += 18; // no es = 18 perk hem de tenir en compte els ticks que passen quan spf_PID != current()->PID, per aixo el += 18
			spf_sem = gfps;
			if(spf_last_screen != NULL) sys_put_screen(spf_last_screen); 
		}
	}
}

void user_to_sys()
{
	struct stats *st = &current()->estat;
  st->user_ticks += get_ticks() - st->elapsed_total_ticks;
  st->elapsed_total_ticks = get_ticks();
  st->remaining_ticks = quantum_ticks;

}

void sys_to_user_ready()
{
	struct stats *st = &current()->estat;
  st->system_ticks += get_ticks() - st->elapsed_total_ticks;
  st->elapsed_total_ticks = get_ticks();
  st->remaining_ticks = quantum_ticks;

}


void ready_to_sys()
{
	struct stats *st = &current()->estat;
  st->ready_ticks += get_ticks() - st->elapsed_total_ticks;
  st->elapsed_total_ticks = get_ticks();
  st->remaining_ticks = quantum_ticks;
	st->total_trans++;
// caldria incrementar total_trans?

}


struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}
struct task_struct * get_task(int pid)
{
    for(int i = 0; i < NR_TASKS; i++) {
        struct task_struct *task_struct = &(task[i].task);
        if (task_struct->PID == pid) return task_struct;
    }
    return 0;
}


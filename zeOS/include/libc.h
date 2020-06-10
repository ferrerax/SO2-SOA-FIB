/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */
 
#ifndef __LIBC_H__
#define __LIBC_H__

#include <stats.h>
 

int write(int fd, char *buffer, int size);

void itoa(int a, char *b);

int strlen(char *a);

int getpid();

int gettime(); //Nova crida a sistema gettime.

int fork();

void exit();

int get_stats(int pid, struct stats *st);

int get_key(char *c);

int put_screen(char *s);

int set_fps(int fps);

int fflush();

void* sbrk (int increment);

void perror(void);

void *get_scenario();

void del_scenario();  //borra l'ultim.

#endif  /* __LIBC_H__ */


/*
 * circular_buffer.h - Definició del buffer circular i les seves funcions
 */
#ifndef _CIRCULAR_H
#define _CIRCULAR_H

#define MAX_VAL 8

struct circular_buffer {
	char buffer[8];
	int read;
	int write;
};


int circular_empty(struct circular_buffer *buff);
int circular_read(struct circular_buffer *buff, char *rd);
void circular_write(struct circular_buffer *buff, char wr);
void circular_init(struct circular_buffer *buff);

#endif

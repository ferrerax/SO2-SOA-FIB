#include <circular_buffer.h>
#include <errno.h>

void circular_init(struct circular_buffer *buff){
	buff->read = 0;
	buff->write = 0;
	int i;
	for (i = 0; i<MAX_VAL; i++){
		buff->buffer[i] = '\x00';       //valor null
	}
}

int circular_empty(struct circular_buffer *buff){
	return buff->buffer[buff->read] == '\x00'; 
}

int circular_read(struct circular_buffer *buff, char *rd){
		if (! circular_empty(buff)){
			*rd = buff->buffer[buff->read];
			buff->buffer[buff->read++] = '\x00';
			buff->read = buff->read % MAX_VAL;
			return 1;
		} 
		*rd = '\x00';
		return -ENOINP;
}

void circular_write(struct circular_buffer *buff, char wr){
		buff->buffer[buff->write++] = wr;
		buff->write = buff->write % MAX_VAL;
}

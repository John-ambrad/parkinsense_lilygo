#include "circular_buff.h"

void circular_buff_init(circular_buff_t* circ_buff){
    circ_buff->head = 0;
    circ_buff->tail = 0;
    circ_buff->count = 0;
    circ_buff->buffer[BUFFER_SIZE] = {0};
}

bool circular_buff_empty(circular_buff_t* circ_buff){

    return circ_buff->count != 0;

}


void circular_buff_write(bma_accel_data_t input, circular_buff_t* circ_buff){

    if(circ_buff->count < BUFFER_SIZE){
    circ_buff->buffer[circ_buff->head] = input;
    circ_buff->count += 1;
    circ_buff->head = (circ_buff->head+1) % BUFFER_SIZE;
    }
    else{
    circ_buff->buffer[circ_buff->head] = input;
    circ_buff->head = (circ_buff->head+1) % BUFFER_SIZE;
    circ_buff->tail = (circ_buff->tail+1) % BUFFER_SIZE;


    }
    //should I put checks for overwrites? 
}


bma_accel_data_t circular_buff_read (circular_buff_t* circ_buff){

    bma_accel_data_t buf_output = circ_buff->buffer[circ_buff->tail];
    circ_buff->tail = (circ_buff->tail+1) % BUFFER_SIZE;

    return buf_output;
}
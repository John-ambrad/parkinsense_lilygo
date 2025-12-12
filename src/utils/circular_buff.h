#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H


#include <stdint.h>
#include <stdbool.h>
#include "../hardware/motion.h" //using bma_accel_t struct


#ifndef BUFFER_SIZE
#define BUFFER_SIZE (1024)
#endif

typedef struct circular_buff_s{
  bma_accel_data_t buffer[BUFFER_SIZE];
  int head;
  int tail;
  int count; //check elements in array, useful for checking status of buff

} circular_buff_t;
//should this be a class?

void circular_buff_init(circular_buff_t* circ_buff);
void circular_buff_write(bma_accel_data_t input, circular_buff_t* circ_buff);
bool circular_buff_empty(circular_buff_t* circ_buff);
bma_accel_data_t circular_buff_read (circular_buff_t* circ_buff);







#endif //CIRCULAR_BUFFER_H

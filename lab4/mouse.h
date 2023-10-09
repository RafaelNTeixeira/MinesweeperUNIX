#pragma once
#include <lcom/lcf.h>
#include "statemachine.h"

#define DELAY_US2 25000

bool (check_input_buffer)();
bool (check_output_buffer)();
int (mouse_subscribe_int)(uint8_t *bit_no);
int (mouse_unsubscribe_int)();
void (mouse_ih)();
void (buildPacket)(struct packet * pacote);
int (mouse_enable_data_report)();
int (mouse_disable_data_report)();
int(mouse_write_cmd)(uint32_t cmd, uint8_t *resp);
int (mouse_stream)();
int (mouse_remote)();
int (mouse_read_data)();
uint16_t convert_2_complement(uint8_t number, uint8_t msb, struct packet *pkt);

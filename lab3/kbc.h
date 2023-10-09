#pragma once
#include <lcom/lcf.h>

extern uint8_t scan_bytes[2];
extern bool codecomplete;
extern int nrbytes;
int (kbc_subscribe_int)(uint8_t *bit_no);
int (kbc_unsubscribe_int)();


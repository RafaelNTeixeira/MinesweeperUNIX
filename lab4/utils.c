#include <lcom/lcf.h>
#include <stdint.h>

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t val = 0;
  int returned_value = sys_inb(port, &val);
  *value = val;
  return returned_value;
}

uint16_t convert_2_complement(uint8_t number, uint8_t msb, struct packet *pkt) {
  if (pkt->bytes[0] & msb) return number - 256;
  return number;
}

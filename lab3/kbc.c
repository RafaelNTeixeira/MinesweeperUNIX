#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>
#include "i8042.h"


static int kbd_id = KBD_IRQ1;
uint8_t scan_bytes[2];
bool codecomplete = false;
int nrbytes = 0;


int (kbc_subscribe_int)(uint8_t *bit_no) {
    kbd_id = KBD_IRQ1;
    *bit_no = kbd_id;
    return sys_irqsetpolicy(KBD_IRQ1, (IRQ_REENABLE | IRQ_EXCLUSIVE), &kbd_id);
}

int (kbc_unsubscribe_int)() {
    return sys_irqrmpolicy(&kbd_id);
}

int(kbd_get_status)(uint8_t *status) {
    return util_sys_inb(KBC_ST_REG, status);
}

int (kbd_read_out_buffer)(uint8_t *output) {
    return util_sys_inb(KBC_OUT_BUF, output);
}

/*
int (kbc_enable_interrupt)() {
  uint8_t cmd_b = 0;

  kbc_write_cmd(KBC_CMD_REG, RCB);
  util_sys_inb(KBC_OUT_BUF, &cmd_b);
  cmd_b |= INT;
  kbc_write_cmd(KBC_CMD_REG, WCB);
  kbc_write_cmd(KBC_ARGS, cmd_b);

  return 0;
}
*/

bool kbc_can_read(uint8_t *st) {
  return !(*st & KBC_ERR_SIG & AUX);
}

int (kbc_read)(uint8_t *data) {
  uint8_t st;

  if (kbd_get_status(&st))
      return PARITY_ERR;

  if (st & OBF) {
    if (!kbc_can_read(&st))
      return TIMEOUT_ERR;

    if (util_sys_inb(KBC_OUT_BUF, data))
      return IBF_ERR;

    return 0;
  }
  return AUX_ERR;
}

void (kbc_ih)() {
  if (codecomplete == true) {
    codecomplete = false;
    nrbytes = 0;
  }

  int err;
  uint8_t scancode;
  if ((err = kbc_read(&scancode)) != 0) {
    printf("There was a problem while reading the scancode. Error code: %d\n", err);
  }

  scan_bytes[nrbytes] = scancode;
  nrbytes++;

  if ((nrbytes == 2) || (scancode != TWO_BYTE_SCAN)) {
    codecomplete = true;
  }
}





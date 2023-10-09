#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include <stdbool.h>
#include <stdint.h>
#include "i8042.h"
#include "kbc.h"
#include "timer.h"

extern int cnt;
extern int counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("./trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("./output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  uint8_t bit_no;
  kbc_subscribe_int(&bit_no);
  uint32_t irq_set = BIT(bit_no);

  message msg;
  int ipc_status;

  while (scan_bytes[nrbytes - 1] != ESC_BREAK) {
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & irq_set) {
            kbc_ih();
            if(codecomplete) {
              kbd_print_scancode(!(scan_bytes[nrbytes - 1] & MAKE_BREAK_DIFF), nrbytes, scan_bytes); // i+1 because size = 2 bytes
            }
        }
        break;

      default:
        break;
      }
    }
  }
  kbc_unsubscribe_int();
  kbd_print_no_sysinb(cnt);

  return 0;
}

/*
int(kbd_test_poll)() {
  while(scancode != ESC_BREAK) {
    if(kbc_read(&scancode)) { 
      tickdelay(micro_delay(DELAY_US));
      continue;
    }

    if (scancode == TWO_BYTE_SCAN) {
     scan_bytes[i] = scancode;
     i++;
     continue;
    }

    scan_bytes[i] = scancode;
    kbd_print_scancode(!(scancode & MAKE_BREAK_DIFF), i+1, scan_bytes);
    i = 0;
  }
  kbc_enable_interrupt();
  kbd_print_no_sysinb(cntr);

  return 0;
}
*/

int(kbd_test_timed_scan)(uint8_t n) {
  uint8_t bit_no_kbd;
  kbc_subscribe_int(&bit_no_kbd);
  uint32_t irq_set_kbd = BIT(bit_no_kbd);

  uint8_t bit_no_timer;
  timer_subscribe_int(&bit_no_timer);
  uint32_t irq_set_timer = BIT(bit_no_timer);

  message msg;
  int ipc_status;

  while ((scan_bytes[nrbytes - 1] != ESC_BREAK) && (counter / 60 < n)) {
    
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set_kbd) {
            kbc_ih();
            counter = 0;
            if (codecomplete) {
              kbd_print_scancode(!(scan_bytes[nrbytes - 1] & MAKE_BREAK_DIFF), nrbytes, scan_bytes);
              counter = 0;
            }
            
          }

          if (msg.m_notify.interrupts & irq_set_timer) {
            timer_int_handler();
          }
          break;

        default:
          break;
      }
    }
  }
  kbc_unsubscribe_int();
  timer_unsubscribe_int();
  kbd_print_no_sysinb(cnt);

  return 0;
}

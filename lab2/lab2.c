#include <lcom/lcf.h>
#include <lcom/lab2.h>
#include "i8254.h"

#include <stdbool.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

extern int counter;

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  uint8_t status_timer = 0;

  timer_get_conf(timer, &status_timer);
  return timer_display_conf(timer, status_timer, field);
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  return timer_set_frequency(timer, freq);
}

int(timer_test_int)(uint8_t time) {
  uint8_t bit_no;
  timer_subscribe_int(&bit_no);
  uint32_t irq_set = BIT(bit_no);

  int ipc_status;
  message msg;

  while (time > 0) {
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & irq_set) {
          timer_int_handler();
          if (counter % 60 == 0) { // TIMER0 runs at 60HZ - 60 cycles per second - 1 second has passed
            timer_print_elapsed_time();
            time--;
          }
        }
      break;

    default:
      break;
    }
  }
  }
  timer_unsubscribe_int();
  return 0;
}

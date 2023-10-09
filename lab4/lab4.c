#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>
#include "mouse.h"
#include "timer.h"
#include "i8042.h"

extern uint8_t scancode;
extern int counter;

uint8_t scan_bytes[3];
struct packet pkt;

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
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

extern struct packet pkt;

int (mouse_test_packet)(uint32_t cnt) {
  bool readSecond = false, readThird = false;

  if (mouse_enable_data_report() != 0) {
    printf("Data reporting data enabling failed!");
    return 1; 
  }

  uint8_t bit_no;
  mouse_subscribe_int(&bit_no);
  uint32_t irq_set = BIT(bit_no);

  message msg;
  int ipc_status;

  while (cnt != 0) {
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & irq_set) {
           mouse_ih();
            if (!readSecond && !readThird && (scancode & BIT(3))) {
              scan_bytes[0] = scancode;
              readSecond = true;
            }
            else if (readSecond) {
                scan_bytes[1] = scancode;
                readThird = true;
                readSecond = false;
              }
              else if (readThird) {
                scan_bytes[2] = scancode;
                readThird = false;
                for(unsigned int i = 0; i < 3; i++)
                  pkt.bytes[i] = scan_bytes[i];
                buildPacket(&pkt);
                mouse_print_packet(&pkt);
                cnt--;
              }
              else
                continue;
            }
            break;
      default:
        break;
      }
    }
  }
  if (mouse_unsubscribe_int() != 0) return 1;
  if (mouse_disable_data_report()) return 1;
  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
  bool readSecond = false, readThird = false;

  if (mouse_enable_data_report() != 0) {
    printf("Data reporting data enabling failed!");
    return 1; 
  }

  uint8_t bit_no_mouse;
  mouse_subscribe_int(&bit_no_mouse);
  uint32_t irq_set_mouse = BIT(bit_no_mouse);

  uint8_t bit_no_timer;
  timer_subscribe_int(&bit_no_timer);
  uint32_t irq_set_timer = BIT(bit_no_timer);

  message msg;
  int ipc_status;

  while (counter / 60 < idle_time) {
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE: 
            if (msg.m_notify.interrupts & irq_set_mouse) {
              mouse_ih();
              if (!readSecond && !readThird && (scancode & BIT(3))) {
                scan_bytes[0] = scancode;
                readSecond = true;
              }
              else if (readSecond) {
                scan_bytes[1] = scancode;
                readThird = true;
                readSecond = false;
              }
              else if (readThird) {
                scan_bytes[2] = scancode;
                readThird = false;
                for (unsigned int i = 0; i < 3; i++) {
                  pkt.bytes[i] = scan_bytes[i];
                }
                buildPacket(&pkt);
                mouse_print_packet(&pkt);
              }
              counter = 0;
              continue;
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
  if (timer_unsubscribe_int() != 0) return 1;
  if (mouse_unsubscribe_int() != 0) return 1;
  if (mouse_disable_data_report() !=0) return 1;

  return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  bool done = false;
  bool ReadSecond = false, ReadThird = false;

  if (mouse_enable_data_report() != 0) {
    printf("Data reporting data enabling failed!");
    return 1; 
  }

  uint8_t bit_no;
  mouse_subscribe_int(&bit_no);
  uint32_t irq_set = BIT(bit_no);

  message msg;
  int ipc_status;

  while(!done){
    int r;
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: 
          if (msg.m_notify.interrupts & irq_set) {
            mouse_ih();
            if (!ReadSecond && !ReadThird && (scancode & BIT(3))) {
              scan_bytes[0] = scancode;
              ReadSecond = true;
            }
            else if (ReadSecond) {
              scan_bytes[1] = scancode;
              ReadThird = true;
              ReadSecond = false;
            }
            else if (ReadThird) {
              scan_bytes[2] = scancode;
              ReadThird = false;
              for(unsigned int i = 0; i < 3; i++)
                pkt.bytes[i] = scan_bytes[i];
              buildPacket(&pkt);
              mouse_print_packet(&pkt);
              done = draw_handler(x_len, tolerance, &pkt);
            }
            else
              continue;
          }
          break;
        default:
          break; 
      }
    }
  }

  if (mouse_unsubscribe_int()!=0) return 1;
  if (mouse_disable_data_report() != 0) return 1;
  
  return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
  bool readSecond = false, readThird = false;

  while (cnt > 0) {
    if (mouse_read_data() != 0) continue;

    if (!readSecond && !readThird && (scancode & BIT(3))) {
      scan_bytes[0] = scancode;
      readSecond = true;
    }
    else if (readSecond) {
      scan_bytes[1] = scancode;
      readThird = true;
      readSecond = false;
    }
    else if (readThird) {
      scan_bytes[2] = scancode;
      readThird = false;
      for(unsigned int i = 0; i < 3; i++)
        pkt.bytes[i] = scan_bytes[i];
      buildPacket(&pkt);
      mouse_print_packet(&pkt);
      cnt--;
      tickdelay(micros_to_ticks(period*1000));
    }
    else continue;
  }

  if (sys_outb(MOUSE_CMD_REG, MOUSE_OUT_BUF) != 0) return 4;
  if (sys_outb(WCB, minix_get_dflt_kbc_cmd_byte()) != 0) return 4;

  if (mouse_stream() != 0) return 1;
  if (mouse_disable_data_report() != 0) return 1;

  return 0;
}

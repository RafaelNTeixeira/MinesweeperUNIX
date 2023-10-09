// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include <stdint.h>
#include <stdio.h>
#include "define.h"
#include "video.h"
#include "kbc.h"
#include "i8042.h"

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

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

int(video_test_init)(uint16_t mode, uint8_t delay) {
    if (change_mode(mode) != 0) return 1;
    sleep(delay);
    if(vg_exit() != 0) return 1;
    return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  if(map_virtual_machine(mode) != 0) return 1;

  if (change_mode(mode) != 0) return 1;
  
  if (vg_draw_rectangle(x, y, width, height, color) != 0) return 1;
  
  uint8_t bit_no;
  if(kbc_subscribe_int(&bit_no) != 0) return 1;
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
        }
        break;

      default:
        break;
      }
    }
  }

  if(kbc_unsubscribe_int() != 0) return 1;
  if(vg_exit() != 0) return 1;

  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  if(map_virtual_machine(mode) != 0) return 1;

  if (change_mode(mode) != 0) return 1;
  
  if (vg_draw_matrix(mode, no_rectangles, first, step) != 0) return 1;
  
  uint8_t bit_no;
  if(kbc_subscribe_int(&bit_no) != 0) return 1;
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
        }
        break;

      default:
        break;
      }
    }
  }

  if(kbc_unsubscribe_int() != 0) return 1;
  if(vg_exit() != 0) return 1;

  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  if(map_virtual_machine(0x105) != 0) return 1;

  if (change_mode(0x105) != 0) return 1;

  xpm_image_t image;
  xpm_load(xpm, XPM_INDEXED, &image);

  draw_xpm(image, x, y);

  uint8_t bit_no;
  if(kbc_subscribe_int(&bit_no) != 0) return 1;
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
        }
        break;

      default:
        break;
      }
    }
  }

  if(kbc_unsubscribe_int() != 0) return 1;
  if(vg_exit() != 0) return 1;

  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate) {
  if(map_virtual_machine(0x105) != 0) return 1;

  if(change_mode(0x105) != 0) return 1;

  if(speed == 0 || fr_rate > 60) return 1;

  xpm_image_t image;
  xpm_load(xpm, XPM_INDEXED, &image);

  draw_xpm(image, xi, yi);

  uint8_t bit_no_kbc;
  if(kbc_subscribe_int(&bit_no_kbc) != 0) return 1;
  uint32_t irq_set_kbc = BIT(bit_no_kbc);

  uint8_t bit_no_timer;
  if(timer_subscribe_int(&bit_no_timer) != 0) return 1;
  uint32_t irq_set_timer = BIT(bit_no_timer);

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
        if (msg.m_notify.interrupts & irq_set_kbc) {
          kbc_ih();
        }
        else if (msg.m_notify.interrupts & irq_set_timer) {
          timer_int_handler();
          counter = 0;
        }
        break;

      default:
        break;
      }
    }
  }

  if(timer_unsubscribe_int() != 0) return 1;
  if(kbc_unsubscribe_int() != 0) return 1;
  if(vg_exit() != 0) return 1;

  return 0;

}

int(video_test_controller)() {
  vg_vbe_contr_info_t info;

  controller_info(&info);
  vg_display_vbe_contr_info(&info);
  return 0;
}

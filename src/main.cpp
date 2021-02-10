#include "Arduino.h"
#include "fsmc_ssd1963.h"
#include "lv_examples.h"
#include "lvgl.h"
#include "Adafruit_TFTLCD_16bit_STM32.h"
#include "sram.h"

static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];

#if USE_LV_LOG != 0
/* Serial debugging */
void my_print(lv_log_level_t level, const char *file, uint32_t line, const char *dsc)
{

  Serial.printf("%s@%d->%s\r\n", file, line, dsc);
  Serial.flush();
}
#endif

/*Read the touchpad*/
// bool my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data)
// {
//     uint16_t touchX, touchY;

//     bool touched = tft.getTouch(&touchX, &touchY, 600);

//     if(!touched) {
//       data->state = LV_INDEV_STATE_REL;
//     } else {
//       data->state = LV_INDEV_STATE_PR;

//       /*Set the coordinates*/
//       data->point.x = touchX;
//       data->point.y = touchY;

//       Serial.print("Data x");
//       Serial.println(touchX);

//       Serial.print("Data y");
//       Serial.println(touchY);
//     }

//     return false; /*Return `false` because we are not buffering and no more data to read*/
// }

void setup()
{
  pinMode(PD12, OUTPUT);
  digitalWrite(PD12, HIGH);

  Serial.begin(115200); /* prepare for possible serial debug */

  lv_init();

#if USE_LV_LOG != 0
  lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

  // tft.begin(); /* TFT init */
  // tft.setRotation(1); /* Landscape orientation */
  fsmc_ssd1963_init(0, false);

  uint16_t calData[5] = {275, 3620, 264, 3532, 1};
  //tft.setTouch(calData);

  lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

  /*Initialize the display*/
  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = TFT_WIDTH;
  disp_drv.ver_res = TFT_HEIGHT;
  disp_drv.flush_cb = fsmc_ssd1963_flush;
  disp_drv.buffer = &disp_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the (dummy) input device driver*/
  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  //indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  /* Try an example from the lv_examples repository
		 * https://github.com/lvgl/lv_examples*/
  //lv_ex_btn_1();

  lv_demo_music();
  //lv_demo_widgets();
  //lv_demo_stress();
}

void loop()
{

  lv_task_handler(); /* let the GUI do its work */
  delay(5);
}

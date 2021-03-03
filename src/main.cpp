#include "Arduino.h"
#include "fsmc_ssd1963.h"
#include "lv_examples.h"
#include "lvgl.h"
#include "Adafruit_TFTLCD_16bit_STM32.h"
#include "sram.h"
#include "hasp_drv_xpt2046.h"

// #define KEY_OPEN 0x74       // Keyboard Execute
// #define KEY_HELP 0x75       // Keyboard Help
// #define KEY_PROPS 0x76      // Keyboard Menu
// #define KEY_FRONT 0x77      // Keyboard Select
// #define KEY_STOP 0x78       // Keyboard Stop
// #define KEY_AGAIN 0x79      // Keyboard Again
// #define KEY_UNDO 0x7a       // Keyboard Undo
// #define KEY_CUT 0x7b        // Keyboard Cut
// #define KEY_COPY 0x7c       // Keyboard Copy
// #define KEY_PASTE 0x7d      // Keyboard Paste
// #define KEY_FIND 0x7e       // Keyboard Find
// #define KEY_MUTE 0x7f       // Keyboard Mute
// #define KEY_VOLUMEUP 0x80   // Keyboard Volume Up
// #define KEY_VOLUMEDOWN 0x81 // Keyboard Volume Down

// #define KEY_MEDIA_PLAYPAUSE 0xe8
// #define KEY_MEDIA_STOPCD 0xe9
// #define KEY_MEDIA_PREVIOUSSONG 0xea
// #define KEY_MEDIA_NEXTSONG 0xeb
// #define KEY_MEDIA_EJECTCD 0xec
// #define KEY_MEDIA_VOLUMEUP 0xed
// #define KEY_MEDIA_VOLUMEDOWN 0xee
// #define KEY_MEDIA_MUTE 0xef
// #define KEY_MEDIA_WWW 0xf0
// #define KEY_MEDIA_BACK 0xf1
// #define KEY_MEDIA_FORWARD 0xf2
// #define KEY_MEDIA_STOP 0xf3
// #define KEY_MEDIA_FIND 0xf4
// #define KEY_MEDIA_SCROLLUP 0xf5
// #define KEY_MEDIA_SCROLLDOWN 0xf6
// #define KEY_MEDIA_EDIT 0xf7
// #define KEY_MEDIA_SLEEP 0xf8
// #define KEY_MEDIA_COFFEE 0xf9
// #define KEY_MEDIA_REFRESH 0xfa
// #define KEY_MEDIA_CALC 0xfb

// #define HID_MEDIA_PAUSE 0xB1 // pause
// #define HID_MEDIA_RECORD 0xB3
// #define HID_MEDIA_SCAN_NEXT 0xB5
// #define HID_MEDIA_SCAN_PREV 0xB6
// #define HID_MEDIA_STOP 0xB7
// #define HID_MEDIA_EJECT 0xB8
// #define HID_MEDIA_VOL_UP 0xE9
// #define HID_MEDIA_VOL_DONW 0xEA
// #define HID_MEDIA_PLAY 0xCD // play/pause

#if 0
#include "Keyboard.h"
#define SERIAL Serial // Wifi header
#else
#define SERIAL SerialUSB // USB Virtual COM port
#endif

const char *btnmap[] = {LV_SYMBOL_MUTE, LV_SYMBOL_VOLUME_MID, LV_SYMBOL_VOLUME_MAX, LV_SYMBOL_UP, "btn2", "btn3", "btn1", "btn2", "\n", "btn1", "btn2", LV_SYMBOL_LEFT, LV_SYMBOL_DOWN, LV_SYMBOL_RIGHT, "btn3", "btn1", "btn2", "\n",
                        LV_SYMBOL_PREV, LV_SYMBOL_PLAY, LV_SYMBOL_STOP, LV_SYMBOL_NEXT, "btn1", "btn2", "btn1", "btn2", "\n", "btn1", "btn2", "btn3", "btn1", "btn2", "btn3", "btn1", "btn2", ""};

static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10] __attribute__((section("ccmram")));

#if USE_LV_LOG != 0
/* Serial debugging */
void my_print(lv_log_level_t level, const char *file, uint32_t line, const char *dsc)
{

  SerialUSB.printf("%s@%d->%s\r\n", file, line, dsc);
  SerialUSB.flush();
}
#endif

static void selector_event_handler(lv_obj_t *obj, lv_event_t event)
{
  if (event == LV_EVENT_VALUE_CHANGED)
  {
    char buffer[128];
    char property[36];
    uint16_t val = 0;
    uint16_t max = 0;

    val = lv_btnmatrix_get_active_btn(obj);
    const char *txt = lv_btnmatrix_get_btn_text(obj, val);
    strncpy(buffer, txt, sizeof(buffer));

    SerialUSB.print("You pressed button ");
    SerialUSB.print(val);
    SerialUSB.print(" = ");
    SerialUSB.println(txt);
    SerialUSB.println();

    // switch (val)
    // {
    // case 0:
    //   Keyboard.press(KEY_MEDIA_MUTE);
    //   delay(10);
    //   break;
    // case 1:
    //   Keyboard.press(HID_MEDIA_VOL_DONW);
    //   delay(10);
    //   break;
    // case 2:
    //   Keyboard.press(HID_MEDIA_VOL_UP);
    //   delay(10);
    //   break;
    // case 3:
    //   Keyboard.press(KEY_UP_ARROW);
    //   delay(10);
    //   break;
    // case 4:
    //   Keyboard.press(HID_MEDIA_SCAN_PREV);
    //   delay(10);
    //   break;
    // case 5:
    //   Keyboard.press(KEY_MEDIA_VOLUMEUP);
    //   delay(10);
    //   break;
    // case 6:
    //   Keyboard.press(KEY_MEDIA_VOLUMEDOWN);
    //   delay(10);
    //   break;
    // case 7:
    //   Keyboard.press(KEY_MEDIA_MUTE);
    //   delay(10);
    //   break;
    // case 10:
    //   Keyboard.press(KEY_LEFT_ARROW);
    //   delay(10);
    //   break;
    // case 11:
    //   Keyboard.press(KEY_DOWN_ARROW);
    //   delay(10);
    //   break;
    // case 12:
    //   Keyboard.press(KEY_RIGHT_ARROW);
    //   delay(10);
    // case 16:
    //   Keyboard.press(HID_MEDIA_SCAN_PREV);
    //   delay(10);
    //   break;
    // case 17:
    //   Keyboard.press(HID_MEDIA_PLAY);
    //   delay(10);
    //   break;
    // case 18:
    //   Keyboard.press(HID_MEDIA_STOP);
    //   delay(10);
    //   break;
    // case 19:
    //   Keyboard.press(HID_MEDIA_SCAN_NEXT);
    //   delay(10);
    // default:

    //   Keyboard.print(buffer);
    // }
    // Keyboard.releaseAll();

    // set the property
    // snprintf_P(property, sizeof(property), PSTR("val\":%d,\"text"), val);
    // hasp_send_obj_attribute_str(obj, property, buffer);
  }
  else if (event == LV_EVENT_DELETE)
  {
  }
}

void my_touchpad_click(_lv_indev_drv_t *indev, lv_event_t event)
{
  switch (event)
  {
  case LV_EVENT_PRESSED:
    tone(PD13, 1000, 1);
    break;
  case LV_EVENT_RELEASED:
    //tone(PD13, 500, 5);
    break;
  }
}

/*Read the touchpad*/
bool my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  int16_t touchX, touchY;

  bool touched = XPT2046_getXY(&touchX, &touchY, true);

  if (!touched)
  {
    data->state = LV_INDEV_STATE_REL;
  }
  else
  {
    data->state = LV_INDEV_STATE_PR;

    /*Set the coordinates*/
    data->point.x = touchX;
    data->point.y = touchY;

    // SerialUSB.print("Data x");
    // SerialUSB.println(touchX);

    // SerialUSB.print("Data y");
    // SerialUSB.println(touchY);
  }

  return false; /*Return `false` because we are not buffering and no more data to read*/
}

void setup()
{
  pinMode(PD12, OUTPUT);
  digitalWrite(PD12, HIGH);

  pinMode(PD13, OUTPUT);

  SerialUSB.begin(115200); /* prepare for possible serial debug */
  delay(2000);

  lv_init();

#if USE_LV_LOG != 0
  lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

  // tft.begin(); /* TFT init */
  // tft.setRotation(1); /* Landscape orientation */
  fsmc_ssd1963_init(0, false);
  XPT2046_init(0);

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
  indev_drv.read_cb = my_touchpad_read;
  indev_drv.feedback_cb = my_touchpad_click;
  lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv);
  mouse_indev->driver.type = LV_INDEV_TYPE_POINTER;

  // lv_obj_t * label = lv_label_create(lv_layer_sys(), NULL);
  // lv_label_set_text(label, "<");
  // lv_indev_set_cursor(mouse_indev, label); // connect the object to the driver

  lv_obj_t *cursor;
  lv_obj_t *mouse_layer = lv_disp_get_layer_sys(NULL); // default display

  LV_IMG_DECLARE(mouse_cursor_icon);          /*Declare the image file.*/
  cursor = lv_img_create(mouse_layer, NULL);  /*Create an image object for the cursor */
  lv_img_set_src(cursor, &mouse_cursor_icon); /*Set the image source*/
  lv_indev_set_cursor(mouse_indev, cursor);   /*Connect the image  object to the driver*/

  /* Try an example from the lv_examples repository
		 * https://github.com/lvgl/lv_examples*/
  //lv_ex_btn_1();

  //lv_demo_music();
  //lv_demo_widgets();
  //lv_demo_stress();

  lv_obj_t *btnm = lv_btnmatrix_create(NULL, NULL);
  lv_btnmatrix_set_map(btnm, btnmap);
  lv_obj_set_size(btnm, TFT_WIDTH, TFT_HEIGHT);
  lv_scr_load(btnm);
  lv_obj_set_event_cb(btnm, selector_event_handler);

#if 0
  Keyboard.begin(); // Start HID device
  delay(2000);      // Let the PC detect the new device
  //Keyboard.print("Hello!");
#endif
}

void loop()
{
  // SerialUSB.print(".");
  lv_task_handler(); /* let the GUI do its work */
  delay(5);
}

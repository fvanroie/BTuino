/* MIT License - Copyright (c) 2020 Francis Van Roie
   For full license information read the LICENSE file in the project folder */

#ifndef HASP_DRV_XPT2046B_H
#define HASP_DRV_XPT2046B_H

#if TOUCH_DRIVER == 0x2046B

#include <XPT2046_Touchscreen_swspi.h>

#define XPT2046_HOR_RES TFT_WIDTH
#define XPT2046_VER_RES TFT_HEIGHT
#define XPT2046_X_MIN 200
#define XPT2046_Y_MIN 200
#define XPT2046_X_MAX 3800
#define XPT2046_Y_MAX 3800
#define XPT2046_AVG 4
#define XPT2046_INV 0

bool XPT2046_getXY(int16_t *touchX, int16_t *touchY, bool debug);
void XPT2046_init(uint8_t rotation);

#endif
#endif
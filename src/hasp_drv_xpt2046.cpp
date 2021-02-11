#include <XPT2046_Touchscreen_swspi.h>

#define CS_PIN PE6

//XPT2046_Touchscreen ts(CS_PIN);
#define TIRQ_PIN PC13
//XPT2046_Touchscreen ts(CS_PIN);  // Param 2 - NULL - No interrupts
//XPT2046_Touchscreen ts(CS_PIN, 255);  // Param 2 - 255 - No interrupts
XPT2046_Touchscreen ts(CS_PIN, TIRQ_PIN); // Param 2 - Touch IRQ Pin - interrupt enabled polling

void XPT2046_init(uint8_t rotation)
{
    ts.begin();
    ts.setRotation(rotation);
    delay(10);
}

bool XPT2046_getXY(int16_t *touchX, int16_t *touchY, bool debug)
{
    TS_Point p = ts.getPoint();

    *touchX = map(4095 - p.x, 156, 3913, 0, TFT_WIDTH);
    *touchY = map(p.y, 383, 3891, 0, TFT_HEIGHT);

    if (debug)
    {
        Serial.print("Pressure = ");
        Serial.print(p.z);

        Serial.print(", p.x = ");
        Serial.print(4095 - p.x);
        Serial.print(", p.y = ");
        Serial.print(p.y);

        Serial.print(", x = ");
        Serial.print(*touchY);
        Serial.print(", y = ");
        Serial.print(*touchY);
    }
    return (p.z >= 400);
}

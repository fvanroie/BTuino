#include <XPT2046_Touchscreen_swspi.h>

#define CS_PIN PE6

//XPT2046_Touchscreen ts(CS_PIN);
#define TIRQ_PIN PC13
//XPT2046_Touchscreen ts(CS_PIN);  // Param 2 - NULL - No interrupts
//XPT2046_Touchscreen ts(CS_PIN, 255);  // Param 2 - 255 - No interrupts
XPT2046_Touchscreen ts(CS_PIN, TIRQ_PIN); // Param 2 - Touch IRQ Pin - interrupt enabled polling

#define XPT2046_X_MIN 250
#define XPT2046_Y_MIN 425
#define XPT2046_X_MAX 3875
#define XPT2046_Y_MAX 3825
#define XPT2046_AVG 5

int16_t avg_buf_x[XPT2046_AVG];
int16_t avg_buf_y[XPT2046_AVG];
uint8_t avg_last;

static void xpt2046_corr(int16_t *x, int16_t *y)
{
#if XPT2046_XY_SWAP != 0
    int16_t swap_tmp;
    swap_tmp = *x;
    *x = *y;
    *y = swap_tmp;
#endif

    if ((*x) > XPT2046_X_MIN)
        (*x) -= XPT2046_X_MIN;
    else
        (*x) = 0;

    if ((*y) > XPT2046_Y_MIN)
        (*y) -= XPT2046_Y_MIN;
    else
        (*y) = 0;

    (*x) = (uint32_t)((uint32_t)(*x) * TFT_WIDTH) /
           (XPT2046_X_MAX - XPT2046_X_MIN);

    (*y) = (uint32_t)((uint32_t)(*y) * TFT_HEIGHT) /
           (XPT2046_Y_MAX - XPT2046_Y_MIN);

#if XPT2046_X_INV != 0
    (*x) = XPT2046_HOR_RES - (*x);
#endif

#if XPT2046_Y_INV != 0
    (*y) = XPT2046_VER_RES - (*y);
#endif
}

static void xpt2046_avg(int16_t *x, int16_t *y)
{
    /*Shift out the oldest data*/
    uint8_t i;
    for (i = XPT2046_AVG - 1; i > 0; i--)
    {
        avg_buf_x[i] = avg_buf_x[i - 1];
        avg_buf_y[i] = avg_buf_y[i - 1];
    }

    /*Insert the new point*/
    avg_buf_x[0] = *x;
    avg_buf_y[0] = *y;
    if (avg_last < XPT2046_AVG)
        avg_last++;

    /*Sum the x and y coordinates*/
    int32_t x_sum = 0;
    int32_t y_sum = 0;
    for (i = 0; i < avg_last; i++)
    {
        x_sum += avg_buf_x[i];
        y_sum += avg_buf_y[i];
    }

    /*Normalize the sums*/
    (*x) = (int32_t)x_sum / avg_last;
    (*y) = (int32_t)y_sum / avg_last;
}

void XPT2046_init(uint8_t rotation)
{
    ts.begin();
    ts.setRotation(rotation);
    delay(10);
}

bool XPT2046_getXY(int16_t *touchX, int16_t *touchY, bool debug)
{

    static int16_t last_x = 0;
    static int16_t last_y = 0;
    uint8_t buf;

    int16_t x = 0;
    int16_t y = 0;

    //uint8_t irq = LV_DRV_INDEV_IRQ_READ;

    TS_Point p = ts.getPoint();

    if (p.z)
    {

        // while (avg_last < XPT2046_AVG)
        // {
        //     /*Normalize Data*/
        //     int16_t x = 4095 - p.x;
        //     int16_t y = p.y;
        //     xpt2046_corr(&x, &y);
        //     xpt2046_avg(&x, &y);

        //     delay(1);
        //     p = ts.getPoint();
        // }

        /*Normalize Data*/
        int16_t x = 4095 - p.x;
        int16_t y = p.y;
        xpt2046_corr(&x, &y);
        xpt2046_avg(&x, &y);

        last_x = x;
        last_y = y;

        *touchX = x;
        *touchY = y;

        Serial.print("Pressure = ");
        Serial.print(p.z);

        Serial.print(", p.x = ");
        Serial.print(4095 - p.x);
        Serial.print(", p.y = ");
        Serial.print(p.y);

        Serial.print(", x = ");
        Serial.print(*touchY);
        Serial.print(", y = ");
        Serial.println(*touchY);

        return avg_last >= 2;
    }
    else
    {
        x = last_x;
        y = last_y;
        avg_last = 0;
        return false;
    }
}

#define pnls 8
const int _MATRIX_WIDTH=(64*pnls);
#ifndef _ESP32_LedMatrix_MultiPanel_DMA
#define _ESP32_LedMatrix_MultiPanel_DMA

#include <stdint.h>

typedef struct Point{
  int16_t x;
  int16_t y;
};

#define ESP32_I2S_CLOCK_SPEED (20000000UL)
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "esp_attr.h"
#include "esp_heap_caps.h"
#include "esp32_i2s_parallel.h"

#include "Adafruit_GFX.h"

class ESP32_LedMatrix_MultiPanel_DMA : public Adafruit_GFX {
  public:
    uint16_t _matrixbuff[((_MATRIX_WIDTH)*32)];
    uint16_t* matrixbuff;
    ESP32_LedMatrix_MultiPanel_DMA(bool _doubleBuffer = false)
      : Adafruit_GFX(_MATRIX_WIDTH, 32), doubleBuffer(_doubleBuffer) {
      initMatrixBuff();
    }
    void begin(void);
    void stop(void);
    void drawManual();
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color);
    virtual void drawPixel(Point p, uint16_t color);
    uint16_t color444(uint8_t r, uint8_t g, uint8_t b) { return ((r & 0xf) << 1) | ((uint16_t)(g & 0xf) << 6) | ((uint16_t)(b & 0xf) << 11); }
    uint16_t color555(uint8_t r, uint8_t g, uint8_t b) { return (r&0x1f) | ((uint16_t)(g & 0x1f) << 5) | ((uint16_t)(b & 0x1f) << 10); }
    uint16_t colorHSV(long hue, uint8_t sat, uint8_t val);

    void swapBuffer() {
      matrixbuff = drawBuffer();
    }

    void printScr(){
          if (timer)
          {
            timerDetachInterrupt(timer);
            timerEnd(timer);
          }
        onTimer();
    }
    void copyBuffer(uint8_t  * payload, int len, int globalIndex) {
      typedef union{
        uint16_t * u16p;
        uint8_t  * u8p;
      }MB_DATA;
      MB_DATA unitMB;
      unitMB.u8p=payload;
      memcpy(&_matrixbuff[(1920*globalIndex)],unitMB.u16p,len);
    }
  private:
    
    void initMatrixBuff() {
      matrixbuff = &_matrixbuff[0];
    }
    static void IRAM_ATTR onTimer(void);
    void IRAM_ATTR draw();

    uint16_t* drawBuffer() {
        return &_matrixbuff[0];
    }
    hw_timer_t* timer;

    uint8_t pinR1 = 25;
    uint8_t pinG1 = 26;
    uint8_t pinB1 = 27;
    uint8_t pinR2 = 21;
    uint8_t pinG2 = 22;
    uint8_t pinB2 = 23;

    uint8_t pinCLK = 18;
    uint8_t pinLAT = 32;
    uint8_t pinOE = 33;

    uint8_t pinA = 19;
    uint8_t pinB = 14;
    uint8_t pinC = 13;
    uint8_t pinD = 5;
    
    bool doubleBuffer;
    static volatile SemaphoreHandle_t timerSemaphore;
    static ESP32_LedMatrix_MultiPanel_DMA*singleton;
    
    void drawNow()
    {
    portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL_ISR(&timerMux);

    singleton->draw();

    portEXIT_CRITICAL_ISR(&timerMux);
    xSemaphoreGiveFromISR(timerSemaphore, NULL);
    }
};

#endif



#include <stdint.h>


#ifndef I2C_h
#define I2C_h

#include "hsArduino.h"

//-- MAX7311 Register --
#define MAX7311_IN_L 0x0
#define MAX7311_IN_H 0x1
#define MAX7311_OUT_L 0x2
#define MAX7311_OUT_H 0x3
#define MAX7311_POLINV_L 0x4
#define MAX7311_POLINV_H 0x5
#define MAX7311_DDR_L 0x6
#define MAX7311_DDR_H 0x7
#define MAX7311_TO_REG 0x8

class I2C {
  public:
    I2C();
    uint8_t detectI2C(char * output, uint16_t len);
    uint16_t DS1631Start(uint8_t addr);
    uint16_t DS1631Stop(uint8_t addr);
    uint16_t DS1631Temp(uint8_t addr, int16_t *temp, int16_t *stemp);

#ifdef PCF8574_SUPPORT
    uint16_t PCF8574xSet(uint8_t addr, uint8_t data);
    uint16_t PCF8574xGet(uint8_t addr, uint8_t *data);
#endif

#ifdef MAX7311_SUPPORT
    uint16_t MAX7311SetDDRw(uint8_t addr, uint16_t data);
    uint16_t MAX7311GetDDRw(uint8_t addr, uint16_t *data);
    uint16_t MAX7311SetOUTw(uint8_t addr, uint16_t data);
    uint16_t MAX7311GetOUTw(uint8_t addr, uint16_t *data);
    uint16_t MAX7311Set(uint8_t addr, uint8_t port, uint8_t state);
    uint16_t MAX7311Pulse(uint8_t addr, uint8_t port, uint16_t time);
#endif

  private:
    uint16_t DS1631ReadTemp(uint8_t addr);
    
#ifdef MAX7311_SUPPORT
    uint16_t MAX7311WriteReg(uint8_t addr, uint8_t reg, uint16_t data);
    uint16_t MAX7311ReadReg(uint8_t addr, uint8_t reg, uint16_t *data);
#endif

    uint8_t MSByte;
    uint8_t LSByte;
};

#endif

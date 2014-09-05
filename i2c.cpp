
#include "i2c.h"
#include <Wire.h>
#include <util/delay.h>


I2C::I2C() {
  Wire.begin();
}

uint16_t I2C::DS1631Start(uint8_t addr) {
  Wire.beginTransmission(addr);
  Wire.write(0x51); // Start conversion
  Wire.endTransmission();
}

uint16_t I2C::DS1631Stop(uint8_t addr) {
  Wire.beginTransmission(addr);
  Wire.write(0x22); // Stop conversion
  Wire.endTransmission();
}

uint16_t I2C::DS1631Temp(uint8_t addr, uint16_t *temp, uint16_t *stemp) {
  DS1631ReadTemp(addr);
  if (MSByte & 0x80) {
    *temp = (128 - (MSByte & 0x7F));
    *temp *= -1;
  } else {
    *temp = MSByte;
  }
  *stemp = ((LSByte >> 4) * 625);
}

uint16_t I2C::DS1631ReadTemp(uint8_t addr) {
  Wire.beginTransmission(addr);
  Wire.write(0xAA); // AA : Request Temperature
  Wire.endTransmission();
  Wire.requestFrom(int(addr), 2); // READ 2 bytes
  Wire.available(); // 1st byte
  MSByte = Wire.read(); // read a byte
  Wire.available(); // 2nd byte
  LSByte = Wire.read(); // read a byte
  Wire.endTransmission();
}

uint16_t I2C::PCF8574xSet(uint8_t addr, uint8_t data) {
  Wire.beginTransmission(addr);
  Wire.write(data);
  Wire.endTransmission();
}

uint16_t I2C::PCF8574xGet(uint8_t addr, uint8_t *data) {
  Wire.beginTransmission(addr);
  Wire.requestFrom(int(addr), 1);
  Wire.available();
  *data = Wire.read();
  Wire.endTransmission();
}

uint16_t I2C::MAX7311SetDDRw(uint8_t addr, uint16_t data) {
  MAX7311WriteReg(addr, MAX7311_DDR_L, data);
}

uint16_t I2C::MAX7311GetDDRw(uint8_t addr, uint16_t *data) {
  MAX7311ReadReg(addr, MAX7311_DDR_L, data);
}

uint16_t I2C::MAX7311SetOUTw(uint8_t addr, uint16_t data) {
  MAX7311WriteReg(addr, MAX7311_OUT_L, data);
}

uint16_t I2C::MAX7311GetOUTw(uint8_t addr, uint16_t *data) {
  MAX7311ReadReg(addr, MAX7311_OUT_L, data);
}

uint16_t I2C::MAX7311Set(uint8_t addr, uint8_t port, uint8_t state) {
  uint16_t tmp;
  MAX7311GetOUTw(addr, &tmp);
  tmp = (state ? tmp | (1 << port) : tmp & ~(1 << port));
  MAX7311SetOUTw(addr, tmp);
}

uint16_t I2C::MAX7311Pulse(uint8_t addr, uint8_t port, uint16_t time) {
  uint16_t tmp;
  MAX7311GetOUTw(addr, &tmp);
  MAX7311SetOUTw(addr, tmp ^ (1 << port));
  while(time--)
    _delay_ms(1);
  MAX7311SetOUTw(addr, tmp);
}

uint16_t I2C::MAX7311ReadReg(uint8_t addr, uint8_t reg, uint16_t *data) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.requestFrom(int(addr), 2);
  Wire.available(); // 1st byte
  LSByte = Wire.read(); // read a byte
  Wire.available(); // 2nd byte
  MSByte = Wire.read(); // read a byte
  Wire.endTransmission();
  *data = ((MSByte << 8) | LSByte);
}

uint16_t I2C::MAX7311WriteReg(uint8_t addr, uint8_t reg, uint16_t data) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(data & 0xFF);
  Wire.write((data >> 8) & 0xFF);
  Wire.endTransmission();
}


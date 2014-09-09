
#include "parser.h"
#include "i2c.h"
#include "rec868.h"

#ifdef FS20_SUPPORT
#include "fs20.h"
#endif

#include <stdio.h>

#ifndef PC6
#define PC6 6
#endif
#ifndef PC7
#define PC7 7
#endif

const struct ecmd_command_t PROGMEM ecmd_cmds[] = {
  
#ifdef MAX7311_SUPPORT
  { "max7311 setDDRw", parseMAX7311SetDDRw },
  { "max7311 getDDRw", parseMAX7311GetDDRw },
  { "max7311 setOUTw", parseMAX7311SetOUTw },
  { "max7311 set", parseMAX7311Set },
  { "max7311 pulse", parseMAX7311Pulse },
#endif

#ifdef PCF8574_SUPPORT
  { "pcf8574x set", parsePCF8574xSet },
  { "pcf8574x get", parsePCF8574xGet },
#endif

#ifdef REC868_SUPPORT
  { "rec868 start", parseRec868Start },
#endif

  { "reset", parseReset },
  { "i2c detect", parseI2cDetect },
  { "ds1631 start", parseDS1631Start },
  { "ds1631 stop", parseDS1631Stop },
  { "ds1631 temp", parseDS1631Temp },
  
#ifdef FS20_SUPPORT
  { "fs20 send", parseFS20Send },
#endif

  { "pin set", parsePinSet },
  { "help", parseHelp },
  { NULL, NULL }
};

I2C i2c;

uint16_t parser ( char *cmd, char *output, int16_t len ) {
  int ret = ECMD_ERR_PARSE_ERROR;
  int retSum = 0;
  char *text = NULL;
  bool parseAgain = true;
  //int16_t (*func)(char*, char*, uint16_t) = NULL;
  func_t func = NULL;
  uint8_t pos = 0;

  while (1) {
    text = (char *)pgm_read_word(&ecmd_cmds[pos].name);
    if (text == NULL)
      break;
    if (memcmp_P(cmd, text, strlen_P(text)) == 0) {
      cmd += strlen_P(text);
      //func = (int16_t (*)(char*, char*, uint16_t))(void*)pgm_read_word(&ecmd_cmds[pos].func);
      func = (func_t)pgm_read_word(&ecmd_cmds[pos].func);
      break;
    }
    pos++;
  }
  if (func != NULL) {
      ret = func(cmd, output, len);
  }
  if (ret == ECMD_ERR_PARSE_ERROR) {
    memcpy_P(output, PSTR("parse error"), 11);
    ret = 11;
  } else if (ret == ECMD_FINAL_OK) {
    output[0] = 'O';
    output[1] = 'K';
    ret = 2;
  } 
  return ret;
}

#ifdef MAX7311_SUPPORT

int16_t parseMAX7311SetDDRw(char *cmd, char *output, uint16_t len) {
  uint8_t adr;
  uint16_t data;
  uint8_t ret;
  sscanf_P(cmd, PSTR("%hhu %hX"), &adr, &data);
  if (adr > 0x6F)
    return ECMD_ERR_PARSE_ERROR;
  ret = i2c.MAX7311SetDDRw(adr, data);
  if (ret == 0) {
    return ECMD_FINAL_OK;
  } else {
    return ECMD_FINAL(snprintf_P(output, len, PSTR("no sensor detected")));
  }
}

int16_t parseMAX7311GetDDRw(char *cmd, char *output, uint16_t len) {
  uint8_t adr;
  uint16_t data;
  uint8_t ret;
  sscanf_P(cmd, PSTR("%hhu"), &adr);
  if (adr > 0x6F)
    return ECMD_ERR_PARSE_ERROR;
  ret = i2c.MAX7311GetDDRw(adr, &data);
  if (ret == 0) {
    return ECMD_FINAL(snprintf_P(output, len, PSTR("%X"), data));
  } else {
    return ECMD_FINAL(snprintf_P(output, len, PSTR("no sensor detected")));
  }
}

int16_t parseMAX7311SetOUTw(char *cmd, char *output, uint16_t len) {
  uint8_t adr;
  uint16_t data;
  uint8_t ret;
  sscanf_P(cmd, PSTR("%hhu %hX"), &adr, &data);
  if (adr > 0x6F)
    return ECMD_ERR_PARSE_ERROR;
  ret = i2c.MAX7311SetOUTw(adr, data);
  if (ret == 0) {
    return ECMD_FINAL_OK;
  } else {
    return ECMD_FINAL(snprintf_P(output, len, PSTR("no sensor detected")));
  }
}

int16_t parseMAX7311Set(char *cmd, char *output, uint16_t len) {
  uint8_t adr;
  uint8_t bit;
  uint8_t state;
  uint8_t ret;
  sscanf_P(cmd, PSTR("%hhu %hhu %hhu"), &adr, &bit, &state);
  if (adr > 0x6F || bit > 15)
    return ECMD_ERR_PARSE_ERROR;
  ret = i2c.MAX7311Set(adr, bit, state);
  if (ret == 0) {
    return ECMD_FINAL_OK;
  } else {
    return ECMD_FINAL(snprintf_P(output, len, PSTR("no sensor detected")));
  }
}

int16_t parseMAX7311Pulse(char *cmd, char *output, uint16_t len) {
  uint8_t adr;
  uint8_t bit;
  uint16_t time;
  uint8_t ret;
  sscanf_P(cmd, PSTR("%hhu %hhu %hu"), &adr, &bit, &time);
  if (adr > 0x6F || bit > 15)
    return ECMD_ERR_PARSE_ERROR;
  if (time > 1000)
    time = 1000;
  ret = i2c.MAX7311Pulse(adr, bit, time);
  if (ret == 0) {
    return ECMD_FINAL_OK;
  } else {
    return ECMD_FINAL(snprintf_P(output, len, PSTR("no sensor detected")));
  }
}

#endif

#ifdef PCF8574_SUPPORT
int16_t parsePCF8574xSet(char *cmd, char *output, uint16_t len) {
  uint8_t adr, chip, value;
  sscanf_P(cmd, PSTR("%hhu %hhu %hhx"), &adr, &chip, &value);
  if (chip == 0) {
    adr += I2C_SLA_PCF8574;
  } else {
    adr += I2C_SLA_PCF8574A;
  }
  i2c. PCF8574xSet(adr, value);
  return ECMD_FINAL(snprintf_P(output, len, PSTR("0x%X"), value));
}

int16_t parsePCF8574xGet(char *cmd, char *output, uint16_t len) {
  uint8_t adr, chip, data;
  sscanf_P(cmd, PSTR("%hhu %hhu"), &adr, &chip);
  if (chip == 0) {
    adr += I2C_SLA_PCF8574;
  } else {
    adr += I2C_SLA_PCF8574A;
  }
  i2c.PCF8574xGet(adr, &data);
  return ECMD_FINAL(snprintf_P(output, len, PSTR("0x%X"), data));
}
#endif

#ifdef REC868_SUPPORT
int16_t parseRec868Start(char *cmd, char *output, uint16_t len) {
  rec868_global.stat.wett = TRUE;
  rec868_global.stat.hell = TRUE;
  rec868_global.stat.fs20 = TRUE;
  rec868_start();
  return ECMD_FINAL_OK;
}
#endif

int16_t parseDS1631Start(char *cmd, char *output, uint16_t len) {
  uint8_t adr;
  sscanf_P(cmd, PSTR("%hhu"), &adr);
  if (adr > 7)
    return ECMD_ERR_PARSE_ERROR;
  uint16_t ret = i2c.DS1631Start(I2C_SLA_DS1631 + adr);
  if (ret == 0xffff)
    return ECMD_FINAL(snprintf_P(output, len, PSTR("no sensor detected")));
  return ECMD_FINAL_OK;
}

int16_t parseDS1631Stop(char *cmd, char *output, uint16_t len) {
  uint8_t adr;
  sscanf_P(cmd, PSTR("%hhu"), &adr);
  if (adr > 7)
    return ECMD_ERR_PARSE_ERROR;
  uint16_t ret = i2c.DS1631Stop(I2C_SLA_DS1631 + adr);
  if (ret == 0xffff)
    return ECMD_FINAL(snprintf_P(output, len, PSTR("no sensor detected")));
  return ECMD_FINAL_OK;
}

int16_t parseDS1631Temp(char *cmd, char *output, uint16_t len) {
  uint8_t adr;
  int16_t temp;
  int16_t stemp;
  sscanf_P(cmd, PSTR("%hhu"), &adr);
  if (adr > 7)
    return ECMD_ERR_PARSE_ERROR;
  uint16_t ret = i2c.DS1631Temp(I2C_SLA_DS1631 + adr, &temp, &stemp);
  if (ret == 0xffff)
    return ECMD_FINAL(snprintf_P(output, len, PSTR("no sensor detected")));
  return ECMD_FINAL(snprintf_P(output, len, PSTR("%d.%d"), temp, stemp));
}

#ifdef FS20_SUPPORT

int16_t parseFS20Send(char *cmd, char *output, uint16_t len) {
  uint16_t hc, addr, c, ew;

  int ret = sscanf_P(cmd, PSTR("%x %x %x %x"), &hc, &addr, &c, &ew);
  if (ret == 3) {
    rec868_stop(); //Empfang waehrend des Sendens abschalten
    fs20_send(hc, (addr & 0xFF), (c & 0xFF));
    if (rec868_global.stat.fs20 | rec868_global.stat.wett | rec868_global.stat.hell)
      rec868_start();
    return ECMD_FINAL_OK;
  } else if (ret == 4) {
    rec868_stop(); //Empfang waehrend des Sendens abschalten
    fs20_send_ew(hc, (addr & 0xFF), (c & 0xFF), (ew & 0xFF));
    if (rec868_global.stat.fs20 | rec868_global.stat.wett | rec868_global.stat.hell)
      rec868_start();
    return ECMD_FINAL_OK;
  }
  return ECMD_ERR_PARSE_ERROR;
}

#endif

int16_t parseReset(char *cmd, char *output, uint16_t len) {
  return ECMD_FINAL_OK;
  WDTCSR=(1<<WDE) | (1<<WDCE) ;
  WDTCSR= (1<<WDE);
  for(;;) {}
}

int16_t parseI2cDetect(char *cmd, char *output, uint16_t len) {
  i2c.detectI2C(output, len);
  return ECMD_FINAL(strlen(output));
}

int16_t parsePinSet(char *cmd, char *output, uint16_t len) {
  char *pnt;
  uint8_t pin;
  if ((pnt = strstr_P(cmd, PSTR("LED_RED"))) != NULL) {
    pin = PC6;
    pnt += 7;
  } else if ((pnt = strstr_P(cmd, PSTR("LED_GREEN"))) != NULL) {
    pin = PC7;
    pnt += 9;
  } else {
    return ECMD_ERR_PARSE_ERROR;
  }
  if (atoi(pnt) == 1) {
    PORTC |= (1 << pin); //Set
  } else {
    PORTC &= ~(1 << pin); //Clear
  }
  return ECMD_FINAL_OK;
}

int16_t parseHelp(char *cmd, char *output, uint16_t len) {
  uint8_t pos = 0;
  char * text = NULL;
  while (1) {
    text = (char *)pgm_read_word(&ecmd_cmds[pos].name);
    if (text == NULL) {
      break;
    } else {
      strcat_P(output, text);
      strcat(output, "\n");
    }
    pos++;
  }
  return ECMD_FINAL(strlen(output));
}


#include "parser.h"
#include "i2c.h"
#include "rec868.h"
#include "fs20.h"
#include <stdio.h>


const struct ecmd_command_t PROGMEM ecmd_cmds[] = {
  { "max7311 setDDRw", parseMAX7311SetDDRw },
  { "max7311 getDDRw", parseMAX7311GetDDRw },
  { "max7311 setOUTw", parseMAX7311SetOUTw },
  { "max7311 set", parseMAX7311Set },
  { "max7311 pulse", parseMAX7311Pulse },
  { "pcf8574x set", parsePCF8574xSet },
  { "pcf8574x get", parsePCF8574xGet },
  { "rec868 start", parseRec868Start },
  { "reset", parseReset },
  { "i2c detect", parseI2cDetect },
  { "ds1631 start", parseDS1631Start },
  { "ds1631 stop", parseDS1631Stop },
  { "ds1631 temp", parseDS1631Temp },
  { "fs20 send", parseFS20Send },
  { "pin set", parsePinSet },
  { NULL, NULL }
};

I2C i2c;

uint16_t parser ( char *cmd, char *output, int16_t len ) {
  int ret = -1;
  char *text = NULL;
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
  if (func != NULL)
    ret = func(cmd, output, len);

  if (output != NULL) {
    if (ret == -1) {
      memcpy_P(output, PSTR("parse error"), 11);
      ret = 11;
    } else if (ret == 0) {
      output[0] = 'O';
      output[1] = 'K';
      ret = 2;
    }
  }
}


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

}

int16_t parseMAX7311SetOUTw(char *cmd, char *output, uint16_t len) {

}

int16_t parseMAX7311Set(char *cmd, char *output, uint16_t len) {

}

int16_t parseMAX7311Pulse(char *cmd, char *output, uint16_t len) {

}

int16_t parsePCF8574xSet(char *cmd, char *output, uint16_t len) {

}

int16_t parsePCF8574xGet(char *cmd, char *output, uint16_t len) {

}

int16_t parseRec868Start(char *cmd, char *output, uint16_t len) {
  rec868_global.stat.wett = TRUE;
  rec868_global.stat.hell = TRUE;
  rec868_global.stat.fs20 = TRUE;
  rec868_start();
  return ECMD_FINAL_OK;
}

int16_t parseReset(char *cmd, char *output, uint16_t len) {

}

int16_t parseI2cDetect(char *cmd, char *output, uint16_t len) {

}

int16_t parseDS1631Start(char *cmd, char *output, uint16_t len) {

}

int16_t parseDS1631Stop(char *cmd, char *output, uint16_t len) {

}

int16_t parseDS1631Temp(char *cmd, char *output, uint16_t len) {

}

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

int16_t parsePinSet(char *cmd, char *output, uint16_t len) {

}

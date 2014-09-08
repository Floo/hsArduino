
#ifndef Parser_h
#define Parser_h

#include <avr/pgmspace.h>

/* function successful, no output; caller may print "OK" */
#define ECMD_FINAL_OK		0
/* function successful, output of size len in output buffer */
#define ECMD_FINAL(len)		(len)
/* error codes */
#define ECMD_ERR_PARSE_ERROR	-1	/* parse error */
#define ECMD_ERR_READ_ERROR	-2	/* reading data failed */
#define ECMD_ERR_WRITE_ERROR	-3	/* writing data failed */

#define I2C_SLA_PCF8574 0x20
#define I2C_SLA_PCF8574A 0x38

/* struct for storing commands */
struct ecmd_command_t {
  PGM_P name;
  int16_t (*func)(char*, char*, uint16_t);
};

typedef int16_t (*func_t)(char*, char*, uint16_t);

uint16_t parser ( char *cmd, char *output, int16_t len );
int16_t parseMAX7311SetDDRw(char *cmd, char *output, uint16_t len);
int16_t parseMAX7311GetDDRw(char *cmd, char *output, uint16_t len);
int16_t parseMAX7311SetOUTw(char *cmd, char *output, uint16_t len);
int16_t parseMAX7311Set(char *cmd, char *output, uint16_t len);
int16_t parseMAX7311Pulse(char *cmd, char *output, uint16_t len);
int16_t parsePCF8574xSet(char *cmd, char *output, uint16_t len);
int16_t parsePCF8574xGet(char *cmd, char *output, uint16_t len);
int16_t parseRec868Start(char *cmd, char *output, uint16_t len);
int16_t parseReset(char *cmd, char *output, uint16_t len);
int16_t parseI2cDetect(char *cmd, char *output, uint16_t len);
int16_t parseDS1631Start(char *cmd, char *output, uint16_t len);
int16_t parseDS1631Stop(char *cmd, char *output, uint16_t len);
int16_t parseDS1631Temp(char *cmd, char *output, uint16_t len);
int16_t parseFS20Send(char *cmd, char *output, uint16_t len);
int16_t parsePinSet(char *cmd, char *output, uint16_t len);

#endif

#ifndef GLOBALVARIABLE_H_
#define GLOBALVARIABLE_H_

#define debugging   0

#define BMSone      1
#define BMStwo      2
#define BMSfour     3

typedef struct
{
    unsigned char data[8];
} CAN_DATA;

//extern CAN_DATA globalCANdata[3];

#define OBMS3B          123731968
#define OBMS3CB         2036334592
#define OBMS6B2         3594518528

#endif

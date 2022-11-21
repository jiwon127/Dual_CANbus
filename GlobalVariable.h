#ifndef GLOBALVARIABLE_H_
#define GLOBALVARIABLE_H_

#define debugging   1

#define BMSone      1
#define BMStwo      2
#define BMSthree    3
#define BMSfour     4
#define BMSfive     5
#define BMSsix      6

typedef struct
{
    unsigned char data[8];
} CAN_DATA;

//extern CAN_DATA globalCANdata[3];

#define OBMS3B          123731968
#define OBMS3CB         2036334592
#define OBMS6B2         3594518528

#endif

#ifndef USB2CAN_DEFINE_H
#define USB2CAN_DEFINE_H

#endif // USB2CAN_DEFINE_H

#include "QString"
#include "QByteArray"
#include "list"

/*
0f0200
0f12020001
0f12021FC0
0f12021000
0f120214ff
0f120208DA
0f12020403

0f12020600
0f12020714

0f200118
0f200201
0f0300
0f12020000
*/


static const char Config[] =        {'\x0f',    '\x02', '\x00'                         };
static const char ResetMod[] =      {'\x0f',    '\x12', '\x02', '\x00', '\x01'         };
static const char ClockDivData[] =  {'\x0f',    '\x12', '\x02', '\x1F', '\xC0'         };   //Clock divider
static const char AccCode[] =       {'\x0f',    '\x12', '\x02', '\x10', '\x00'         };   //0x00  without filtration CAN adress
static const char AccMask[] =       {'\x0f',    '\x12', '\x02', '\x14', '\xff'         };   //0ff   without filtratio CAN adress
static const char OutCtrl[] =       {'\x0f',    '\x12', '\x02', '\x08', '\xDA'         };
static const char IE[] =            {'\x0f',    '\x12', '\x02', '\x04', '\x03'         };
static const char BT0[] =           {'\x0f',    '\x12', '\x02', '\x06', '\x00'         };   //BT0 Bus Timing 0x00 prescaller
static const char BT1[] =           {'\x0f',    '\x12', '\x02', '\x07', '\x14'         };   //BT1 Bus Timing set on the 1M This setting the finest value of baudrate
static const char CTL_Code[] =      {'\x0f',    '\x20', '\x01', '\x18'                 };
static const char TRL_Code[] =      {'\x0f',    '\x20', '\x02', '\x01'                 };
static const char NormalMode[] =    {'\x0f',    '\x03', '\x00'                         };   //Set Normal mod
static const char ModRegDat[] =     {'\x0f',    '\x12', '\x02', '\x00', '\x00'         };   //withou filtration message 0x00
                                                                                            //value is dependent on msg filtration
            //BusTiming0Reg = SJW_MB_24 | Presc_MB_24;
            //BusTiming1Reg = TSEG2_MB_24 | TSEG1_MB_24;
            //SJA1000 Clock frequency 24MHz 6-24MHz

static const char synchr =          {'\x0f' };
static const char CMD_writeReg =    {'\x12'};
static const char writeReg[] =      {synchr,    CMD_writeReg};
static const char readReg[] =       {'\x0f',    '\x10'};

static const char getMode[] =       {'\x0f',    '\x06',    '\x00'};

//QByteArray initReg  =               {0,31,16,17,18,19,20,21,22,23,8,4         };
//QByteArray initData[]  =           {1,C0,0,0,0,0,ff,ff,ff,ff,DA,03           };
//static const char initReg[]  =       {'\x00','\x31','\x16','\x17','\x18','\x19','\x20','\x21','\x22','\x23','\x08','\x04'};
//static const char initData[] =       {'\x01','\xC0','\x00','\x00','\x00','\x00','\xff','\xff','\xff','\xff','\xDA','\x03'};

//QByteArray initReg[]  =       {0x00,0x31,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x08,0x04};
//QByteArray initData[] =       {0x01,0xC0,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xDA,0x03};

//unsigned char initReg[]  =       {0x00,0x31,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x08,0x04};
//unsigned char initData[] =       {0x01,0xC0,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xDA,0x03};

//QByteArray initReg[12] = {0x00,0x31,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x08,0x04};
QString initReg  = {"003116171819202122230804"};
QString initData = {"01C000000000ffffffffDA03"};


/*
static const char Config[] =        {'\x0f',    '\x02', '\x00', '\x0D'                 };
static const char ResetMod[] =      {'\x0f',    '\x12', '\x02', '\x00', '\x01'         };
static const char ClockDivData[] =  {'\x0f',    '\x12', '\x02', '\x1F', '\xC0'         };
static const char AccCode[] =       {'\x0f',    '\x12', '\x02', '\x10', '\x00'         };
static const char AccMask[] =       {'\x0f',    '\x12', '\x02', '\x14', '\xff'         };
static const char OutCtrl[] =       {'\x0f',    '\x12', '\x02', '\x08', '\xDA'         };
static const char IE[] =            {'\x0f',    '\x12', '\x02', '\x04', '\x03'         };
static const char BT0[] =           {'\x0f',    '\x12', '\x02', '\x06', '\x00'         };
static const char BT1[] =           {'\x0f',    '\12',  '\x02', '\x07', '\x14'         };
static const char CTL_Code[] =      {'\x0f',    '\x20', '\x01', '\x18'                 };
static const char TRL_Code[] =      {'\x0f',    '\x20', '\x02', '\x01'                 };
static const char NormalMode[] =    {'\x0f',    '\x03', '\x00'                         };
static const char ModRegDat[] =     {'\x0f',    '\x12', '\x02', '\x00', '\x00'         };

static const char synchr[] =        {'\x0f'           };
static const char writeReg[] =      {'\x0f',    '\x12'};
static const char readReg[] =       {'\x0f',    '\x10'};

static const char getMode[] =       {'\x0f',    '\x06',    '\x00'};
*/
//Commands
static const QString msg = "msg";
static const QString test1 = "test1";

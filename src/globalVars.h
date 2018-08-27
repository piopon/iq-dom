#include <Arduino.h>
#include <IRremote.h>

const int serialSpeed=9600;

const byte pinHighVoltage=0;
const byte pinSSForSD=4;
const byte pinIR=6;
const byte pinSirene=7;
const byte pinVent=8;

const byte pirPin=A0;
const byte temperaturePin=A1;
const byte gasPin=A2;
const byte soundPin=A3;

const byte pilotBtnNone=0;
const byte pilotBtnOnOff=1;
const byte pilotBtnProgUp=2;
const byte pilotBtnProgDown=3;
const byte pilotBtnVolUp=4;
const byte pilotBtnVolDown=5;
const byte pilotBtnMute=6;

const int servo1Ch=0;
typedef enum {
  servo1,
  servo2,
  led1,
  led2,
  led3
} muxChannels;
muxChannels selectedChannel;

static int dhtTemp=0;
static int dhtHumidity=1;
int lastTemperature=24;
int lastHumidity=24;

byte gasCount;
float gasThreshold = 5.0;
int soundThreshold = 250;
int pirThreshold = 150;
byte countdownTime = 30;
bool armed, menuOpened;
unsigned long startTime, currTime;
String password = "1234";
char customKey;

boolean buttonsVisible=false, lastBtnState=true; 
boolean sireneOn=true;
int currPage=0;
byte nowHour, nowMinute;

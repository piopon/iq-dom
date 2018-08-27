#include <RTClibORIGINAL.h>

RTC_DS1307 rtc;
DateTime now; 

void setUpRTC(){
  Wire.begin();
  rtc.begin();
  if (!rtc.isrunning()) { 
    printLCD(1,2,"RTC is NOT running!", true); 
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void printTime(bool clearLine){
  now=rtc.now(); 
  nowHour=now.hour(); 
  nowMinute=now.minute();
  printLCD(0,1,"TIME:",clearLine);
  if(nowHour<10){
    printLCD(5,1,"0",clearLine); 
    printLCD(6,1,String(nowHour),clearLine); 
  } else {
    printLCD(5,1,String(nowHour),clearLine); 
  }
  printLCD(7,1,".",clearLine); 
  if(nowMinute<10){
    printLCD(8,1,"0",clearLine); 
    printLCD(9,1,String(nowMinute),clearLine); 
  } else {
    printLCD(8,1,String(nowMinute),clearLine); 
  }     
}

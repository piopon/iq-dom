#include "globalVars.h"

void setup() {
  gasCount = 0;
  armed = false;
  currPage = 0;

  setUpVoltage230();
  setUpVoltage12();
  setUpRecvIR();
  setUpMuxPWM();
  setUpKeypad();
  setUpRTC();
  setUpLCD();  
}

void loop() {
  while(readGasSensor()>=gasThreshold && gasCount<10){
    disableAlarm("GAS ALARM",true); 
    gasCount++;
  }
  showMenu(buttonsVisible);
  checkPilotButton(50);
  customKey = getKeypadKey();
  if (customKey != '\0'){
    if(armed){
      disableAlarm("INPUT PASS",true); 
    } else {
      if(customKey=='#'){
        enableAlarm();  
        lastBtnState=!lastBtnState;
        showMenu(buttonsVisible);  
      } else if(customKey=='*'){
        buttonsVisible=!buttonsVisible;
        showMenu(buttonsVisible);
      } else {
        if(buttonsVisible){
          switch(customKey) {
            case 'A':
              if(currPage==0) scanScreen();
              if(currPage==1) testScreen();
              //aby uaktualnic menu
              lastBtnState=!lastBtnState;
              break;
            case 'B':
              if(currPage==0) pilotScreen();
              if(currPage==1) sireneScreen(); 
               //aby uaktualnic menu           
              lastBtnState=!lastBtnState;
              break;
            case 'C':
              if(currPage==0) passwordScreen();
              if(currPage==1) thresholdScreen();
              //aby uaktualnic menu            
              lastBtnState=!lastBtnState;
              break;
            case 'D':
              currPage++;
              currPage%=2;
              //aby uaktualnic menu
              lastBtnState=!lastBtnState;
              break;
            default: //no action 
              break;
          }
          showMenu(buttonsVisible);
        }
      }
    }
  }  
  if(readSoundSensor()<=soundThreshold){
    if(armed) disableAlarm("SOUND ALARM",false); 
  }
  if(readPIRSensor()<=pirThreshold){
    if(armed){
      disableAlarm("MOVE ALARM",false); 
    } else {
      if(nowHour>18 && nowMinute>30) highVoltageON();
    }
  }  
}

void disableAlarm(String cause, bool countSetAlarm) {
  String inputPass, stars="";
  char kpadKey;
  int diffTime=0, timeLeft;
  bool cont = true, startPass = false;

  printLCD(0,1,"!!! "+cause+" !!!",true);
  printLCD(0,2,"                    ",true); 
  printLCD(0,3,"                    ",true); 
  startTime = millis();
  while(cont) {
    if (countSetAlarm) {
      diffTime = (millis()-startTime); 
      if(diffTime>countdownTime*1000) {
        countSetAlarm = false;
        printLCD(0,2," ALARM ON           ",false);
        printLCD(0,3,"! ! ! ! ! ! ! ! ! ! ",true);
      } else {
        timeLeft=countdownTime-(diffTime)/1000;
        if(timeLeft%2==0) {
          ledRGB(0,255,0);
        } else {
          ledRGB(0,0,0);
        }
        printLCD(0,2,"Entrance time:     s",false);
        if(timeLeft>=10) {
          printLCD(17,2,String(timeLeft),false);
        } else {
          printLCD(17,2," "+String(timeLeft),false);
        }
      }  
    } else {
        setAlarm();     
    }
    kpadKey = getKeypadKey();
    if (startPass==false){
      if(kpadKey == '#') startPass = true;
    } else {
      if(kpadKey != '\0'){
        if(kpadKey == '#') {
          if(inputPass==password) {
            resetAlarm();
            cont = false;
            lastBtnState=!lastBtnState;
          } else {
            inputPass = "";    stars="";
            if(countSetAlarm) {
              printLCD(10,3,"PASS NOK ",false);
            } else {
              printLCD(10,3,"PASS NOK ",false);
            }
            delay(100);
            startPass = false;
          } 
        } else {
          inputPass+=kpadKey;   stars+="*";
          if(countSetAlarm) {
            printLCD(13,3,stars+" ",false);  
          } else {
            printLCD(13,2,stars+" ",false); 
          }
        }
      }
    }     
  }
}

void enableAlarm(void) {
  char pressedKey='\0';
  String  currPass,stars="";
  int diffTime=0, timeLeft;
  
  printLCD(0,0,"== IQHOME: ALARM ===",false);
  printLCD(0,1,"PASSWORD:           ",false);  
  printLCD(0,2,"                    ",false);    
  printLCD(0,3,"EXIT='*'      OK='#'",false);   
  while (pressedKey != '*'){
    pressedKey = getKeypadKey();
    if(pressedKey != '\0'){
      if(pressedKey=='#'){
        stars="";
        if(currPass==password) {
          currPass="";
          startTime = millis();
          printLCD(0,2,"Exit time:         s",false);
          printLCD(0,3,"CANCEL='*'          ",false);
          while(timeLeft>0 && pressedKey!='*') {
            pressedKey=getKeypadKey();
            timeLeft=countdownTime-(millis()-startTime)/1000;
            if(timeLeft>=10) {
              printLCD(17,2,String(timeLeft),false);
            } else {
              printLCD(17,2," "+String(timeLeft),false);
            }
            if(timeLeft%2==0) {
              ledRGB(0,255,0);
            } else {
              ledRGB(0,0,0);
            }
          }
          if(pressedKey=='*'){
            delay(100);
            ledRGB(0,0,0);
            armed=false;
          } else {
            pressedKey='*';
            armed=true;
            ledRGB(0,255,0);
          }
        } else {
          currPass="";
          printLCD(0,2,"   BAD PASSWORD!    ",false);
          delay(1000);
          printLCD(0,2,"                    ",false); 
          armed=false;
        }
      } else {
        currPass+=pressedKey; 
        stars+="*";
        printLCD(10,1,stars+"  ",false);
      }
    } 
  }  
}    

void resetAlarm(void) {
  sireneOFF();
  ledRGB(0,0,255); 
  armed = false;
  printLCD(0,0,"== IQHOME: ALARM ===",false);
  printLCD(0,1,"                    ",false);  
  printLCD(0,2,"      DISARMED      ",false);  
  printLCD(0,3,"                    ",false); 
  delay(3000);
  ledRGB(0,0,0);
}

void setAlarm(void) {
  sireneON();
  ledRGB(255,0,0);
  armed = true;   
}

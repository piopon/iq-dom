#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);

uint8_t downArrows[8] = {B00000,B00000,B10001,B11011,B01110,B00100,B00000};
uint8_t upArrows[8] = {B00000,B00100,B01110,B11011,B10001,B00000,B00000};

void setUpLCD(){
  lcd.init();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.createChar(0, downArrows);
  lcd.createChar(1, upArrows);  
  greetingsLCD(5000);
}

void greetingsLCD(int waitTime) {
  lcd.setCursor(1,0);
  lcd.print("===== IQHOME =====");  
  lcd.setCursor(1,1);
  lcd.print("Arduino testing");    
  lcd.setCursor(1,2);
  lcd.print("");    
  lcd.setCursor(1,3);
  lcd.print("==================");  
  delay(waitTime);
  lcd.clear();  
}

void showMenu(bool show) {
  lcd.setCursor(0,0);
  lcd.print("== IQHOME: MAIN ====");
  buttons(show,currPage);
}

void buttons(bool show, byte page){
  if(show) {
    if(lastBtnState!=show) {
      printPanel(true,1);
    }
    lcd.setCursor(0,2);
    if(page==0) lcd.print("scan|pilot|pass | ->");
    if(page==1) lcd.print("test|siren|thres| ->");
    lcd.setCursor(0,3);
    lcd.print(" A  |  B  |  C  | D "); 
  } else {
    lcd.setCursor(0,1);
    printLCD(12,1,"TEMP."+printTempSensor(dhtTemp,false)+"C",lastBtnState!=show);
    printTime(lastBtnState!=show);
    lcd.setCursor(0,2);
    lcd.print("                    "); 
    if(lastBtnState!=show) {
      printPanel(false,3);
    }
  }
  lastBtnState=show;
}

void printPanel(bool showBtns, int lineNo){
  lcd.setCursor(0,lineNo);
  lcd.print("                    "); 
  if(showBtns){
    lcd.setCursor(0,lineNo);
    lcd.print(char(0));
    lcd.setCursor(1,lineNo);
    lcd.print(char(0));
    lcd.setCursor(2,lineNo);
    lcd.print(char(0));
    lcd.setCursor(3,lineNo);
    lcd.print(" hide menu (*) ");    
    lcd.setCursor(18,lineNo);    
    lcd.print(char(0)); 
    lcd.setCursor(19,lineNo);           
    lcd.print(char(0));    
  } else {
    lcd.setCursor(0,lineNo);
    lcd.print(char(1));
    lcd.setCursor(1,lineNo);
    lcd.print(char(1));
    lcd.setCursor(2,lineNo);
    lcd.print(char(1));
    lcd.setCursor(3,lineNo);
    lcd.print(" show menu (*) ");    
    lcd.setCursor(18,lineNo);    
    lcd.print(char(1)); 
    lcd.setCursor(19,lineNo);           
    lcd.print(char(1));
  }
}

void scanScreen(void) {
  char pressedKey='\0';
  byte address,error, nDevices, foundAddr[10];
  
  printLCD(0,0,"== IQHOME: SCAN ====",false);
  printLCD(0,1," Press '#' to start ",false);
  printLCD(0,2,"                    ",false);
  printLCD(0,3,"                    ",false);  
  while (pressedKey != '*'){
    pressedKey = getKeypadKey();
    if(pressedKey == '#'){
      nDevices = 0; 
      printLCD(0,1," ... Scanning ...",true); 
      for(address = 1; address < 127; address++ ) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission(); 
        if(error==0) {
          foundAddr[nDevices]=address;
          nDevices++;          
        }
      }
      printLCD(0,1,"  Found "+String(nDevices)+" devices!",true);
      if(nDevices>0){
        for(byte i=0;i<nDevices;i++){
          lcd.setCursor(i*5,2);
          lcd.print("0x");
          lcd.setCursor(i*5+2,2);
          lcd.print(foundAddr[i],HEX);
        }
      }
      delay(5000);
      printLCD(0,1," Press '#' to start ",false);
    }   
  }  
}

void testScreen(void) {
  char pressedKey='\0';
  byte testValue, PCF8591=(0x90 >> 1);
  bool testOn=false;
  
  printLCD(0,0,"== IQHOME: TEST ====",false);
  printLCD(0,1," 1) Connect tester  ",false);
  printLCD(0,2," 2) Press # to start",false);
  printLCD(0,3,"                    ",false);   
  while (pressedKey  != '*'){
    pressedKey = getKeypadKey();
    if(pressedKey == '#'){
      //przelaczamy wlaczanie/wylaczanie testu
      if(testOn) {
        testOn=false;
        printLCD(0,2," 2) Press # to start",false);
        printLCD(0,3,"                    ",false);        
      } else {
        testOn=true; 
        printLCD(0,2," 2) Press # to stop ",false);
        printLCD(0,3,"                    ",false);         
      } 
    }
    if(testOn) {
      Wire.beginTransmission(PCF8591);
      Wire.write(0x03);
      Wire.endTransmission();
      Wire.requestFrom((0x90 >> 1), 2);
      testValue=Wire.read();
      testValue=Wire.read();
      if(testValue<100) {
        printLCD(5,3,"ADC = 0"+String(testValue),false);
      } else {
        printLCD(5,3,"ADC = "+String(testValue),false); 
      }
    }    
  }
}

void pilotScreen(void) {
  char pressedKey='\0';
  //czyscimy ekran
  printLCD(0,0,"== IQHOME: PILOT ===",false);
  printLCD(0,1,"  Press any button  ",false);
  printLCD(0,2,"                    ",false);
  printLCD(0,3,"                    ",false);  
  while (pressedKey != '*'){
    pressedKey = getKeypadKey();
    unsigned long tempBtn=getCurrPilotBtn(50);
    if(tempBtn!=0) printLCD(0,2,String(tempBtn),true);
  }  
}

void sireneScreen(void) {
  char pressedKey='\0';
  
  printLCD(0,0,"== IQHOME: SIRENE ==",false);
  if(sireneOn) {
    printLCD(0,1,"     Sirene ON   ",true);
  } else {
    printLCD(0,1,"     Sirene OFF  ",true);  
  }
  printLCD(0,2,"                    ",false);
  printLCD(0,3,"turnON=A   turnOFF=B",false); 
  while (pressedKey!= '*'){
    pressedKey = getKeypadKey(); 
    if(pressedKey == 'A'){
      sireneOn=true;
      printLCD(0,1,"     Sirene ON   ",false);
    } else if(pressedKey == 'B'){
      sireneOn=false; 
      printLCD(0,1,"     Sirene OFF  ",false);  
    }
  }  
}

void passwordScreen(void) {
  bool newPassInput=false;
  char pressedKey='\0';
  String currPass, stars="";
  
  printLCD(0,0,"== IQHOME: PASS ====",false);
  printLCD(0,1,"  Current password  ",false);
  printLCD(0,2,"   Apply with '#'   ",false);
  printLCD(0,3,"                    ",false);   
  while (pressedKey != '*'){
    pressedKey = getKeypadKey();
    if(pressedKey != '\0'){
      if(pressedKey=='#'){
        stars="";
        if(newPassInput) {
          password=currPass;
          newPassInput=false;
          printLCD(0,3,"  NEW PASSWORD OK!  ",false);
          delay(1000);
          printLCD(0,1,"  CURRENT PASSWORD  ",false);
          printLCD(0,3,"                    ",false);          
        } else {
          if(currPass==password) {
            currPass="";
            printLCD(0,1,"    New password    ",false);
            printLCD(0,3,"                    ",false);
            newPassInput=true;
          } else {
            currPass="";
            printLCD(0,3,"  WRONG PASSWORD!   ",false);
            delay(1000);
            printLCD(0,3,"                    ",false); 
          }
        }
      } else {
        currPass+=pressedKey; 
        stars+="*";
        printLCD(8,3,stars+"    ",false);
      } 
    }
  }  
}

void thresholdScreen(void) {
  char pressedKey='\0';
  String newValStr;
  int newVal;
  byte currVar=0;
  
  printLCD(0,0,"== IQHOME: THRESHOLD",false);
  printLCD(0,1," A = sound : "+String(soundThreshold),true);
  printLCD(0,2," B = PIR   : "+String(pirThreshold),true);
  printLCD(0,3," C = time  : "+String(countdownTime),true);   
  while (pressedKey != '*'){
    pressedKey = getKeypadKey(); 
    if(pressedKey == 'A'){
      currVar=1;      
    } else if(pressedKey == 'B'){
      currVar=2;
    } else if(pressedKey == 'C'){
      currVar=3;      
    } else {
       switch(currVar) {
         case 0:
           break;
         case 1:
           printLCD(0,1," A = sound :        ",false);
           pressedKey='\0';
           while (pressedKey != '#'){
             pressedKey = getKeypadKey(); 
             if(pressedKey!='\0'){
               newValStr+=pressedKey;
               printLCD(13,1,newValStr,false);
             } 
           }
           newVal=newValStr.toInt();
           if(newVal>255) newVal=255;             
           soundThreshold=newVal;
           printLCD(0,1," A = sound : "+String(soundThreshold)+" OK",true);
           currVar=0; pressedKey='\0'; newValStr="";
           delay(1000);
           printLCD(0,1," A = sound : "+String(soundThreshold)+"   ",false);
           break;
         case 2:
           printLCD(0,2," B = PIR   :        ",false);
           pressedKey='\0';
           while (pressedKey != '#'){
             pressedKey = getKeypadKey(); 
             if(pressedKey!='\0'){
               newValStr+=pressedKey;
               printLCD(13,2,newValStr,false);
             } 
           }
           newVal=newValStr.toInt();
           if(newVal>255) newVal=255;             
           pirThreshold=newVal;
           printLCD(0,2," B = PIR   : "+String(pirThreshold)+" OK",true);
           currVar=0; pressedKey='\0'; newValStr="";
           delay(1000);
           printLCD(0,2," B = PIR   : "+String(pirThreshold)+"   ",false);
           break;
         case 3:
           printLCD(0,3," C = time  :        ",false);
           pressedKey='\0';
           while (pressedKey != '#'){
             pressedKey = getKeypadKey(); 
             if(pressedKey!='\0'){
               newValStr+=pressedKey;
               printLCD(13,3,newValStr,false);
             } 
           }
           newVal=newValStr.toInt();
           if(newVal>255) newVal=255;             
           countdownTime=newVal;
           printLCD(0,3," C = time  : "+String(countdownTime)+" OK",true);
           currVar=0; pressedKey='\0'; newValStr="";
           delay(1000);
           printLCD(0,3," C = time  : "+String(countdownTime)+"   ",false);
           break;
         default:
           break;  
       } 
    }
  }
}

void printLCD(byte colNo, byte rowNo, String toPrint, boolean clearLine) {
   if(clearLine){
     lcd.setCursor(0,rowNo);
     lcd.print("                    ");   
   }
   lcd.setCursor(colNo,rowNo); 
   lcd.print(toPrint);
}

/*
 * ODBIORNIK IR
 */
#include <IRremote.h>

IRrecv recvCodeIR(pinIR);
decode_results irCodes;

class irCmds {
  public:
     unsigned long in;
     byte out;
};
const byte maxCmdNo=20;
irCmds commands[maxCmdNo]={ {0x260C914D,1},{0xC9418F01,1},{0xF6417F77,2},{0x530C81C3,2},
                            {0x5D2ABE91,3},{0x5FBC45,3}  ,{0xABEA935F,4},{0x4F1F9113,4},
                            {0x393B6B28,5},{0xDC7068DC,5},{0x8DF5B365,6},{0x312AB119,6} };

void setUpRecvIR(){
  //wlaczamy odbiornik IR
  recvCodeIR.enableIRIn();
}

unsigned long getCurrPilotBtn(byte delayMs) {
  unsigned long result=0;
  
  if (recvCodeIR.decode(&irCodes)) {
    result = irCodes.value;
    delay(delayMs);
    recvCodeIR.resume();
  }  
  return result;
}

void checkPilotButton(byte delayMs) {
  byte cmdID=0;
  if (recvCodeIR.decode(&irCodes)) {
    if (checkCommand(irCodes,&cmdID)) {
      executeCmd(cmdID);
    }
    delay(delayMs);
    recvCodeIR.resume();
  }
}

bool checkCommand(decode_results res, byte *id){
  bool result=false;
  for(byte i=0;i<maxCmdNo;i++){
    if(commands[i].in==res.value){
      result=true;
      *id=i;
      break;
    }
  }
}

void executeCmd(byte selectedID) {
  switch(commands[selectedID].out){
    case 0: 
     break;
    case 1: 
     highVoltageON();
     break;
    case 2: 
     highVoltageOFF(); 
     break;
    case 3:
     sireneHorn(250);
     break;
    case 4:
     ledRGB(255,0,0);
     break;
    case 5:
     ledRGB(0,0,255);
     break;  
    case 6:
     ledRGB(0,255,0);
     break; 
    default: 
     break;     
  }
}

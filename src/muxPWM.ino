#include <Wire.h>
#include <Tlc5940.h>

const int led1R=8;
const int led1G=9;
const int led1B=10;
const int led2R=7;
const int led2G=6;
const int led2B=5;
const int led3R=4;
const int led3G=3;
const int led3B=2;

void setUpMuxPWM(){
  Tlc.init();
  Tlc.clear();
}

void ledRGB(int R, int G, int B) {
    if ((R>=0 && R<=255)&&(G>=0 && G<=255)&&(B>=0 && B<=255)) {
      Tlc.set(led1R, scaleToPWM(R));
      Tlc.set(led1G, scaleToPWM(G));
      Tlc.set(led1B, scaleToPWM(B));
      Tlc.set(led2R, scaleToPWM(R));
      Tlc.set(led2G, scaleToPWM(G));
      Tlc.set(led2B, scaleToPWM(B));  
      Tlc.set(led3R, scaleToPWM(R));
      Tlc.set(led3G, scaleToPWM(G));
      Tlc.set(led3B, scaleToPWM(B));      
      Tlc.update();
    }
}

int scaleToPWM(int colorVal) {
  int result;
  
  result=colorVal*(4095/256); 
  
  return result;
}


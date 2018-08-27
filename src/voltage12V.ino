/*
 * ODBIORNIKI 12V
 */
void setUpVoltage12(void) {
  pinMode(pinSirene, OUTPUT);
  pinMode(pinVent, OUTPUT);  
}

void sireneHorn(int hornTimeMS) {
  if(sireneOn){
    digitalWrite(pinSirene, HIGH);
    delay(hornTimeMS);
    digitalWrite(pinSirene, LOW);
  }
}

void sireneON(void) {
  if(sireneOn){
    digitalWrite(pinSirene, HIGH);
  }
}

void sireneOFF(void) {
  digitalWrite(pinSirene, LOW);
}

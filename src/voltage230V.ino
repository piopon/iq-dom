/*
 * ZASILANIE 230V
 */
void setUpVoltage230(void) {
  pinMode(pinHighVoltage, OUTPUT);
}

void highVoltageON(){
  digitalWrite(pinHighVoltage, HIGH);  
}

void highVoltageOFF(){
  digitalWrite(pinHighVoltage, LOW);
}


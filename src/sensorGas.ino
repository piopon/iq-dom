float readGasSensor() {
  int gasSensorVal;
  float result;
  
  gasSensorVal = analogRead(gasPin);
  result=(float)gasSensorVal/1024*5.0;

  return result;
}

String printGasSensor(){
  return String(readGasSensor());
} 


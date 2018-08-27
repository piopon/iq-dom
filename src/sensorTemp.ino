#include <Dht11.h>

static Dht11 sensor(temperaturePin);

String printTempSensor(int whichData,boolean showError) {
  String result="";
       
  switch (sensor.read()) {
    case Dht11::OK:
        if(whichData==dhtTemp){
          lastTemperature=sensor.getTemperature();
          result=String(lastTemperature);
        }
        if(whichData==dhtHumidity){
          lastHumidity=sensor.getHumidity();
          result=String(lastHumidity);
        }
        break;
     default:        
        result=String(lastTemperature);
        break;
  }
  
  return result;
}

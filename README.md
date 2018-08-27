# iq-dom
Uploaded of an simple (and old) Arduino smart home project with usage of:
1. one servo (blind control simulation)
2. small vent 
3. controlling of a 230V desk lamp (lightning simulation)
4. 20 x 4 LCD and 4x4 keypad to interact with **control unit**:
	* reed-switch (main door) which triggers countdown to insert password
	* _PIR_ sensor for detecting movement   
	* temperature and humidity sensor [_DHT11_]
	* gas leakage, sound sensor
	* _RTC_ for time display and countdown management, 
	* dynamic setting of sensor thresholds (sensitivity, value that triggers alarm)
	* password and users management
	* distributed system = usage of _TWI_ interface (_multiple devices connected to one bus_)
5. 12V sirene and 3 RGB leds for acustic and optic **breach indication**
6. **remote control** with IR + ability to register new control codes

---

#include <LifePlanters.h>
#include <Wire.h>

SerialReadPrint serialRP;
RunningAverage runavg_moisture(5);
RunningAverage runavg_light(5);
RunningAverage runavg_temp(5);

LifePlanters::LifePlanters(uint8_t addr) : soilSensor(addr) { // calling another object constructor with my constructor
 
}

LifePlanters::LifePlanters(uint8_t addr, uint8_t pump) : soilSensor(addr) { // calling another object constructor with my constructor
	_pump_pin = pump;
}

LifePlanters::LifePlanters(uint8_t addr, uint8_t pump, uint8_t light) : soilSensor(addr) { // calling another object constructor with my constructor
	_pump_pin = pump;
	_light_pin = light;
}
/*****************************************
Initialization methods
******************************************/
void LifePlanters::checkStatus() {
	soilSensor.begin();
	delay(1000);
	Serial.print("Trying to inititialize Soil Moisture Sensor at Address: ");
	Serial.println(soilSensor.getAddress(),HEX);
	Serial.print("Sensor Firmware version: ");
  	Serial.println(soilSensor.getVersion(),HEX);
  	Serial.println();
}

void LifePlanters::findAllSensors(){
	byte error, address;
	int nDevices;

	Serial.println("Scanning...");

	nDevices = 0;
	for(address = 1; address < 127; address++ ) {
	// The i2c_scanner uses the return value of
	// the Write.endTransmisstion to see if
	// a device did acknowledge to the address.
	Wire.beginTransmission(address);
	error = Wire.endTransmission();

	if (error == 0) {
	  Serial.print("I2C device found at address 0x");
	  if (address<16)
	    Serial.print("0");
	  Serial.print(address,HEX);
	  Serial.println("  !");

	  nDevices++;
	}
	else if (error==4) {
	  Serial.print("Unknown error at address 0x");
	  if (address<16)
	    Serial.print("0");
	  Serial.println(address,HEX);
	}    
	}
	if (nDevices == 0)
	Serial.println("No I2C devices found\n");
	else
	Serial.println("done\n");

	delay(5000);           // wait 5 seconds for next scan
}

bool LifePlanters::changeSensorAddr(uint8_t newAddr){
	Serial.print("Trying to change address to: ");
	Serial.print(newAddr, HEX);
	Serial.print(".......");
	if (soilSensor.setAddress(newAddr,true)) // set Sensor Address to 0x21 and reset
	    Serial.println("... DONE");
	else 
	    Serial.println("... ERROR");
		Serial.println();
}

/************************************
Setter methods. Compulsory setter methods if the corresponding control scheme is activated. Compiler will throw an exception error if undeclared
************************************/
void LifePlanters::set_pump_pin(uint8_t pin){
	_pump_pin = pin;
	pinMode(pin, OUTPUT);
}

void LifePlanters::set_light_pwm_pin(uint8_t pin){
	_light_pin = pin;
	pinMode(pin, OUTPUT);
}

/******************************************
Optional setter methods:
******************************************/
void LifePlanters::set_desired_moisture(int desired){	_desired_moisture = desired;}
void LifePlanters::set_desired_light(int desired){	_desired_light = desired;}

void LifePlanters::set_moisture_maplimits(int min, int max){
	out_min_moist = min;
	out_max_moist = max;
}

void LifePlanters::set_light_maplimits(int min, int max){
	out_min_light = min;
	out_max_light = max;
}


/******************************************
Controller Methods
******************************************/
void LifePlanters::control_pump(){
	int curr_moisture = readMoisture();
	_activatePump(curr_moisture);
}

void LifePlanters::control_light(){
	int curr_light = readLight();
	_activateLight(curr_light);
}

/******************************************
Actuation Methods 
******************************************/
void LifePlanters::_activatePump(int curr_moisture){ // should be private
	int min_thresh = 0.6 * _desired_moisture;
	if(curr_moisture < min_thresh){ digitalWrite(_pump_pin, HIGH);}
	if(curr_moisture > _desired_moisture){ digitalWrite(_pump_pin, LOW);}
}

// open loop control for now. With actual grow lights, i can implement closed loop conrol
int LifePlanters::_activateLight(int curr_light_intensity){
	// inverse mapping of curent light intensity to output pwm
	int light_pwm = _mapping(0, 255, curr_light_intensity, 0, 100, 2); //(int out_min, int out_max, long in_reading, long in_min, long in_max, uint8_t mode)
	analogWrite(_light_pin, light_pwm);
	return light_pwm;
}

/******************************************
Reading Methods 
******************************************/
void LifePlanters::getAllReadings(){
	int soilMoisture = readMoisture();
	int light = readLight();
	int temp = readTemp();
	serialRP.s_println("Scaled Soil Moisture : ", soilMoisture);
	serialRP.s_println("Scaled Light Level : ", light);
	serialRP.s_println("Scaled Temperature : ", temp);
}

void LifePlanters::getScaledMoisture(){
	int soilMoisture = readMoisture();
	serialRP.s_println("Scaled Soil Moisture : ", soilMoisture);
}

void LifePlanters::getScaledLight(){
	int light = readLight();
	serialRP.s_println("Scaled Light Level : ", light);
}



int LifePlanters::readMoisture(){
	uint16_t soilMoisture = soilSensor.getCapacitance();
	// uint16_t soilMoisture = 400;
	runavg_moisture.addValue(soilMoisture);
	uint16_t filtered = runavg_moisture.getAverage();
	int mapped = _mapping(out_min_moist, out_max_moist, filtered, 236, 655, 0);
	return mapped;
}

int LifePlanters::readLight(){
	uint16_t light = soilSensor.getLight(true);
	// uint16_t light = 40000;
	runavg_light.addValue(light);
	uint16_t filtered = runavg_light.getAverage();
	int mapped = _mapping(out_min_light, out_max_light, filtered, 236, 60000, 0);
	return mapped;
}

int LifePlanters::readTemp(){
	int soilTemp = soilSensor.getTemperature();
	int temp = 401;
	runavg_temp.addValue(temp);
	float filtered = runavg_temp.getAverage();
	// float filtered = (float)filtered/10.0;
	return filtered;
}

/***************
 Data Processing methods
 *****************/
double LifePlanters::_mapping(long out_min, long out_max, long in_reading, long in_min, long in_max, uint8_t mode){
	double out_reading;
  // Saturation limits
 	if(in_reading>in_max){in_reading = in_max;}
	if(in_reading<in_min){in_reading = in_min;}
	if(in_reading == null){Serial.println("null input into the mapping function"); return null;};
	switch(mode)
	{
	case 1:{// linear mapping   
	  out_reading = null;         
	  break;
	}
	case 2:{ //inverse linear mapping
	  out_reading = (double)out_max - (double)(out_max - out_min)*((double)(in_reading - in_min)/(in_max - in_min));
	  break;
	}
	default:{    
	  out_reading = out_min + (double)(out_max - out_min)*(double)((double)(in_reading - in_min)/(in_max - in_min));
	}
	}
	return out_reading;
}
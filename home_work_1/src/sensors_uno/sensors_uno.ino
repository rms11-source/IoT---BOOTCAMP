#include <Keypad.h>

//NAME OF SENSORS
//1 - Temperature sensor
//2 - PIR sensor
//3 - Smoke sensor 
//4 - Ultrasonic sensor
//5 - Light intensity sensor

enum {sensorTemp, sensorLight, sensorPIR, sensorDistance, sensorGas, nrOfSensors};
char listCommands[5] = {'1', '2', '3', '4', '5'};

typedef struct {
  float (*get) (void);
  float threshold;
  char paramName[20];
  char unitName[10];
} sensorType;


//CONFIGURE KEYPAD
const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
{'1', '2', '3', 'A'},
{'4', '5', '6', 'B'},
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {11, 10, 9, 8};
byte colPins[COLS] = {7, 6, 5, 4};

bool checkCommand(char command){
  bool value = false;
  for(int i=0;i < nrOfSensors;i++){
    if(command == listCommands[i]){
      value = true;
    }
  }
  return value;
}

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//CONFIGURE LED PINS

#define RED_PIN 3
#define GREEN_PIN 2

void setupLeds() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
}

void activateRedLed(){
  digitalWrite(RED_PIN, HIGH);
  delay(3000);
  digitalWrite(RED_PIN, LOW);
}

void activateGreenLed(){
  digitalWrite(GREEN_PIN, HIGH);
  delay(3000);
  digitalWrite(GREEN_PIN, LOW);
}

//TEMPERATURE SENSOR

#define TEMP_PIN A5
// ADC range
#define ADC_MIN 0
#define ADC_MAX 1023
// Temperature range
#define TEMP_MIN -40
#define TEMP_MAX 125

void setupTemperature() {
  pinMode(TEMP_PIN, INPUT);
}

float getTemperature(){
  int rawTemp = analogRead(TEMP_PIN);
  float temp = map(((rawTemp - 20) * 3.04), ADC_MIN, ADC_MAX, TEMP_MIN, TEMP_MAX);
  return temp;
} 

//LIGHT SENSOR

#define LIGHT_PIN A4
#define LIGHT_MIN 10
#define LIGHT_MAX 100
#define ADC_MIN 0
#define ADC_MAX 1023
#define VOLT_MIN 0
#define VOLT_MAX 5
  

void setupLight() {
  pinMode(LIGHT_PIN, INPUT);
}

float getLight() {
  int rawLight = analogRead(LIGHT_PIN);
  float voltage = map(rawLight, ADC_MIN, ADC_MAX, VOLT_MIN, VOLT_MAX);
  float light = map(voltage, VOLT_MIN, VOLT_MAX, LIGHT_MIN, LIGHT_MAX);
  return light;
}


//PIR SENSOR

#define PIR_PIN 12

void setupPIR() {
  pinMode(PIR_PIN, INPUT);
}

float getPIR(){
  int statePIR = digitalRead(PIR_PIN);
  return statePIR;
}

//ULTRASONIC SENSOR
#define TRIG_PIN 0
#define ECHO_PIN 1

// defines variables
long duration;
int distance;

void setupUltrasonic() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT); 
}

float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  // Calculating the distance
  distance= duration*0.034/2;
  return distance;
}

//GAS SENSOR

#define GAS_PIN 13

void setupGas(){
  pinMode(GAS_PIN, INPUT);
}

float getGas(){
  int stateGas;
  float rawGas = analogRead(GAS_PIN);
  if (rawGas >= 150) {
    stateGas = 1;
  }
  else {
    stateGas = 0;
  }
  return stateGas;
}


sensorType mySensors[nrOfSensors] = {
  {
    getTemperature,
    40.0,
    "Temperature",
    "Celsius"
  },
  {
    getLight,
    50.0,
    "Light intensity",
    "points"
  },
  {
    getPIR,
    1.0,
    "Motion",
    "yes/no"
  },
  {
    getDistance,
    150.0,
    "Distance",
    "cm"
  },
  {
    getGas,
    1.0,
    "Smoke",
    "yes/no"
  }
};

float getSensorValue(int id) {
  if(id < nrOfSensors) {
    return mySensors[id].get();
  }
}

float getThreshold(int id) {
  if(id < nrOfSensors) {
    return mySensors[id].threshold;
  }
}

char * getSensorUnit(int id) {
    if(id < nrOfSensors) {
      return mySensors[id].unitName;
    }
}

char * getParamName(int id) {
    if(id < nrOfSensors) {
      return mySensors[id].paramName;
    }
}


void setup()
{
  Serial.begin(9600);
  setupLeds();
  setupTemperature();
  setupLight();
  setupPIR();
  setupUltrasonic();
  setupGas();
}

void loop()
{
  char command = customKeypad.waitForKey();
  if (checkCommand(command)){
    int index = command - '0';
    index--;
    char * paramName = getParamName(index);
    Serial.print(paramName);
    Serial.print(": ");
    
    float value = getSensorValue(index);  
    Serial.print(value);

    char * unit = getSensorUnit(index);
    Serial.print(" ");
    Serial.print(unit);
    
    Serial.println();
    
    float threshold = getThreshold(index);
    if(value >= threshold) {
      activateRedLed();
    }
    else {
      activateGreenLed();
    }
  }
  else {
    Serial.println("Wrong button. Try again");
    Serial.println();
  }
}

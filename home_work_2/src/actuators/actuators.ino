#define PIR_PIN 3

void setupPIR(){
  pinMode(PIR_PIN, INPUT);
}

int getPIR(){
  int statePIR = digitalRead(PIR_PIN);
  return statePIR;
}

//LIGHT SENSOR

#define LIGHT_PIN A0
#define LIGHT_MIN 10
#define LIGHT_MAX 100
#define ADC_MIN 0
#define ADC_MAX 1023
#define VOLT_MIN 0
#define VOLT_MAX 5

void setupLight(){
  pinMode(LIGHT_PIN, INPUT);
}

int getLight(){
  int rawLight = analogRead(LIGHT_PIN);
  int stateLight;
  float voltage = map(rawLight, ADC_MIN, ADC_MAX, VOLT_MIN, VOLT_MAX);
  float light = map(voltage, VOLT_MIN, VOLT_MAX, LIGHT_MIN, LIGHT_MAX);
  if (light >= 30) {
    stateLight = 1;
  }
  else {
    stateLight = 0;
  }
  return stateLight;
}


//RELAY

#define RELAY_PIN 2

void setupRelay(){
  pinMode(RELAY_PIN, OUTPUT);
}

int getRelayState() {
  int state = digitalRead(RELAY_PIN);
  return state;
}

void turnRelayOn(){
  digitalWrite(RELAY_PIN, HIGH);
}

void turnRelayOff(){
  digitalWrite(RELAY_PIN, LOW);
}

void setup() {
  Serial.begin(9600);
  setupPIR();
  setupLight();
  setupRelay();
}

void loop() {
  int statePIR = getPIR();
  int stateLight = getLight();
  int stateRelay = getRelayState();
  if ((statePIR == 1) && (stateLight == 0)) {
    if (stateRelay == 0) {
      turnRelayOn(); 
    }
  }
  if (stateRelay == 1) {
    turnRelayOff(); 
  }
}

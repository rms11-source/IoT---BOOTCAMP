#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
//#include <MQUnifiedsensor.h>

#define DHT1_PIN 16
#define DHT2_PIN 17     
//#define MQ2_PIN (A2)

#define DHTTYPE DHT11
//#define Type  ("MQ-2")

const char* ssid = "HOME_Wi-Fi";
const char* password = "home1112";

const char* mqtt_server = "192.168.1.101";
const char* topic = "home/datatopic";
   
// Initializes the espClient
WiFiClient espClient;
PubSubClient client(espClient);

DHT sensor1(DHT1_PIN, DHTTYPE);
DHT sensor2(DHT2_PIN, DHTTYPE);
//MQ2_PIN sensor3("MQ-2");

// Timers auxiliar variables
long now = millis();
long lastMeasure = 0;

char data[80];

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP_IN")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  sensor1.begin();
  sensor2.begin();
  //sensor3.begin();
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())

    client.connect("ESP_IN");
    
  now = millis();
  // Publishes new temperature and humidity every 30 seconds
  if (now - lastMeasure > 10000) {
    lastMeasure = now;
    float h1 = sensor1.readHumidity();
    // Read temperature as Celsius
    float t1 = sensor1.readTemperature();

    float h2 = sensor2.readHumidity();
    // Read temperature as Celsius
    float t2 = sensor2.readTemperature();


    // Check if any reads failed and exit early (to try again).
    if (isnan(h1) || isnan(t1)) {
      Serial.println("Failed to read from DHT sensor1");
      return;
    }

    static char temperatureTemp[7];
    dtostrf(t1, 6, 2, temperatureTemp);
    
    static char humidityTemp[7];
    dtostrf(h1, 6, 2, humidityTemp);


    // Check if any reads failed and exit early (to try again).
    if (isnan(h2) || isnan(t2)) {
      Serial.println("Failed to read from DHT sensor2");
      return;
    }

    static char temperatureTemp1[7];
    dtostrf(h2, 6, 2, temperatureTemp1);
    
    static char humidityTemp1[7];
    dtostrf(t2, 6, 2, humidityTemp1);

    //float g = analogRead(A0);
   // if (isnan(g))
    //{
    //Serial.println("Failed to read from MQ-5 sensor!");
   // return;
   // }

    char buffer[256];
     
    //Publishes Temperature and Humidity values    
    sprintf(buffer, "{\"v\": %.2f, \"t\": \"%c\", \"id\": \"%s\"}", h1 , 'h', "sensor1");

    client.publish("home/datatopic", buffer);
        
    Serial.println(buffer);
    
    sprintf(buffer, "{\"v\": %.2f, \"t\": \"%c\", \"id\": \"%s\"}", t1 , 't', "sensor1");


    client.publish("home/datatopic", buffer);
        
    Serial.println(buffer);
    
    sprintf(buffer, "{\"v\": %.2f, \"t\": \"%c\", \"id\": \"%s\"}", t2 , 't', "sensor2");


    client.publish("home/datatopic", buffer);
        
    Serial.println(buffer);
    
    sprintf(buffer, "{\"v\": %.2f, \"t\": \"%c\", \"id\": \"%s\"}", h2 , 'h', "sensor2");


    client.publish("home/datatopic", buffer);
        
    Serial.println(buffer);
    
    //sprintf(buffer, "{\"v\": %.2f, \"t\": \"%c\", \"id\": \"%s\"}", g , 'g', "sensor3");
    
    //client.publish("home/datatopic", buffer);
        
    //Serial.println(buffer);
    
  }
}

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <OneWire.h>
#include <DS18B20.h>
#include <DallasTemperature.h>

#define DHT_PIN 16   

OneWire oneWire(17);
DallasTemperature t(&oneWire);

#define DHTTYPE DHT11

const char* ssid = "HOME_Wi-Fi";
const char* password = "home1112";

const char* mqtt_server = "192.168.1.101";
const char* topic = "home/datatopic";
   
// Initializes the espClient
WiFiClient ESP_IN;
PubSubClient client(ESP_OUT);

DHT sensor1(DHT_PIN, DHTTYPE);

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
  if (now - lastMeasure > 100000) {
    lastMeasure = now;
    float h = sensor1.readHumidity();
    t.requestTemperaturesByIndex(0);


    // Check if any reads failed and exit early (to try again).
    if (isnan(h)) {
      Serial.println("Failed to read from sensor!");
      return;
    }

    static char temperatureTemp[7];
    
    static char humidityTemp[7];
    dtostrf(h, 6, 2, humidityTemp);


    char buffer[128];
     
    //Publishes Temperature and Humidity values    
    sprintf(buffer, "{\"v\": %.2f, \"t\": \"%c\", \"id\": \"%s\"}", h , 'h', "sensor1");

    client.publish("home/datatopic", buffer);
        
    Serial.println(buffer);
    
  
    sprintf(buffer, "{\"v\": %.2f, \"t\": \"%c\", \"id\": \"%s\"}", t , 't', "sensor2");


    client.publish("home/datatopic", buffer);
        
    Serial.println(buffer);
    
  }
}

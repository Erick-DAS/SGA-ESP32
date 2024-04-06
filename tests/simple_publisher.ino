//WiFi library for the esp32
#include <WiFi.h>
//Library used as client for all mqtt services
#include <PubSubClient.h>

//**** WiFi credentials *****
const char* ssid = "WIFI-NAME";
const char* password = "WIFI-PASSWORD";
const char* mqtt_server = "BROKER-IP";
const int connection_port = 0; // Insert the desired port here (usually 1883 works on a local broker)

//create wifi client 
WiFiClient espClient;
//create PubSubClient
PubSubClient client(espClient);
//variable for time ellapsed
long lastMsg = 0;

//variable for temperature. Here we generate random values but you can easily replace this with a sensor generated one
float temperature = 0;

//setup()
void setup() {
  //initialize serial communication at 115200 kB/s
  Serial.begin(115200);
  //initialize WiFi and MQTT
  setup_wifi();
  //initialize server at XXXXXXXX:1883
  client.setServer(mqtt_server, 1883);
}

void setup_wifi() {
  //some serial prints
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  //connect to WiFi network
  WiFi.begin(ssid, password);

  //test to see, if connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  //print the ip address of the esp32
  Serial.println(WiFi.localIP());
}

//function to reconnect if disconnected from the server
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    //random client ID, you can put whatever you want.
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      //if not connected, print the error code
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

//loop()
void loop() {
  if (!client.connected()) {
    //reconnect to MQTT broker if not connected
    reconnect();
  }
  client.loop();
  long now = millis();
  //send message after 5s
  if (now - lastMsg > 5000) {
    lastMsg = now;
    //generate random temperature value
    temperature = random(0, 300);   
    char tempString[8];
    //convert temperature to string with 2 decimal places
    dtostrf(temperature, 1, 2, tempString);
    Serial.print("Temperature: ");
    Serial.println(tempString);
    // Publish temperature value to MQTT topic named esp32/temperature
    client.publish("esp32/teste", tempString);
  }
}
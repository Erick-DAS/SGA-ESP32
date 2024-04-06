#include <WiFi.h>
#include <PubSubClient.h>

// PIN setup variables

const int PINO_COMEU_MACA = 39; // FPGA signal 
const int PINO_BUZZER = 33; // Buzzer pin 
const int PINO_3V3 = 32; // 3V3 pin
const int PINO_LED = 2; // Internal LED for debugging
const int PWM_CHANNEL = 0; // PWM Channel for buzzer

// Network setup variables
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

#define PWM_FREQ 1000
#define PWM_RES 8

//setup()
void setup() {
  // PIN/PWM setup
  Serial.begin(115200); // Baud Rate
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES); // Setups PWM channel
  ledcAttachPin(PINO_BUZZER, PWM_CHANNEL); 
  pinMode(PINO_LED, OUTPUT); 
  pinMode(PINO_COMEU_MACA, INPUT); 
  pinMode(PINO_3V3, OUTPUT);   


  // Network setup
  setup_wifi();
  client.setServer(mqtt_server, connection_port);
}

void setup_wifi() {
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

void loop() {
    digitalWrite(PINO_3V3, HIGH);

    int comeu_maca = digitalRead(PINO_COMEU_MACA);
    Serial.println("comeu_maca: ");
    Serial.println(comeu_maca);

    int melody[] = {
        440, 470, 500, 395, 410, 440, 470, 500
    };

    int tempo[] = {
        50, 50, 50, 50, 50, 50, 50, 50
    };

    if (!client.connected()) {
        // Reconnect to MQTT broker if not connected
        reconnect();
    }
    
    client.loop();
    long now = millis();

  if (comeu_maca == 1) {
    digitalWrite(PINO_LED, HIGH);
    client.publish("esp32/teste", "Estou apitando");
    for (int i = 0; i < sizeof(melody) / sizeof(melody[0]); i++)  {
      ledcWrite(PWM_CHANNEL, melody[i]);
      delay(tempo[i] * 1.30);             // Pequeno delay para espaçar as notas
      ledcWrite(PWM_CHANNEL, 0);          // Desliga o buzzer ao final de cada nota
      delay(25);                          // Pequeno delay para evitar cliques entre as notas
    }
  }
  else {
    digitalWrite(PINO_LED, LOW);
    client.publish("esp32/teste", "Nao estou apitando");
  }
  
  delay(50); // Pausa na musica por 50 ms

}
#include <WiFi.h>
#include <PubSubClient.h>
#include <stdio.h>
#include <stdlib.h>

// PIN setup variables

// SNAKE_POS

const int PINO_SNAKE6 = 36;
const int PINO_SNAKE5 = 39;
const int PINO_SNAKE4 = 34;
const int PINO_SNAKE3 = 35;
const int PINO_SNAKE2 = 32;
const int PINO_SNAKE1 = 33;

 
// APPLE_POS

const int PINO_APPLE1 = 23;
const int PINO_APPLE2 = 22;
const int PINO_APPLE3 = 5;
const int PINO_APPLE4 = 21;
const int PINO_APPLE5 = 19;
const int PINO_APPLE6 = 18;

// STATE

const int PINO_STATE1 = 27;
const int PINO_STATE2 = 13;
const int PINO_STATE3 = 16;
const int PINO_STATE4 = 4;
const int PINO_STATE5 = 15;

// VELOCITY

const int PINO_VEL = 14;

// MODE

const int PINO_MODE = 12;

// DIFF

const int PINO_DIFF = 2;

// const int PINO_3V3 = 15; 

// COMEU MACA

const int PINO_COMEU_MACA = 25; // FPGA signal 

// BUZZER

const int PINO_BUZZER = 26; // Buzzer pin

// LED
// const int PINO_LED = 2; // Internal LED for debugging

const int PWM_CHANNEL = 0; // PWM Channel for buzzer

// Network setup variables
const char* ssid = "Das";
const char* password = "12345678";
const char* mqtt_server = "192.168.135.253";
const int connection_port = 1883;

//create wifi client 
WiFiClient espClient;
//create PubSubClient
PubSubClient client(espClient);

#define PWM_FREQ 1000
#define PWM_RES 8

int comeu_maca = 0;

String snake_pos_string = "";
String apple_pos_string = "";
String state_string = "";

String new_snake_pos_string = "";
String new_apple_pos_string = "";
String new_state_string = "";

bool updated = false;



//setup()
void setup() {
  // PIN/PWM setup
  Serial.begin(115200); // Baud Rate
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES); // Setups PWM channel
  ledcAttachPin(PINO_BUZZER, PWM_CHANNEL);

  pinMode(PINO_SNAKE6, INPUT);
  pinMode(PINO_SNAKE5, INPUT);
  pinMode(PINO_SNAKE4, INPUT);
  pinMode(PINO_SNAKE3, INPUT);
  pinMode(PINO_SNAKE2, INPUT);
  pinMode(PINO_SNAKE1, INPUT); 
  
  pinMode(PINO_APPLE1, INPUT);
  pinMode(PINO_APPLE2, INPUT);
  pinMode(PINO_APPLE3, INPUT);
  pinMode(PINO_APPLE4, INPUT);
  pinMode(PINO_APPLE5, INPUT);
  pinMode(PINO_APPLE6, INPUT);

  pinMode(PINO_STATE1, INPUT);
  pinMode(PINO_STATE2, INPUT);
  pinMode(PINO_STATE3, INPUT);
  pinMode(PINO_STATE4, INPUT);
  pinMode(PINO_STATE5, INPUT);

  //pinMode(PINO_3V3, OUTPUT);

  //pinMode(PINO_LED, OUTPUT);

  pinMode(PINO_VEL, INPUT); 
  pinMode(PINO_MODE, INPUT); 
  pinMode(PINO_DIFF, INPUT); 
  pinMode(PINO_COMEU_MACA, INPUT);   


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
    Serial.println("Trying to connect to wifi \n");
    Serial.println(ssid);
    Serial.println(password);
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

    int snake_pos[6] = {digitalRead(PINO_SNAKE6),
                        digitalRead(PINO_SNAKE5),
                        digitalRead(PINO_SNAKE4),
                        digitalRead(PINO_SNAKE3),
                        digitalRead(PINO_SNAKE2),
                        digitalRead(PINO_SNAKE1)};

    int apple_pos[6] = {digitalRead(PINO_APPLE1),
                        digitalRead(PINO_APPLE2),
                        digitalRead(PINO_APPLE3),
                        digitalRead(PINO_APPLE4),
                        digitalRead(PINO_APPLE5),
                        digitalRead(PINO_APPLE6)};

    int state[5]     = {digitalRead(PINO_STATE1),
                        digitalRead(PINO_STATE2),
                        digitalRead(PINO_STATE3),
                        digitalRead(PINO_STATE4),
                        digitalRead(PINO_STATE5)};

    new_snake_pos_string = "";
    new_apple_pos_string = "";
    new_state_string = "";

    for (int i = 0; i < 6; i++) {
      new_snake_pos_string += String(snake_pos[i]);
      new_apple_pos_string += String(apple_pos[i]);
      if (i < 5) {
        new_state_string += String(state[i]);
      }
    }

    int new_comeu_maca = digitalRead(PINO_COMEU_MACA);
    
    int vel_select = digitalRead(PINO_VEL);
    int mode_select = digitalRead(PINO_MODE);
    int diff_select = digitalRead(PINO_DIFF);

    if (new_comeu_maca != comeu_maca 
    // || strcmp(new_snake_pos_string.c_str(), snake_pos_string.c_str()) != 0 || strcmp(new_apple_pos_string.c_str(), apple_pos_string.c_str()) != 0 || strcmp(new_state_string.c_str(), state_string.c_str()) != 0
    ) {
      updated = true;
    }
    else {
      updated = false;
    }

    int melody[] = {
        440, 100, 850
    };

    int tempo[] = {
        50, 50, 50
    };

    if (!client.connected()) {
        // Reconnect to MQTT broker if not connected
        reconnect();
    }
    
    client.loop();

  //if (updated) {
    Serial.println("comeu_maca: ");
    Serial.println(new_comeu_maca);

    snake_pos_string = new_snake_pos_string;
    apple_pos_string = new_apple_pos_string;
    state_string = new_state_string;

    client.publish("snake_game/snake_pos", snake_pos_string.c_str());
    client.publish("snake_game/apple_pos", apple_pos_string.c_str());
    client.publish("snake_game/state", state_string.c_str());

    client.publish("snake_game/vel_selector", String(vel_select).c_str());
    client.publish("snake_game/mode_selector", String(mode_select).c_str());
    client.publish("snake_game/diff_selector", String(diff_select).c_str());

    if (new_comeu_maca == 1) {
      comeu_maca = 1;
      // digitalWrite(PINO_LED, HIGH);
      client.publish("snake_game/comeu_maca", "1");

      for (int i = 0; i < sizeof(melody) / sizeof(melody[0]); i++)  {
        ledcWrite(PWM_CHANNEL, melody[i]);
        delay(tempo[i] * 1.30);             // Pequeno delay para espaçar as notas
        ledcWrite(PWM_CHANNEL, 0);          // Desliga o buzzer ao final de cada nota
        if (i != sizeof(melody) / sizeof(melody[0]) - 1)
          delay(25);                          // Pequeno delay para evitar cliques entre as notas
      }

    }
    else {
      comeu_maca = 0;
      // digitalWrite(PINO_LED, LOW);
      client.publish("snake_game/comeu_maca", "0");
    }
  //}
}
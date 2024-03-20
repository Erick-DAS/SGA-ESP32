#include <WiFi.h>
#include <PubSubClient.h>
#include <stdio.h>
#include <stdlib.h>

// PIN setup variables

// SNAKE_POS

const int PINO_SNAKE1 = 36;
const int PINO_SNAKE2 = 39;
const int PINO_SNAKE3 = 34;
const int PINO_SNAKE4 = 35;
const int PINO_SNAKE5 = 32;
const int PINO_SNAKE6 = 33;


// APPLE_POS

const int PINO_APPLE1 = 23;
const int PINO_APPLE2 = 22;
const int PINO_APPLE3 = 3;
const int PINO_APPLE4 = 21;
const int PINO_APPLE5 = 19;
const int PINO_APPLE6 = 18;

// STATE

const int PINO_STATE1 = 27;
const int PINO_STATE2 = 13;
const int PINO_STATE3 = 17;
const int PINO_STATE4 = 4;
const int PINO_STATE5 = 15;

// COMEU MACA

const int PINO_COMEU_MACA = 25; // FPGA signal 

// BUZZER

const int PINO_BUZZER = 26; // Buzzer pin

// LED
const int PINO_LED = 2; // Internal LED for debugging

const int PWM_CHANNEL = 0; // PWM Channel for buzzer

// Network setup variables
const char* ssid = "NOME";
const char* password = "SENHA";
const char* mqtt_server = "IP DO PC";
const int connection_port = 1883;

//create wifi client 
WiFiClient espClient;
//create PubSubClient
PubSubClient client(espClient);

#define PWM_FREQ 1000
#define PWM_RES 8

//setup()
void setup() {
  // PIN/PWM setup
  Serial.begin(115200); // Baud Rate
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES); // Setups PWM channel
  ledcAttachPin(PINO_BUZZER, PWM_CHANNEL);

  pinMode(PINO_SNAKE1, INPUT);
  pinMode(PINO_SNAKE2, INPUT);
  pinMode(PINO_SNAKE3, INPUT);
  pinMode(PINO_SNAKE4, INPUT);
  pinMode(PINO_SNAKE5, INPUT);
  pinMode(PINO_SNAKE6, INPUT); 
  
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

  pinMode(PINO_LED, OUTPUT); 
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

char *stringify(int* int_array, int length) {

    // Calculate the total length needed for the string
    int total_length = length * 2 + 1; // Twice the length of the integer array plus one for the null terminator
    
    // Allocate memory for the string
    char* char_array = (char*)malloc(total_length * sizeof(char));
    
    if (char_array == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    
    // Convert integers to characters
    int i;
    char* ptr = char_array;
    for (i = 0; i < length; i++) {
        ptr += sprintf(ptr, "%d", int_array[i]);
    }
    
    // Add null terminator
    *ptr = '\0';
    
    return char_array;
}

void loop() {
    int snake_pos[6] = {digitalRead(PINO_SNAKE1),
                        digitalRead(PINO_SNAKE2),
                        digitalRead(PINO_SNAKE3),
                        digitalRead(PINO_SNAKE4),
                        digitalRead(PINO_SNAKE5),
                        digitalRead(PINO_SNAKE6)};

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

    char *snake_pos_string = stringify(snake_pos, 6);
    char *apple_pos_string = stringify(apple_pos, 6);
    char *state_string = stringify(state, 6);

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

    

  client.publish("snake_game/snake_pos", snake_pos_string);
  client.publish("snake_game/apple_pos", apple_pos_string);
  client.publish("snake_game/state", state_string);

  if (comeu_maca == 1) {
    digitalWrite(PINO_LED, HIGH);
    client.publish("esp32/teste", "Estou apitando");
    client.publish("snake_game/comeu_maca", "1");
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
    client.publish("snake_game/comeu_maca", "0");
  }
  
  delay(50); // Pausa na musica por 50 ms

}
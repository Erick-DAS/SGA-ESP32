#include <WiFi.h>
#include <PubSubClient.h>

// PIN setup variables
const int PINO_TESTADO = 39; // FPGA signal 
const int PINO_BUZZER = 33; // Buzzer pin 
const int PINO_3V3 = 32; // 3V3 pin
const int PINO_LED = 2; // Internal LED for debugging
const int PWM_CHANNEL = 0; // PWM Channel for buzzer

#define PWM_FREQ 1000
#define PWM_RES 8

//setup()
void setup() {
  // PIN/PWM setup
  Serial.begin(115200); // Baud Rate
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES); // Setups PWM channel
  ledcAttachPin(PINO_BUZZER, PWM_CHANNEL); 
  pinMode(PINO_LED, OUTPUT); 
  pinMode(PINO_TESTADO, INPUT); 
  pinMode(PINO_3V3, OUTPUT);   

}

void loop() {
    digitalWrite(PINO_3V3, HIGH);

    int comeu_maca = digitalRead(PINO_TESTADO);
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
    for (int i = 0; i < sizeof(melody) / sizeof(melody[0]); i++)  {
      ledcWrite(PWM_CHANNEL, melody[i]);
      delay(tempo[i] * 1.30);             // Pequeno delay para espaçar as notas
      ledcWrite(PWM_CHANNEL, 0);          // Desliga o buzzer ao final de cada nota
      delay(25);                          // Pequeno delay para evitar cliques entre as notas
    }
  }
  else {
    digitalWrite(PINO_LED, LOW);
  }
  
  delay(50); // Pausa na musica por 50 ms

}
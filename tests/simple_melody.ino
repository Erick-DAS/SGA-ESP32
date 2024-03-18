#include <Arduino.h>

const int PINO_COMEU_MACA = 39; // FPGA signal D35 GPIO35
const int PINO_BUZZER = 33; // Buzzer pin D33 GPIO33
const int PINO_3V3 = 32; // 3v3 signal pin
const int PINO_LED = 2; // Internal LED GPIO2
const int PWM_CHANNEL = 0; // PWM Channel for buzzer

#define PWM_FREQ 1000
#define PWM_RES 8

void setup() {
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES); // Setups PWM channel
  ledcAttachPin(PINO_BUZZER, PWM_CHANNEL); // Setups Buzzer pin with PWM
  pinMode(PINO_LED, OUTPUT); // Setups internal led pin
  // pinMode(PINO_3V3, OUTPUT); // Uncomment for 3v3 feeding pin
  pinMode(PINO_COMEU_MACA, INPUT);   
  Serial.begin(115200);
}

void loop()  {
  // digitalWrite(PINO_3V3, HIGH); // Uncomment for 3v3 feeding pin 

  // int pino_3v3 = digitalRead(PINO_3V3); // Uncomment for 3v3 feeding pin
  // Serial.println("3v3: ");
  // Serial.println(pino_3v3);

  int comeu_maca = digitalRead(PINO_COMEU_MACA);
  Serial.println("comeu_maca: ");
  Serial.println(comeu_maca);

  int melody[] = {
    440, 470, 500, 395, 410, 440, 470, 500
  };

  int tempo[] = {
    50, 50, 50, 50, 50, 50, 50, 50
  };
  
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
#include <Arduino.h>

const int PINO_BUZZER = 33; // Buzzer pin D33 GPIO33
const int PINO_LED = 2; // Internal LED GPIO2
const int PWM_CHANNEL = 0; // PWM Channel for buzzer

#define PWM_FREQ 1000
#define PWM_RES 8

void setup() {
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES); // Setups PWM channel
  ledcAttachPin(PINO_BUZZER, PWM_CHANNEL); // Setups Buzzer pin with PWM
  pinMode(PINO_LED, OUTPUT); // Setups internal led pin 
  Serial.begin(115200);
}

void loop()  {

  int melody[] = {
    440, 470, 500, 395, 410, 440, 470, 500
  };

  int tempo[] = {
    50, 50, 50, 50, 50, 50, 50, 50
  };

  for (int i = 0; i < sizeof(melody) / sizeof(melody[0]); i++)  {
    ledcWrite(PWM_CHANNEL, melody[i]);
    delay(tempo[i] * 1.30);             // Pequeno delay para espaçar as notas
    ledcWrite(PWM_CHANNEL, 0);          // Desliga o buzzer ao final de cada nota
    delay(25);                          // Pequeno delay para evitar cliques entre as notas
  }
  
  delay(2000); // Pausa na musica por 2 segundos
}
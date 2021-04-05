/*
 * 
 * 
 * 
 * 
 * 
 * 
 */

//=============================== Inclusão das Bibliotecas ===============================
#include <Arduino.h>                                     // Inclusão da biblioteca do Arduino
#include "MHZ19.h"                                       // Inclusão da biblioteca do sensor Mh-Z19B
#include <SoftwareSerial.h>                              // Inclusão da biblioteca SoftwareSerial

//=============================== Definição dos LEDs ===============================
#define LedVd1 2                                         // Define o primeiro led verde na porta 2
#define LedVd2 3                                         // Define o segundo led verde na porta 3
#define LedAm1 4                                         // Define o primeiro led amarelo na porta 4
#define LedAm2 5                                         // Define o segundo led amarelo na porta 5
#define LedVm1 6                                         // Define o primeiro led vermelho na porta 6
#define Buzzer 7                                         // Define o buzzer na porta 7

//=============================== Definição do RX e TX do Sensor ==============================
#define RX_PIN 9                                         // Define a porta 9 do arduino como RX_PIN
#define TX_PIN 10                                        // Define a porta 10 do arduino como TX_PIN
#define VELOCIDADE 9600                                  // Define 9600 como VELOCIDADE

MHZ19 myMHZ19;                                           // Constructor for library
SoftwareSerial mySerial(RX_PIN, TX_PIN);                 // Usa a biblioteca SoftwareSerial para definir RX_PIN e TX_PIN como RX e TX no arduino

//=============================== Variáveis Globais ==============================
unsigned long getDataTimer = 0;                          // Define a variável para controle do tempo de amostragem do sensor MH-Z19B
int CO2;                                                 // Define a variável "CO2" como número inteiro
int maiorValor;                                          // Define a variável "maiorValor", que armazena a maior leitura de CO2 registrada 
int intervaloAlarme;                                     // Define a variável "intervaloAlarme" para controole do tempo de bip do Alarme

//=============================== Declaração das Funções ==============================
void exibe_ppm(void);
void testeLed(void);
void atualiza_Barra(void);
//void S_O_S(void);
//void Alarme(int);


void setup() {
  Serial.begin(9600);                                    // Inicia o Serial a 9600 bits [pesquisar direito]
  mySerial.begin(VELOCIDADE);                            // Inicia o MySerial na velocidade
  myMHZ19.begin(mySerial);                               // Define LedVd1 como saída
  myMHZ19.autoCalibration();
  
  pinMode(LedVd1, OUTPUT);                               // Define LedVd1 como saída
  pinMode(LedVd2, OUTPUT);                               // Define LedVd2 como saída
  pinMode(LedAm1, OUTPUT);                               // Define LedAm1 como saída
  pinMode(LedAm2, OUTPUT);                               // Define LedAm2 como saída
  pinMode(LedVm1, OUTPUT);                               // Define LedVm1 como saída
  pinMode(Buzzer, OUTPUT);                               // Define Buzzer como saída
  
  testeLed();                                            // Realiza a função de testLed

  delay(1000);
}

void loop() {
  exibe_ppm();
  atualiza_Barra();
  digitalWrite(LedVd1, CO2 > 390 ? HIGH : LOW);
  digitalWrite(LedVd2, CO2 > 800 ? HIGH : LOW);
  digitalWrite(LedAm1, CO2 > 1000 ? HIGH : LOW);
  digitalWrite(LedAm2, CO2 > 1200 ? HIGH : LOW);
  digitalWrite(LedVm1, CO2 > 1600 ? HIGH : LOW);
  
  if(CO2 >= 1600){
    tone(Buzzer, 750);
    delay(150);
    noTone (Buzzer);
    delay(100); 
    }
}
//================================================================================================ Teste dos LEDs ================================================================================================
void testeLed(void) {
  for(int x=2; x<7; x++) {
    digitalWrite(x, HIGH);
    delay(500);
    digitalWrite(x, LOW);
    delay(500);
  }
  tone(Buzzer, 750);
  delay(300);
  noTone(Buzzer);
}
//================================================================================================ PPM ================================================================================================
void exibe_ppm(void) {
  int8_t Temperatura;                                      // Define a variável Temperatura como número inteiro
    
  if(millis() - getDataTimer >= 2000) {                    // Condicional de intervalo de captura de informação do sensor de CO2 (a cada 2 segundos) 
    CO2 = myMHZ19.getCO2();                                // Armazena a leitura de CO2 (PPM) na variável global "CO2"
    Temperatura = myMHZ19.getTemperature();                // Armazena a leitura de temperatura (ºC) na variável "Temperatura"
    getDataTimer = millis();                               // Armazenando na variável "getDataTimer" o valor atual do tempo de execução do arduíno
  }
  if(CO2 > maiorValor) {                                   // Condicional se leitura atual de "CO2" é maior do que a leitura armazenada na variável "maioValor"
    maiorValor = CO2;                                        // Atualiza a variável "maiorValor"
  }
    
  Serial.print("va.val=");                                 // Informa na saída Serial o valor da variável "CO2"
  Serial.print(CO2);
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);

  Serial.print("mvr.val=");                               // Informa na saída Serial o valor da variável "maiorValor"
  Serial.print(maiorValor);
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);

  Serial.print("ta.val=");                                // Informa na saída Serial o valor da variável "Temperatura"
  Serial.print(Temperatura);
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);
  }

//================================================================================================ Barra de Progresso ================================================================================================
void atualiza_Barra(void) {
  if (CO2 <= 2000) {
  int valorBarra = map(CO2, 0, 2000, 0, 100);          // Defini o limite do valor como 2000 e não como 5000 pois o design da barra iria somente até os 2000

    Serial.print("bar_niv.val="); 
    Serial.print(valorBarra); 
    Serial.write(0xff);
    Serial.write(0xff);
    Serial.write(0xff);
  } else {                                           // quando o valor do PPM superava os 2000 PPM, a barra estava mostra 100%
    Serial.print("bar_niv.val=");
    Serial.print(100);
    Serial.write(0xff);
    Serial.write(0xff);
    Serial.write(0xff);  
  }
}


//================================================================================================ Funcionamento dos Toques ================================================================================================
/*void Alarme (int j) {
  if(millis() - intervaloAlarme >= 60000) {                    // Condicional de intervalo para bip a cada 60 segundos
    if(j = 1) {tone(Buzzer, 500); delay(300); noTone(Buzzer); delay(300);}
    if(j = 2) {
    for(int k=1; k<3; k++){tone(Buzzer, 500); delay(300); noTone(Buzzer); delay(300); }
    }
    if(j = 3) {
    for(int l=1; l<4; l++){tone(Buzzer, 500); delay(300); noTone(Buzzer); delay(300); }
    }
    intervaloAlarme = millis();                               // Armazena na variável "intervaloAlarme" o valor atual do tempo de execução do arduíno
  }
  j = 0;
}*/
 
//================================================================================================ Toque de SOS ================================================================================================
/*void S_O_S(void) {
  for(int y = 0; y < 3; y++) {
    tone(Buzzer, 750);
    delay(150);
    noTone (Buzzer);
    delay(100);    
  }
  delay(200);

  for(int y = 0; y < 3; y++) {
    tone(Buzzer, 750);
    delay(400);
    noTone (Buzzer);
    delay(100);   
  }
  delay(200);
    
  for(int y = 0; y < 3; y++) {
    tone(Buzzer, 750);
    delay(150);
    noTone (Buzzer);
    delay(100);   
  }
  delay(1000);
}*/

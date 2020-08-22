#include <Ultrasonic.h> //Carrega a biblioteca do sensor ultrassonico
#include <EEPROM.h> //Carrega a biblioteca da EEPROM
#include <AltSoftSerial.h> //Carrega a biblioteca da AltSoftSerial
#include <DallasTemperature.h> //Carrega a biblioteca da AltSoftSerial para trabalgar com sensor de temperatura
//#include <OneWire.h>
#include <avr/wdt.h> // Carrega o Watchdog - Sistema de Reset Automático

#define DS18B20 7 //DEFINE O PINO DIGITAL UTILIZADO PELO SENSOR

OneWire ourWire(DS18B20); //Configura uma instância ONEWIRE para se comunicar com o sensor de temperatura
DallasTemperature sensors(&ourWire); //BIBLIOTECA DallasTemperature UTILIZA A OneWire



AltSoftSerial altSerial;

#include "Metodos_Auxiliares.h"
String input;
String inputBT;
#define MAX_LENGTH 64
#define MAX_LENGTH_BT 64 
//Define os pinos para o trigger e echo
#define pino_trigger 11
#define pino_echo 10
Ultrasonic ultrasonic(pino_trigger, pino_echo);//Inicializa o sensor nos pinos definidos acima

//Freedom Board Pinos:
/*
D2 - Circuito detector de tensão (Zero Cross).
D3 - Saída para o Relé.
D4 - Saída para o Relé.
D5 - Saída para o Relé.
D6 - Saída para o Rele.
D7 a D13 - Disponível via Borne (Entradas ou Saídas Digitais).
A0 a A2 -  Disponível via Borne (Pinos Analógicos).
*/

void setup(){
  //Inicializa o watchdog timer
  //WDTO_8S, WDTO_4S, WDTO_1S, WDTO_500MS, WDTO_250MS,WDTO_120MS, WDTO_60MS, WDTO_30MS e WDTO_15MS.
  wdt_enable(WDTO_8S);
  
  //Inicializa a comuniocação serial
  Serial.begin(57600);// Configura a velocidade da comunicação serial
  altSerial.begin(9600);//Configura a velocidade da comunicação serial emulada
  //altSerial.println("OK");
  
  //Inicializa os sensores
  sensors.begin(); //Inicia o sensor
  
  //Variaveis auxiliares para tratamento serial
  input.reserve(MAX_LENGTH + 1);  // Define o tamanho máximo do buffer (+ 1 por causa do \0 no final)
  inputBT.reserve(MAX_LENGTH_BT + 1);  // Define o tamanho máximo do buffer (+ 1 por causa do \0 no final)

  //Mensagem de saudação
  Saudacao();
  }

void loop(){
    wdt_reset();//Reinicia o wdt.
    cmdSER();
    cmdBT();  
    //terminalHC_06();//Terminal direto com o bluetooth  
    //delay(500);
    //memoriaLivre_print();
    //distCm();
    //delay(500);
  }

void cmdSER(){
//Tratamento dos comandos via serial
  if (Serial.available()) {
       char c = Serial.read(); // Recebe um caracter
       if(c == '\n') {     // Se foi digitado um ENTER entao processa a String  if(c == '\n')    
         //Serial.println("A string digitada foi ");
         //Serial.println(input);
         
         input.toUpperCase();    // Converte toda a String para maiusculo
         input.trim();           // Tira os espacos antes e depois
         
          if(input == "HELP"){    
            Serial.println("Lista de Comandos:");
            Serial.println("TEMP - Retorna a temperatura no sensor");
            Serial.println("DIST - Retorna a distancia lida pelo sensor");
            Serial.println("MEMO - Retorna a quantidade de memória livre");
         } 
          else if(input == "TEMP"){    
            temp();
         }
          else if (input == "DIST") {
            distCm();
         }  
          else if (input == "MEMO") {
            memoriaLivre_print();
         }            
         input = "";// Limpa a String para comecar a armazenar de novo         
       } 
    else {    
         // Se nao veio um ENTER entao vai armazenando até o tamanho maximo que a string pode suportar        
         if(input.length() < MAX_LENGTH) {
            if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')){ // Aqui estamos filtrando so as letras. Poderia filtrar numeros e espaco por exemplo
                input += c;
            }
         }
       }
    }
    }

 void cmdBT(){
//Tratamento dos comandos via serialemulada 
  if (altSerial.available()) {
       char c = altSerial.read(); // Recebe um caracter
       if(c == '\n') {     // Se foi digitado um ENTER entao processa a String  if(c == '\n')    
         //Serial.println("A string digitada pelo bluetooth foi ");
         //Serial.println(inputBT);
         //altSerial.println(inputBT);
         
         inputBT.toUpperCase();    // Converte toda a String para maiusculo
         inputBT.trim();           // Tira os espacos antes e depois
                 
         if(inputBT == "HELP"){    
          Serial.println("Lista de Comandos:");
          Serial.println("TEMP - Retorna a temperatura no sensor");
          Serial.println("DIST - Retorna a distancia lida pelo sensor");
          Serial.println("MEMO - Retorna a quantidade de memória livre");
         }
         else if (inputBT == "TEMP") {
          temp();
         }  
         else if (inputBT == "DIST") {
          distCm();
         }  
         else if (inputBT == "MEMO") {
           memoriaLivre_print();
         }    
         inputBT = "";// Limpa a String para comecar a armazenar de novo         
       } 
    else {    
         // Se nao veio um ENTER entao vai armazenando até o tamanho maximo que a string pode suportar        
         if(inputBT.length() < MAX_LENGTH_BT) {
            if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')){ // Aqui estamos filtrando so as letras. Poderia filtrar numeros e espaco por exemplo
                inputBT += c;
            }
         }
       }
    }
    }



void terminalHC_06(){  
  char c;
  if (Serial.available()) {
    c = Serial.read();
    altSerial.print(c);
  }
  if (altSerial.available()) {
    c = altSerial.read();
    Serial.print(c);
  }
}
  /*Comandos Bluetooth HC06:
    AT – Retorna “OK”, o que indica que o módulo bluetooth está respondendo aos comandos
    AT+NAME : Altera o nome do módulo Bluetooth
    AT+VERSION – Retorna a versão do firmware da placa
    AT+PINXXXX – Altera a senha do bluetooth, onde XXXX é a nova senha, e você pode utilizar qualquer combinação de 4 números. Este comando retorna “OKsetPIN”
    AT+BAUDX – Altera o baud rate (velocidade de transmissão) do módulo, sendo que X por ter os valores de 1 a 9, conforme tabela abaixo :
     Valor    
     Baud Rate   1           1200   
                 2           2400   
                 3           4800   
                 4           9600 (default)   
                 5           19200   
                 6           38400   
                 7           57600   
                 8           115200   
                 9           230400
    */
void distCm(){
    
    //float volumeLitros = 0;
    //float compCm = 100;
    //float diamCM = 40;
    int cmMsec;
    //float cmMsec;
    long microsec = ultrasonic.timing();
    cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
    //Exibe informacoes no serial monitor
    //Serial.print("Comprimento do Cilindro: "); Serial.print(compCm); Serial.println("cm");
    //Serial.print("Diametro do Cilindro: "); Serial.print(diamCM); Serial.println("cm");
    //volumeLitros = 3.14*(diamCM/2)*(compCm-cmMsec);
    //Serial.print("Litros no Cilindro: "); Serial.print(volumeLitros);Serial.print(" Litros");

    Serial.print("Distancia: "); Serial.print(cmMsec); Serial.println("cm");
    altSerial.print(cmMsec);
    //altSerial.print("/n");
    ///////////Fórmula Como Calcular o Volume de 1 (um) Cilindro/////
    //V=pi*r2*h (Volume é igual a pi vezez raio ao quadrado vezes a altura).
    //V é dado em metros cubicos, 1m3 = 1000 litros. 
    
    /*
    //Le as informacoes do sensor, em cm e pol e imprime na seiral
    float cmMsec, inMsec;
    long microsec = ultrasonic.timing();
    cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
    inMsec = ultrasonic.convert(microsec, Ultrasonic::IN);
    //Exibe informacoes no serial monitor
    Serial.print("Distancia em cm: ");
    Serial.print(cmMsec);
    Serial.print(" - Distancia em polegadas: ");
    Serial.println(inMsec);
    */
}

void temp(){
  int auxtemp;
  //float auxtemp;
  sensors.requestTemperatures();//SOLICITA QUE A FUNÇÃO INFORME A TEMPERATURA DO SENSOR
  //Serial.print("Temperatura: "); //IMPRIME O TEXTO NA SERIAL
  //Serial.print(sensors.getTempCByIndex(0)); //IMPRIME NA SERIAL O VALOR DE TEMPERATURA MEDIDO
  //Serial.println("*C "); //IMPRIME O TEXTO NA SERIAL
  auxtemp = sensors.getTempCByIndex(0);
  Serial.print("Temperatura: "); Serial.print(auxtemp); Serial.println("°C");
  altSerial.print(auxtemp);
  //altSerial.print("/n");
  //delay(250);//INTERVALO DE 250 MILISSEGUNDOS
}

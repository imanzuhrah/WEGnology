/*
 * CT ELETROELETRONICA                     
 * SENAI CFP 7.91 BOTUCATU - SP
 * APLICAÇÃO: Criando dashboards com variaveis discretas e analogicas
 * DESCRIÇÃO: Estudo do MQTT e Plataforma Wegnology
 * DESENVOLVIDO POR: PROF.LUIZ HENRIQUE RAMOS 
*/

// INCLUSÃO DE BIBLIOTECAS
#include <ArduinoJson.h>
#include <WiFiS3.h>
#include <PubSubClient.h>

// DEFINIÇÃO DAS CONFIGURAÇÕES DE COMUNICAÇÃO
// CONEXÃO COM INTERNET 
const char* REDE_WIFI = "automacao"; // REDE
const char* SENHA_WIFI = "Sen@i791"; // SENHA

const int btnPin = 8; // Pino do botão

// CONEXÃO COM O BROKER MQTT E PLATAFORMA WEGNOLOGY
const char* BROKER = "broker.app.wnology.io"; 
const char* ACCESS_KEY = "3912dd58-4056-4521-8054-b29533e04852";
const char* ACCESS_SECRET = "4c6a64609465fd11bc1ae82ca3d5b1803ba433d854f261e2c9ee319923a1ad5c";
const char* ID_DISPOSITIVO = "68c59e76cadd796e6852ead4";
const char* TOPICO_PUBLICACAO = "wnology/68c59e76cadd796e6852ead4/state"; 
const int PORTA_MQTT = 1883;  
const int INTERVALO = 3000; // intervalo de envio

WiFiClient cursoIoT;
PubSubClient client(cursoIoT);

// VARIÁVEIS 
long tempoAnterior; 
char ATRIBUTOS[200]; 

// FUNÇÃO DE INICIALIZAÇÃO
void setup() 
{
  pinMode(btnPin, INPUT_PULLUP);
  Serial.begin(115200);
  
  conexao_wifi();
  conexao_broker();
}

// FUNÇÃO DE CONEXÃO COM O WIFI
void conexao_wifi()
{
  WiFi.begin(REDE_WIFI, SENHA_WIFI);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Conectando ao Wi-Fi...");
  }
  Serial.println("Conectado à rede Wi-Fi");
}

// FUNÇÃO DE CONEXÃO COM O BROKER MQTT
void conexao_broker()
{
  client.setServer(BROKER, PORTA_MQTT);
  while (!client.connected())
  {
    Serial.println("Conectando ao servidor MQTT...");
    conexao_mqtt();
  }
}

// FUNÇÃO DE CONEXÃO COM A PLATAFORMA WEGNOLOGY
void conexao_mqtt()
{
  if (client.connect(ID_DISPOSITIVO, ACCESS_KEY, ACCESS_SECRET)) 
  {
    Serial.println("Conectado à plataforma WEGNOLOGY");
  } 
  else 
  {
    Serial.print("Falha na conexão à plataforma WEGNOLOGY, erro: ");
    Serial.println(client.state());
    delay(2000);
  }
}

// FUNÇÃO DE ESTRUTURA DO PAYLOAD NO FORMATO JSON
void data_atributos()
{
  int estadoBtn = digitalRead(btnPin); // Lê botão
  int a = random(0,1);       
  int b = random(40,60);      
  float c = 1.234;  
  String d = "OK";  

  String TESTE1 = String(a);
  String TESTE2 = String(b); 
  String TESTE3 = String(c);
  String TESTE4 = String(d);
  String BOTAO = (estadoBtn == LOW) ? "1" : "0"; // 1=pressionado, 0=solto

  String payload = "{";
  payload += "\"data\":{";
  payload += "\"TESTE1\":" + TESTE1 + ",";
  payload += "\"TESTE2\":" + TESTE2 + ",";
  payload += "\"TESTE3\":" + TESTE3 + ",";
  payload += "\"TESTE4\":\"" + TESTE4 + "\",";
  payload += "\"BOTAO\":" + BOTAO;  
  payload += "}}";

  Serial.println(payload);
 
  payload.toCharArray(ATRIBUTOS, 200);            

  if (millis() - tempoAnterior > INTERVALO) 
  {
    escreveDados();   
    tempoAnterior = millis(); 
  }
}

// FUNÇÃO QUE FAZ A PUBLICAÇÃO DO PAYLOAD
void escreveDados()
{ 
  if (client.connected()) 
  {    
    client.publish(TOPICO_PUBLICACAO, ATRIBUTOS);
  }
}

// FUNÇÃO PRINCIPAL DE EXECUÇÃO 
void loop() 
{
  int estadoBtn = digitalRead(btnPin);

  if (estadoBtn == LOW) {
    Serial.println("Botão pressionado!");
  } else {
    Serial.println("Botão solto.");
  }

  delay(200);

  if (!client.connected()) 
  {
    Serial.println("Reconectando a plataforma Wegnology");
    conexao_mqtt();
  }

  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Reconectando ao wifi");
    conexao_wifi();
  }
  
  client.loop();
  Serial.println("-------------");
  data_atributos();
  delay(1000);
}
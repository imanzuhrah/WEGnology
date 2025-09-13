/*
 * CT ELETROELETRONICA                     
 * SENAI CFP 7.91 BOTUCATU - SP
 * APLICAÇÃO: Criando dashboards com variaveis discretas e analogicas
 * DESCRIÇÃO: Estudo do MQTT e Plataforma Wegnology
 * DESENVOLVIDO POR: PROF.LUIZ HENRIQUE RAMOS 
*/

//INCLUSÃO DE BIBLIOTECAS
#include <ArduinoJson.h>
#include <WiFiS3.h>
#include <PubSubClient.h>


//DEFINIÇÃO DAS CONFIGURAÇÕES DE COMUNICAÇÃO
//CONEXÃO COM INTERNET 
const char* REDE_WIFI = "Henrique 2.4";  //REDE
const char* SENHA_WIFI =  "1497375807";      //SENHA 

//CONEXÃO COM O BROKER MQTT E PLATAFORMA WEGNOLOGY
const char* BROKER = "broker.app.wnology.io"; //BROKER MQTT 
const char* ACCESS_KEY = "ACCESS KEY";  //ACCESS KEY GERADA NA PLATAFORMA WEGNOLOGY
const char* ACCESS_SECRET = "ACCESS SECRET"; //ACCESS SECRET GERADA NA PLATAFORMA WEGNOLOGY
const char* ID_DISPOSITIVO = "DEVICE ID"; //ID DO DISPOSITIVO GERADO NA PLATAFORMA
const char* TOPICO_PUBLICACAO = "wnology/DEVICE ID/state"; //FORMATO PADRÃO DE TÓPICO PARA PLATAFROMA WEGNOLOGY wnology/DEVICE_ID/state
const int PORTA_MQTT = 1883;  //PORTA DE COMUNICAÇÃO COM O SERVIDOR
const int INTERVALO = 30000; //INTERVALO DE ENVIO DAS MENSAGENS PARA A PLATAFORMA

WiFiClient cursoIoT;
PubSubClient client(cursoIoT);

//VARIÁVEIS 
long tempoAnterior; // Armazena o tempo da última publicação 
char ATRIBUTOS [200]; // Array para armazenar o payload JSON
int x=0; // Variável auxiliar (não utilizada no código)

//FUNÇÃO DE INICIALIZAÇÃO
void setup() 
{
  Serial.begin(115200); //INICIALIZAÇÃO MONITOR SERIAL
  
  conexao_wifi(); // Chama a função para conectar WiFi
  conexao_broker(); // Chama a função para conectar ao broker MQTT
}

//FUNÇÃO DE CONEXÃO COM O WIFI
void conexao_wifi()
{
  WiFi.begin(REDE_WIFI, SENHA_WIFI);    //INICIALIZAÇÃO CONEXÃO COM WIFI
  while (WiFi.status() != WL_CONNECTED) //AGUARDANDO CONEXÃO COM WIFI
  {
    delay(500);
    Serial.println("Conectando ao Wi-Fi..."); 
  }
  Serial.println("Conectado à rede Wi-Fi"); //CONFIRMAÇÃO DE CONEXÃO COM WIFI
}

//FUNÇÃO DE CONEXÃO COM O BROKER MQTT
void conexao_broker()
{
  client.setServer(BROKER, PORTA_MQTT); //INICIALIZAÇÃO CONEXÃO COM O SERVIDOR MQTT
  while (!client.connected()) //AGUARDANDO CONEXÃO COM O SERVIDOR MQTT
  {
    Serial.println("Conectando ao servidor MQTT...");
    conexao_mqtt();    
  }
}

//FUNÇÃO DE CONEXÃO COM A PLATAFORMA WEGNOLOGY
void conexao_mqtt()
{
  if (client.connect(ID_DISPOSITIVO, ACCESS_KEY, ACCESS_SECRET)) //CONFIRMAÇÃO 
  {
    Serial.println("Conectado à plataforma WEGNOLOGY");
  } 
  else 
  {
    Serial.print("Falha na conexão à plataforma WEGNOLOGY");
    Serial.print(client.state());
    delay(2000);
  }
}

//FUNÇÃO DE ESTRUTURA DO PAYLOAD NO FORMATO JSON
void data_atributos()
{
  int a = 0;        //TESTE DE VALOR BOOLEANO (1=TRUE 0=FALSE)   
  int b = 5;        //TESTE DE VALOR INTEIRO      
  float c = 1.234;  //TESTE DE VALOR REAL
  String d = "OK";  //TESTE DE STRING (TEXTO)
             
  //CONVERTE OS VALORES SALVOS NAS VARIÁVEIS EM STRING
  String TESTE1 = String(a);
  String TESTE2 = String(b); 
  String TESTE3 = String(c);
  String TESTE4 = String(d);
 
  //CRIA PAYLOAD JSON PARA ENVIAR AO SERVIDOR MQTT
  String payload = "{";      //início da mensagem
  payload += "\"data\":";    //"data" local para armazenar os dados na plataforma wegnology
  payload += "{";            //início da lista de itens a serem armazenados no local "data"
  payload += "\"TESTE1\":";  //Primeiro atributo "TESTE1":                       
  payload += TESTE1;         //Variável que armazena o valor que será atribuído ao atributo
  payload += ",";            //separação de atributos
  payload += "\"TESTE2\":";  //Segundo atributo "TESTE2":
  payload += TESTE2;         //Variável que armazena o valor que será atribuído ao atributo
  payload += ",";            //separação de atributos
  payload += "\"TESTE3\":";  //Terceiro atributo "TESTE3":
  payload += TESTE3;         //Variável que armazena o valor que será atribuído ao atributo
  payload += ",";            //separação de atributos
  payload += "\"TESTE4\":";  //Quarto atributo "TESTE4": 
  payload += "\"";           //Envio do caracter " (aspas) para iniciar o envio de uma string
  payload += TESTE4;         //Variável que armazena o valor que será atribuído ao atributo  
  payload += "\"";           //Envio do caracter " (aspas) para finalizar o envio de uma string
  payload += "}";            //Finalização dos itens dentro do local "data"
  payload += "}";            //Finalização do payload

  /*FORMATO DA MENSAGEM JSON
  {
    data:
    {
      "TESTE1" : 1
      "TESTE2" : 5
      "TESTE2" : 1.234
      "TESTE2" : "OK"
    }
  }
 */

  Serial.println(payload); //ESCRITA DO PAYLOAD NO MONITOR SERIAL
 
  //CRIA UMA ARRAY DE CARACTERES E PUBLICA EM PARES DE "NOME" : VALOR NO SERVIDOR
  payload.toCharArray(ATRIBUTOS, 200); //CARREGA O PAYLOAD NA ARRAY ATRIBUTOS            
  if (millis() - tempoAnterior > INTERVALO) 
  {
    escreveDados(); //CHAMA A FUNÇÃO PARA FAZER A PUBLICAÇÃO DOS DADOS   
    tempoAnterior = millis(); //AGUARDA O INTERVALO DE ENVIO DE MENSAGENS
  }
}
//FUNÇÃO QUE FAZ A PUBLICAÇÃO DO PAYLOAD
void escreveDados()
{ 
  if (client.connect(ID_DISPOSITIVO, ACCESS_KEY, ACCESS_SECRET)) //VERIFICA A CONEXÃO COM A PLATAFORMA
  {    
    client.publish(TOPICO_PUBLICACAO,ATRIBUTOS);//ENVIA O PAYLOAD NO TÓPICO DE PUBLICAÇÃO ("wnology/DEVICE_ID/state",payload)
  }
}

//FUNÇÃO PRINCIPAL DE EXECUÇÃO 
void loop() 
{
  if (!client.connected()) //VERIFICA FALHA NA CONEXAO COM A PLATAFORMA
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
  delay(10000);
}

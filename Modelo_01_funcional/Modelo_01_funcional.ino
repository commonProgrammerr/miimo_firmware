#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiManager.h>

#define HEAD "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>\r\n<title>WebServer ESP8266</title><style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} input{width:80%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;}</style></head><h3> ESP8266 Web Server</h3>"
#define FOOTER "<div><a href=\"?function=broadCastId\"><button>Publicar ID</button></a></div><div><a href=\"?function=reset_wifi\"><button>Resetar WiFi</button></a></div><h4>Criado por Pablo Rodrigol</h4></html>\n"
//Variáveis
long lastMsg = 0;
int cont = 0;
bool is_conected = false;
WiFiManager wifiManager;
WiFiServer server(80);
uint8_t gpioStatus = LOW; //Status Inicial da porta
uint8_t GPIO_PORT = 1;    //Porta do ESP

//Parâmetros de configuração Default//
const char *SSID = "Rede-Wifi";     //Seu SSID da Rede WIFI
const char *PASSWORD = "senha123";  // A Senha da Rede WIFI
String clientName = "Dispositivo_"; //Prefixo do nome do dispositivo ESP

//Trata endereçoo MAC
String macToStr(const uint8_t *mac)
{
  String result;
  for (int i = 0; i < 6; ++i)
  {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

//conecta a wifi -- PASSO 03
void setupWifiServer()
{

  //Trata endereço MAC
  uint8_t mac[6];
  WiFi.macAddress(mac);
  clientName += macToStr(mac);
  clientName.replace(":", "");
  Serial.println("\nDispositivo: " + clientName + "\n");
  wifiManager.setConfigPortalTimeout(180);
  wifiManager.autoConnect(clientName.c_str(), "1234567890");

  //Log na serial se conectar;
  Serial.println("\nWiFi conectado!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

//Verifica conexão com WIFI -- PASSO 02
void setupWIFI(void)
{
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Conectando na rede: ");
  Serial.println(SSID);

  //Tenta conectar por 10 vezes e aguarda por MEIO segundo
  while ((WiFi.status() != WL_CONNECTED) and (cont < 10))
  {
    Serial.print(".");
    Serial.println(cont);
    delay(500);
    cont = cont + 1;
  }
  is_conected = (WiFi.status() == WL_CONNECTED);
  setupWifiServer();
}

//Resset configurações de WIFI
void reset_config(void)
{
  //Reset das definicoes de rede
  wifiManager.resetSettings();
  delay(1500);
  ESP.reset();
}

//PRINCIPAL -- PASSO 01
void setup(void)
{
  Serial.begin(9600);
  setupWIFI();
  // setupMqtt();
}

void loop(void)
{

  //Aguarda uma nova conexao
  WiFiClient client = server.available();
  if (!client)
  {
    Serial.println("Nova conexao requisitada...");
    while (!client.available())
    {
      delay(100);
    }
    Serial.println("Nova conexao OK...");
    //Le a string enviada pelo cliente
    String req = client.readStringUntil('\r');
    //Mostra a string enviada
    Serial.println(req);
    //Limpa dados/buffer
    client.flush();
    //Trata a string do cliente em busca de comandos
    if (req.indexOf("rele_on") != -1)
    {
      digitalWrite(GPIO_PORT, HIGH);
      gpioStatus = HIGH;
    }
    else if (req.indexOf("rele_off") != -1)
    {
      digitalWrite(GPIO_PORT, LOW);
      gpioStatus = LOW;
    }
    else if (req.indexOf("reset_wifi") != -1)
    {
      reset_config();
    }
    else
    {
      Serial.println("Requisicao invalida");
    }

    //Prepara a resposta para o cliente
    String buf = HEAD;

    //De acordo com o status da GPIO envia o comando
    if (gpioStatus)
      buf += "<div><h4>Lampada</h4><a href=\"?function=rele_off\"><button>Desligar</button></a></div>";
    else
      buf += "<div><h4>Lampada</h4><a href=\"?function=rele_on\"><button>Ligar</button></a></div>";

    buf += FOOTER;

    //Envia a resposta para o cliente
    client.print(buf);
    client.flush();
    client.stop();

    Serial.println("Cliente desconectado!");
  }

  if (is_conected)
  {
    long now = millis();
    // A cada 30 segundos ele PUBLICA para o Broker MQTT(mosquitto.org)
    if (now - lastMsg > 30000)
    {
      lastMsg = now;
      Serial.print("30 seg");
      //setSensoresMqtt();
    }
  }
}

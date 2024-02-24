#include <Arduino.h>
#include "FS.h"
#include <WiFi.h>
#include <DNSServer.h>
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

DNSServer dnsServer;
AsyncWebServer server(80);
String ssidWifi = "esptest";

float r1 = 1000000.0;
float r2 = 100000.0;
double vout = 0.0, res = 0.0, resr = 0.0, rout = 0.0, v = 0.0;
float const corrientePin = 0.012;

void iniciarServer(void);

void setup()
{
  Serial.begin(115200);

  if (!SPIFFS.begin(true))
  { // inicializo el spiffs - donde se almacena la pagina web
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  WiFi.mode(WIFI_AP); // inicio el wifi como AP

  WiFi.softAP(ssidWifi.c_str()); // seteo el nombre de la red wifi, sin password

  Serial.println(WiFi.softAPIP()); // imprimo ip del host

  iniciarServer(); // inicializo el server http -(pagina web ddns)

  dnsServer.start(53, "TesterDigital.com", WiFi.softAPIP()); // inicio el server dns para el captive portal

  pinMode(32, OUTPUT);
}
void loop()
{
  for(int x = 0; x<=500; x++){
    v = (analogRead(A0) * 3.3)/ 4095.0;
    res = (v/(r1/(r1+r2)));
    vout = vout+res;
  }
  res = vout/500;
  vout = 0;
  for(int x = 0; x<=500; x++){
    double i = (analogRead(A3) * 3.3)/ 4095.0;
    resr = (i/(r1/(r1+r2)));
    rout = rout+resr;
  }
  resr = rout/500;
  digitalWrite(32, HIGH);
  resr = resr/corrientePin;
  rout = 0;

  delay(300);
}
void iniciarServer(void)
{
  /*********************Captive Portal************************************/
  server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", String(), false, NULL); });

  server.onNotFound([](AsyncWebServerRequest *request)
                    { request->send(SPIFFS, "/index.html", String(), false, NULL); });

  server.on("/fwlink", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", String(), false, NULL); });
  /*********************Captive Portal************************************/

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", String(), false, NULL); });
            
  server.on("/estilos.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/estilos.css", "text/css"); });
  
  server.on("/jquery.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/jquery.min.js", "text/javascript"); });
            
  server.on("/calib.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/calib.js", "text/javascript"); });

  server.on("/escudo.jpg", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/escudo.jpeg", "img/jpeg"); });

  server.on("/recibir_datos", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    String requestBody = "";  // almacenar el cuerpo de la solicitud            
    // leer el cuerpo de la solicitud
    if (request->hasParam("plain", true)) {
        requestBody = request->getParam("plain", true)->value();
        Serial.println("Cuerpo de la solicitud POST:");
        Serial.println(requestBody);
        request->send(200, "text/plain", "Solicitud POST recibida con éxito.");
    } else {
        Serial.println("No se encontró un cuerpo en la solicitud POST.");
        request->send(400, "text/plain", "Cuerpo de la solicitud POST faltante.");
    } });

  server.on("/obtenerMedicion", HTTP_GET, [](AsyncWebServerRequest *request) { // cuando hay una solicitud de medicion lo devuelvo
    request->send(200, "text/plain", String(res) + String("&") + String(resr));
    //request->send(200, "text/plain", String(random(10, 99)) + String(" ") + String(random(0, 3)) + String(" ") + String(152));

  });
  
  server.begin();
}
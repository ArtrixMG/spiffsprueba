#include <Arduino.h>
#include "FS.h"
#include <WiFi.h>
#include <DNSServer.h>
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

DNSServer dnsServer;
AsyncWebServer server(80);
String ssidWifi = "TesterWifi_IDO";

float r1 = 1000000.0;
float r2 = 100000.0;
double resv = 0.0, vout = 0.0, resr = 0.0, rout = 0.0, resi = 0.0, iout = 0.0,  v = 0.0, sens = 100;

void iniciarServer(void);

void setup()
{
  Serial.begin(115200);

  if (!SPIFFS.begin(true))
  { // Inicializo el spiffs - donde se almacena la pagina web.
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  WiFi.mode(WIFI_AP); // Inicio el wifi como AP.

  WiFi.softAP(ssidWifi.c_str()); // Seteo el nombre de la red wifi, sin password.

  Serial.println(WiFi.softAPIP()); // Imprimo ip del host.

  iniciarServer(); // Inicializo el server http -(pagina web ddns).

  dnsServer.start(53, "TesterDigital.com", WiFi.softAPIP()); // Inicio el server dns para el captive portal.

  pinMode(32, OUTPUT);
}
void loop()
{
  for(int x = 0; x<=500; x++){
    v = (analogRead(A0) * 3.3)/ 4095.0;
    resv = (v/(r1/(r1+r2)));
    vout = vout+resv;
  } // Promedio de tension.
  resv = vout/500;
  vout = 0;

  for(int x = 0; x<=500; x++){
    v = (analogRead(A3) * 3.3)/ 4095.0;
    resi = (v-2.5)/sens;// Formula del sensor ACS712.
    iout = iout+resi;
  }// Promedio de tension.
  resi = iout/500;

  digitalWrite(32, HIGH);  // Pin de pulso del modo resistencia.
  for(int x = 0; x<=500; x++){
    v = (analogRead(A3) * 3.3)/ 4095.0;
    resr = (v-2.5)/sens;// Formula del sensor ACS712.
    rout = rout+resr;
  }// Promedio de tension.
  resr = rout/500;
  resr = 3.3/resr;
  rout = 0;
  digitalWrite(32, LOW); // Pin de pulso del modo resistencia.
  
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
            { request->send(SPIFFS, "/escudo.png", "img/png"); }); // Se llama al ESP para que lea los datos de todos los archivos del directorio /data.

  server.on("/recibir_datos", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    String requestBody = "";  // Almacenar el cuerpo de la solicitud.
    // Leer el cuerpo de la solicitud.
    if (request->hasParam("plain", true)) {
        requestBody = request->getParam("plain", true)->value();
        Serial.println("Cuerpo de la solicitud POST:");
        Serial.println(requestBody);
        request->send(200, "text/plain", "Solicitud POST recibida con éxito.");
    } else {
        Serial.println("No se encontró un cuerpo en la solicitud POST.");
        request->send(400, "text/plain", "Cuerpo de la solicitud POST faltante.");
    } });

  server.on("/obtenerMedicion", HTTP_GET, [](AsyncWebServerRequest *request) { // Cuando hay una solicitud de medicion lo devuelvo.
    request->send(200, "text/plain", String(resv) + String("&") + String(resr) + String("&") + String(resi)); // Datos que se reciben en /calib.js en la funcion IngresarDatos().
  });
  
  server.begin();
}
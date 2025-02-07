#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "config.h"

#define ledPin D5
#define statusLed D1

int mode = 1;
int brightness = 0;
int step = 5;  // Tamaño del incremento

const char* ssid = WIFI_SSID;      // Nombre de la red WiFi
const char* password = WIFI_PASS;  // Contraseña de la red WiFi

//En lo siguiente reemplaza puntos por comas
IPAddress local_IP(192,168,0,222);  // Cambia a una IP dentro del rango de tu red
IPAddress gateway(192, 168, 0, 1);     // La puerta de enlace suele ser la dirección del router
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(80);

void manageModes();
void fade();

const char* htmlContent = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Control del LED</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-QWTKZyjpPEjISv5WaRU9OFeRpok6YctnYmDr5pNlyT2bRjXh0JMhjY6hW+ALEwIH" crossorigin="anonymous">
  </head>
<body>
  <h1>Control del LED</h1>
  <p><a href="/ON" ><button class="btn btn-primary" type="button">Encender</button></a></p>
  <p><a href="/OFF"><button class="btn btn-primary" type="button">Apagar</button></a></p>
  <p><a href="/FADE"><button class="btn btn-primary" type="button">Fade</button></a></p>
  <p><a href="/INCREMENT" ><button class="btn btn-primary" type="button">Subir Velocidad</button></a></p>
  <p><a href="/DECREMENT" ><button class="btn btn-primary" type="button">Bajar Velocidad</button></a></p>
  <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/js/bootstrap.bundle.min.js" integrity="sha384-YvpcrYf0tY3lHB60NNkmXc5s9fDVZLESaAA55NDzOxhy9GkcIdslK1eN7N6jIeHz" crossorigin="anonymous"></script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(9600);
  delay(10);

  pinMode(ledPin, OUTPUT);
  pinMode(statusLed, OUTPUT);
  digitalWrite(ledPin, LOW);  // LED apagado al inicio

  // Configurar IP estática
  if (!WiFi.config(local_IP, gateway, subnet))
  {
    Serial.println("Error al configurar la IP estática");
  }

  // Conectar al WiFi
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
      if (millis() - startTime > 15000) {  // 15 segundos de espera
          Serial.println("No se pudo conectar al WiFi. Reiniciando...");
          ESP.restart();
      }
      digitalWrite(statusLed, LOW);
      delay(200);
      digitalWrite(statusLed, HIGH);
      delay(200);
      Serial.println("Estado de conexión: " + String(WiFi.status()));
  }


  Serial.println();
  Serial.println("Conectado al WiFi");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    while (!client.available()) {
      delay(1);
    }

    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

    if (request.indexOf("/OFF") != -1) {
      mode = 0;
    } else if (request.indexOf("/ON") != -1) {
      mode = 1;
    } else if (request.indexOf("/FADE") != -1) {
      mode = 2;
    } else if (request.indexOf("/INCREMENT") != -1){
      step++;
      Serial.println(step);
    } else if (request.indexOf("/DECREMENT") != -1){
      step--;
      Serial.println(step);
    }

    client.println(htmlContent);
  }

  manageModes();
}

void manageModes(){
  if (mode == 0) {
    analogWrite(ledPin, 0);
  } else if (mode == 1) {
    analogWrite(ledPin, 255);
  } else if (mode == 2) {
    fade();  // Ahora se llama continuamente en loop()
  }
}

void fade() {
  analogWrite(ledPin, brightness);
  brightness += step;

  if (brightness >= 255 || brightness <= 0) {
    step = -step;  // Invierte la dirección
  }

  delay(30);  // Controla la velocidad del fade
}

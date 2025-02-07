// Resistencias
// Rc = 220ohm
// Rb = 15kohm
// Transistores BC550B

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "config.h"
#include "rgbModes.h"

#define GREEN_LED D5
#define RED_LED D4
#define BLUE_LED D3
#define statusLed D1

void manageModes();
void fade();
void manageStep(int side);

int mode = FADE;

// Brillo de cada LED
int brightnessGreen = 0;
int brightnessBlue = 0;
int brightnessRed = 0;

int step = 2;  // Tamaño del incremento
int stepIncrement = 2;

int enable = 1; // 0 indica todo apagado, 1 indica que se habilitan los modos

const char* ssid = WIFI_SSID;      // Nombre de la red WiFi
const char* password = WIFI_PASS;  // Contraseña de la red WiFi

//En lo siguiente reemplaza puntos por comas
IPAddress local_IP(192, 168, 0, 222);  // Cambia a una IP dentro del rango de tu red
IPAddress gateway(192, 168, 0, 1);     // La puerta de enlace suele ser la dirección del router
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(80);

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

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(statusLed, OUTPUT);

  // Configurar IP estática
  if (!WiFi.config(local_IP, gateway, subnet)) {
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
      enable = 0;
    } else if (request.indexOf("/ON") != -1) {
      enable = 1;
    } else if (request.indexOf("/FADE") != -1) {
      if (enable) mode = FADE;
    } else if (request.indexOf("/INCREMENT") != -1) {
      manageStep(1);
      //Serial.println(step);
    } else if (request.indexOf("/DECREMENT") != -1) {
      manageStep(-1);
      //Serial.println(step);
    }

    client.println(htmlContent);
  }

  if (enable){
    manageModes();
  } else {
    brightnessGreen = 0;
    brightnessRed = 0;
    brightnessBlue = 0;
  }
    
  analogWrite(GREEN_LED, brightnessGreen);
  analogWrite(RED_LED, brightnessRed);
  analogWrite(BLUE_LED, brightnessBlue);
}

void manageModes() {
  if (mode == FADE) {
    fade();
  }
}

void fade() {
  brightnessRed += step;
  brightnessGreen += step;
  brightnessBlue += step;

  if (brightnessRed >= 255 || brightnessRed <= 0) {
    step = -step;  // Invierte la dirección
  }

  delay(30);  // Controla la velocidad del fade
}

void manageStep(int side){ // Si side == 1 incrementa, si es -1 decrementa
  if (step > 0 && step + side * stepIncrement != 0){
    step += side * stepIncrement;
  } else if (step < 0 && step - side * stepIncrement != 0){
    step -= side * stepIncrement;
  }
}

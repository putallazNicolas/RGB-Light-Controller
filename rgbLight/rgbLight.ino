// Resistencias
// Rc = 220ohm
// Rb = 15kohm
// Transistores BC550B

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "config.h"
#include "rgbModes.h"   // Se asume que define las constantes: WHITE, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, ORANGE, PINK, VIOLET, BROWN, GREY, OLIVE_GREEN, MARINE_BLUE, LIGHT_BLUE, GOLD, FADE
#include "colors.h"     // Header file con las funciones: white(), red(), green(), blue(), yellow(), cyan(), magenta(), orange(), pink(), violet(), brown(), gray(), olive(), navy(), turquoise(), gold()

// Pines de salida
#define GREEN_LED D5
#define RED_LED   D4
#define BLUE_LED  D3
#define statusLed D1

// Declaración de funciones
void manageModes();
void fade();
void manageStep(int side);
void manageRequests(WiFiClient client);
void controlColors();
void controlModeColor(int color);
void strobe();
void controlMaxBrightness(int side);

// Modo de funcionamiento (por defecto FADE)
int mode = WHITE;  

// Variables para el brillo de cada LED (valor entre 0 y 255)
int brightnessGreen = 0;
int brightnessBlue  = 0;
int brightnessRed   = 0;
int maxBrightness   = 255;
int maxUserBrightness = 255;

// **Multiplicadores para corregir el brillo de cada canal**
// Se pueden modificar desde el HTML
float redAdj   = 1.0;
float greenAdj = 0.5;
float blueAdj  = 1.0;

int step = 2;           // Tamaño del incremento para fade
int stepIncrement = 1;
const int maxStep = 100;
unsigned long previousTime;
bool isFading = true;
int fadeColor = WHITE;

int strobeColor = WHITE;

// Variable para habilitar o deshabilitar el modo
int enable = 1; // 0: todo apagado, 1: habilitar modos

// Datos de la red WiFi
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

// Configuración IP estática (reemplaza puntos por comas)
IPAddress local_IP(192, 168, 0, 222);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(80);

// HTML con los botones y formulario para editar multiplicadores
const char* htmlContent = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Control del LED</title>
  <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css" rel="stylesheet">
</head>
<body>
  <h1>Control del LED</h1>
  <p><a href="/ON"><button class="btn btn-primary" type="button">Encender</button></a></p>
  <p><a href="/OFF"><button class="btn btn-primary" type="button">Apagar</button></a></p>
  <p><a href="/FADE"><button class="btn btn-primary" type="button">Fade</button></a></p>
  <p><a href="/STROBE"><button class="btn btn-primary" type="button">Strobe</button></a></p>
  <p><a href="/INCREMENT"><button class="btn btn-primary" type="button">Subir Velocidad</button></a></p>
  <p><a href="/DECREMENT"><button class="btn btn-primary" type="button">Bajar Velocidad</button></a></p>
  <p><a href="/LIGHTER"><button class="btn btn-primary" type="button">Subir Brillo</button></a></p>
  <p><a href="/DARKER"><button class="btn btn-primary" type="button">Bajar Brillo</button></a></p>
  <hr>
  <p><a href="/RED"><button class="btn btn-danger" type="button">RED</button></a></p>
  <p><a href="/GREEN"><button class="btn btn-success" type="button">GREEN</button></a></p>
  <p><a href="/BLUE"><button class="btn btn-primary" type="button">BLUE</button></a></p>
  <p><a href="/WHITE"><button class="btn btn-light" type="button">WHITE</button></a></p>
  <p><a href="/YELLOW"><button class="btn btn-warning" type="button">YELLOW</button></a></p>
  <p><a href="/CYAN"><button class="btn btn-info" type="button">CYAN</button></a></p>
  <p><a href="/MAGENTA"><button class="btn" style="background-color: magenta; border-color: magenta; color: white;" type="button">MAGENTA</button></a></p>
  <p><a href="/ORANGE"><button class="btn" style="background-color: orange; border-color: orange; color: white;" type="button">ORANGE</button></a></p>
  <p><a href="/PINK"><button class="btn" style="background-color: pink; border-color: pink; color: black;" type="button">PINK</button></a></p>
  <p><a href="/VIOLET"><button class="btn" style="background-color: violet; border-color: violet; color: white;" type="button">VIOLET</button></a></p>
  <p><a href="/BROWN"><button class="btn" style="background-color: brown; border-color: brown; color: white;" type="button">BROWN</button></a></p>
  <p><a href="/GREY"><button class="btn btn-secondary" type="button">GREY</button></a></p>
  <p><a href="/OLIVE_GREEN"><button class="btn" style="background-color: olive; border-color: olive; color: white;" type="button">OLIVE GREEN</button></a></p>
  <p><a href="/MARINE_BLUE"><button class="btn" style="background-color: navy; border-color: navy; color: white;" type="button">MARINE BLUE</button></a></p>
  <p><a href="/LIGHT_BLUE"><button class="btn" style="background-color: lightblue; border-color: lightblue; color: black;" type="button">LIGHT BLUE</button></a></p>
  <p><a href="/GOLD"><button class="btn" style="background-color: gold; border-color: gold; color: black;" type="button">GOLD</button></a></p>
  <hr>
  <h2>Ajuste de multiplicadores</h2>
  <form action="/setAdj">
    <div class="mb-3">
      <label for="red" class="form-label">Red Multiplier:</label>
      <input type="number" step="0.01" id="red" name="red" class="form-control" value="1.00">
    </div>
    <div class="mb-3">
      <label for="green" class="form-label">Green Multiplier:</label>
      <input type="number" step="0.01" id="green" name="green" class="form-control" value="0.75">
    </div>
    <div class="mb-3">
      <label for="blue" class="form-label">Blue Multiplier:</label>
      <input type="number" step="0.01" id="blue" name="blue" class="form-control" value="1.00">
    </div>
    <button type="submit" class="btn btn-secondary">Actualizar Multiplicadores</button>
  </form>
  <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/js/bootstrap.bundle.min.js"></script>
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

  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Error al configurar la IP estática");
  }

  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - startTime > 15000) {
      Serial.println("No se pudo conectar al WiFi. Reiniciando...");
      ESP.restart();
    }
    digitalWrite(statusLed, LOW);
    delay(200);
    digitalWrite(statusLed, HIGH);
    delay(200);
    Serial.println("Estado de conexión: " + String(WiFi.status()));
    pink();
  }

  Serial.println();
  Serial.println("Conectado al WiFi");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
  server.begin();

  previousTime = millis();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    manageRequests(client);
  }

  if (enable) {
    manageModes();
  } else {
    brightnessGreen = 0;
    brightnessRed = 0;
    brightnessBlue = 0;
  }
    
  controlColors();
}

void manageRequests(WiFiClient client) {
  while (!client.available()) {
    delay(1);
  }
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Se revisan las rutas para los modos
  if (request.indexOf("/OFF") != -1) {
    enable = 0;
  } else if (request.indexOf("/ON") != -1) {
    enable = 1;
  }

  if (enable){
    if (request.indexOf("/FADE") != -1) {
      isFading = !isFading;
      maxBrightness = maxUserBrightness;
    } else if (request.indexOf("/STROBE") != -1) {
      mode = STROBE;
    } else if (request.indexOf("/RED") != -1) {
      mode = RED;
    } else if (request.indexOf("/GREEN") != -1) {
      mode = GREEN;
    } else if (request.indexOf("/BLUE") != -1) {
      mode = BLUE;
    } else if (request.indexOf("/WHITE") != -1) {
      mode = WHITE;
    } else if (request.indexOf("/YELLOW") != -1) {
      mode = YELLOW;
    } else if (request.indexOf("/CYAN") != -1) {
      mode = CYAN;
    } else if (request.indexOf("/MAGENTA") != -1) {
      mode = MAGENTA;
    } else if (request.indexOf("/ORANGE") != -1) {
      mode = ORANGE;
    } else if (request.indexOf("/PINK") != -1) {
      mode = PINK;
    } else if (request.indexOf("/VIOLET") != -1) {
      mode = VIOLET;
    } else if (request.indexOf("/BROWN") != -1) {
      mode = BROWN;
    } else if (request.indexOf("/GREY") != -1) {
      mode = GRAY;
    } else if (request.indexOf("/OLIVE_GREEN") != -1) {
      mode = OLIVE_GREEN;
    } else if (request.indexOf("/MARINE_BLUE") != -1) {
      mode = NAVY_BLUE;
    } else if (request.indexOf("/LIGHT_BLUE") != -1) {
      mode = LIGHT_BLUE;
    } else if (request.indexOf("/GOLD") != -1) {
      mode = GOLD;
    } else if (request.indexOf("/INCREMENT") != -1) {
      manageStep(1);
    } else if (request.indexOf("/DECREMENT") != -1) {
      manageStep(-1);
    } else if (request.indexOf("/LIGHTER") != -1) {
      controlMaxBrightness(1);
    } else if (request.indexOf("/DARKER") != -1) {
      controlMaxBrightness(-1);
    } else if (request.indexOf("/setAdj") != -1) {
      // Se espera recibir parámetros en formato:
      // /setAdj?red=1.00&green=0.75&blue=1.00
      int redIndex = request.indexOf("red=");
      int greenIndex = request.indexOf("green=");
      int blueIndex = request.indexOf("blue=");
      
      if(redIndex != -1) {
        int redEnd = request.indexOf('&', redIndex);
        if(redEnd == -1) redEnd = request.indexOf(' ', redIndex);
        String redValue = request.substring(redIndex + 4, redEnd);
        redAdj = redValue.toFloat();
      }
      if(greenIndex != -1) {
        int greenEnd = request.indexOf('&', greenIndex);
        if(greenEnd == -1) greenEnd = request.indexOf(' ', greenIndex);
        String greenValue = request.substring(greenIndex + 6, greenEnd);
        greenAdj = greenValue.toFloat();
      }
      if(blueIndex != -1) {
        int blueEnd = request.indexOf('&', blueIndex);
        if(blueEnd == -1) blueEnd = request.indexOf(' ', blueIndex);
        String blueValue = request.substring(blueIndex + 5, blueEnd);
        blueAdj = blueValue.toFloat();
      }
    }
  }

  client.println(htmlContent);
}

void manageModes() {
  if (mode == STROBE) {
    isFading = false;
    strobe();
    return;
  }

  if (isFading) {
    fadeColor = mode;
    fade();
    return;
  }

  switch (mode) {
    case RED: red(); break;
    case GREEN: green(); break;
    case BLUE: blue(); break;
    case WHITE: white(); break;
    case YELLOW: yellow(); break;
    case CYAN: cyan(); break;
    case MAGENTA: magenta(); break;
    case ORANGE: orange(); break;
    case PINK: pink(); break;
    case VIOLET: violet(); break;
    case BROWN: brown(); break;
    case GRAY: gray(); break;
    case OLIVE_GREEN: olive(); break;
    case NAVY_BLUE: navy(); break;
    case LIGHT_BLUE: turquoise(); break;
    case GOLD: gold(); break;
    default: break;  // Modo desconocido
  }
}


void manageStep(int side) { // side = 1 para incrementar, -1 para decrementar
  if (step > 1 && step + side * stepIncrement != 0) {
    step += side * stepIncrement;
  } else if (step < 1 && step - side * stepIncrement != 0) {
    step -= side * stepIncrement;
  }
  if (step > maxStep) {
    step = maxStep;
  } else if (step < -maxStep) {
    step = -maxStep;
  }
}

// Función que aplica los multiplicadores a los valores de brillo y actualiza los pines.
// Se limita el valor máximo a 255.
void controlColors() {
  int adjRed = brightnessRed * redAdj;
  if (adjRed > 255) adjRed = 255;
  int adjGreen = brightnessGreen * greenAdj;
  if (adjGreen > 255) adjGreen = 255;
  int adjBlue = brightnessBlue * blueAdj;
  if (adjBlue > 255) adjBlue = 255;

  analogWrite(RED_LED, adjRed);
  analogWrite(GREEN_LED, adjGreen);
  analogWrite(BLUE_LED, adjBlue);
}

void fade() {
  maxBrightness += step;

  // Verificar límites
  if (maxBrightness >= maxUserBrightness) {
    maxBrightness = maxUserBrightness;
    step *= -1;  // Invertir dirección para comenzar a disminuir
  } else if (maxBrightness <= 0) {
    maxBrightness = 0;
    step *= -1;  // Invertir dirección para comenzar a aumentar
  }

  controlModeColor(fadeColor);
  
  delay(10);
}


void strobe(){
  int actualTime = millis();
  int elapsedTime = actualTime - previousTime;

  if (elapsedTime >= 1000 / step){
    previousTime = actualTime;
    strobeColor++;
    if (strobeColor > GOLD){
      strobeColor = WHITE;
    }
    controlModeColor(strobeColor);
  }
}

void controlModeColor(int color){
  switch (color){
    case RED:
      red();
      break;
    case GREEN:
      green();
      break;
    case BLUE:
      blue();
      break;
    case WHITE:
      white();
      break;
    case YELLOW:
      yellow();
      break;
    case CYAN:
      cyan();
      break;
    case MAGENTA:
      magenta();
      break;
    case ORANGE:
      orange();
      break;
    case PINK:
      pink();
      break;
    case VIOLET:
      violet();
      break;
    case BROWN:
      brown();
      break;
    case GRAY:
      gray();
      break;
    case OLIVE_GREEN:
      olive();
      break;
    case NAVY_BLUE:
      navy();
      break;
    case LIGHT_BLUE:
      turquoise();
      break;
    case GOLD:
      gold();
      break;
  }
}

void controlMaxBrightness(int side){
  maxUserBrightness += side * 64;
  if (maxUserBrightness > 255){
    maxUserBrightness = 255;
  } else if (maxUserBrightness < 0){
    maxUserBrightness = 0;
  }

  if (!isFading){
    maxBrightness = maxUserBrightness;
  }
}


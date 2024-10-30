#include <ESP8266WiFi.h>

const char* ssid = "Fibertel 999-2.4GHz"; // Nombre de la red WiFi
const char* password = "01427544721"; // Contraseña de la red WiFi

//En lo siguiente reemplaza puntos por comas
IPAddress local_IP(192, 168, 0, 50); // Cambia a una IP dentro del rango de tu red
IPAddress gateway(192, 168, 0, 1);    // La puerta de enlace suele ser la dirección del router
IPAddress subnet(255, 255, 255, 0);

const int ledPin = D5; // Pin donde está conectado el LED
const int statusLed = D1;

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
  <p><a href="/LED=ON" ><button class="btn btn-primary" type="button">Encender</button></a></p>
  <p><a href="/LED=OFF"><button class="btn btn-primary" type="button">Apagar</button></a></p>
  <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/js/bootstrap.bundle.min.js" integrity="sha384-YvpcrYf0tY3lHB60NNkmXc5s9fDVZLESaAA55NDzOxhy9GkcIdslK1eN7N6jIeHz" crossorigin="anonymous"></script>
</body>
</html>
)rawliteral";

void setup() 
{
  Serial.begin(9600);
  delay(10);

  pinMode(ledPin, OUTPUT);
  pinMode(statusLed, OUTPUT);
  digitalWrite(ledPin, LOW); // LED apagado al inicio

  // Configurar IP estática
  if (!WiFi.config(local_IP, gateway, subnet)) 
  {
    Serial.println("Error al configurar la IP estática");
  }

  // Conectar al WiFi
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    digitalWrite(statusLed, LOW);
    delay(182);
    digitalWrite(statusLed, HIGH);
    delay(182);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Conectado al WiFi");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() 
{
  WiFiClient client = server.available();
  if (!client) 
  {
    return;
  }

  // Esperar a que el cliente envíe datos
  while(!client.available()) 
  {
    delay(1);
  }

  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Comprobar la solicitud
  if (request.indexOf("/LED=ON") != -1) 
  {
    digitalWrite(ledPin, HIGH); // Enciende el LED
  } 
  else if (request.indexOf("/LED=OFF") != -1) 
  {
    digitalWrite(ledPin, LOW); // Apaga el LED
  }

  // Respuesta HTML al cliente
  client.println(htmlContent);
}
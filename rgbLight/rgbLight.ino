#include <ESP8266WiFi.h>

const char* ssid = ""; // Nombre de la red WiFi
const char* password = ""; // Contraseña de la red WiFi

//En lo siguiente reemplaza puntos por comas
IPAddress local_IP(); // Cambia a una IP dentro del rango de tu red
IPAddress gateway();    // La puerta de enlace suele ser la dirección del router
IPAddress subnet();

const int ledPin = D5; // Pin donde está conectado el LED
const int statusLed = D1;

WiFiServer server(80);

const char* htmlContent = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Control del LED</title>
</head>
<body>
  <h1>Control del LED</h1>
  <p><a href="/LED=ON"><button>Encender</button></a></p>
  <p><a href="/LED=OFF"><button>Apagar</button></a></p>
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
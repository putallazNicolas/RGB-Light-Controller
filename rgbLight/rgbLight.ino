#include <ESP8266WiFi.h>

const char* ssid = ""; // Nombre de la red WiFi
const char* password = ""; // Contraseña de la red WiFi

const int ledPin = D5; // Pin donde está conectado el LED

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  delay(10);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // LED apagado al inicio

  // Conectar al WiFi
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Iniciar el servidor
  server.begin();
  Serial.println("Servidor iniciado");
}

void loop() {
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Esperar a que el cliente envíe datos
  while(!client.available()) {
    delay(1);
  }

  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Comprobar la solicitud
  if (request.indexOf("/LED=ON") != -1) {
    digitalWrite(ledPin, HIGH); // Enciende el LED
  } else if (request.indexOf("/LED=OFF") != -1) {
    digitalWrite(ledPin, LOW); // Apaga el LED
  }

  // Respuesta HTML al cliente
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<h1>Control del LED</h1>");
  client.println("<p><a href=\"/LED=ON\"><button>Encender</button></a></p>");
  client.println("<p><a href=\"/LED=OFF\"><button>Apagar</button></a></p>");
  client.println("</html>");
  delay(1);
}
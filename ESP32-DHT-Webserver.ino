#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DHT.h>

// Configurare detalii rețea WiFi
const char *ssid = "My-Network";
const char *password = "chaker123456";

// Setarea adresei IP statice
IPAddress local_IP(192.168.1.184);
IPAddress gateway(192.168.1.1);
IPAddress subnet(255.255.255.0);
IPAddress primaryDNS(8.8.8.8);  // Opțional
IPAddress secondaryDNS(8.8.4.4);  // Opțional

WebServer server(80);
DHT dht(26, DHT11); // Pinul 26 și tipul DHT11

void handleRoot() {
  char msg[1500];

  // Creare conținut HTML
  snprintf(msg, 1500,
           "<html>\
  <head>\
    <meta http-equiv='refresh' content='4'/>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css' integrity='sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr' crossorigin='anonymous'>\
    <title>ESP32 DHT Server</title>\
    <style>\
    html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center;}\
    h2 { font-size: 3.0rem; }\
    p { font-size: 3.0rem; }\
    .units { font-size: 1.2rem; }\
    .dht-labels{ font-size: 1.5rem; vertical-align:middle; padding-bottom: 15px;}\
    </style>\
  </head>\
  <body>\
      <h2>ESP32 DHT Server!</h2>\
      <p>\
        <i class='fas fa-thermometer-half' style='color:#ca3517;'></i>\
        <span class='dht-labels'>Temperature</span>\
        <span>%.2f</span>\
        <sup class='units'>&deg;C</sup>\
      </p>\
      <p>\
        <i class='fas fa-tint' style='color:#00add6;'></i>\
        <span class='dht-labels'>Humidity</span>\
        <span>%.2f</span>\
        <sup class='units'>&percnt;</sup>\
      </p>\
  </body>\
</html>",
           readDHTTemperature(), readDHTHumidity()
          );
  
  // Trimiterea răspunsului către client
  server.send(200, "text/html", msg);
}

void setup(void) {
  Serial.begin(115200);
  dht.begin();
  
  // Configurare modul Wi-Fi cu IP static
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }
  
  // Conectare la rețeaua Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("");

  // Așteptare conectare Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Inițializare mDNS
  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  // Setare ruta pentru pagina principală
  server.on("/", handleRoot);

  // Pornire server web
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  // Gestionare cereri clienți
  server.handleClient();
  delay(2); // Permite CPU-ului să gestioneze alte sarcini
}

// Funcție pentru citirea temperaturii de la senzorul DHT
float readDHTTemperature() {
  float t = dht.readTemperature();
  if (isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  } else {
    Serial.println(t);
    return t;
  }
}

// Funcție pentru citirea umidității de la senzorul DHT
float readDHTHumidity() {
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  } else {
    Serial.println(h);
    return h;
  }
}

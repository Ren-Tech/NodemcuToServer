#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ArduinoHttpClient.h>


// Node.js server endpoint
const char* serverAddress = "server-1tog.onrender.com";
const int serverPort = 3000; // Port for HTTP

void setup() {
  Serial.begin(9600);

  // Initialize Wi-FiManager
  WiFiManager wifiManager;

  // Disconnect Wi-Fi every time the device powers off (remove if not needed)
  WiFi.disconnect(true);

  // Connect to Wi-Fi or configure credentials
  if (!wifiManager.autoConnect("Wi-Fi Manager", "wifi_manager")) {
    Serial.println("Failed to connect or configure. Restarting...");
    ESP.restart();
  }

  // Print connection status
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected: " + WiFi.SSID());
  } else {
    Serial.println("Not Connected");
  }
}

void loop() {
  while (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    if (data.startsWith("pH: ")) {
      float pHValue = data.substring(4).toFloat();
      sendSensorData("pH", pHValue);
    } else if (data.startsWith("Turbidity: ")) {
      int turbidity = data.substring(11).toInt();
      sendSensorData("turbidity", turbidity);
    }
  }
  delay(1000);
}

void sendSensorData(const char* sensorType, float sensorValue) {
  WiFiClient wifiClient;

  // Construct the JSON payload
  String jsonPayload = "{\"sensor\":\"" + String(sensorType) + "\",\"value\":" + String(sensorValue) + "}";

  HttpClient client = HttpClient(wifiClient, serverAddress, serverPort);

  client.beginRequest();
  client.post("/sensor-data");
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", String(jsonPayload.length()));
  client.beginBody();
  client.print(jsonPayload);
  client.endRequest();

  int statusCode = client.responseStatusCode();

  if (statusCode == 200) {
    Serial.println("Sent sensor data successfully!");
  } else {
    Serial.print("Error sending data. Status code: ");
    Serial.println(statusCode);
  }
}

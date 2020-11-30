#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>

ESP8266WebServer server(80);
SoftwareSerial SerialGPS(4, 5);
TinyGPS gps;
StaticJsonDocument<1024> doc;

extern const char* ssid;
extern const char* pass;

const char* uuid_device = "";

extern void smartdelay(unsigned long ms);
extern void print_float(float val, float invalid, int len, int prec);
extern void print_int(unsigned long val, unsigned long invalid, int len);
extern void print_date(TinyGPS &gps);
extern void print_str(const char *str, int len);

const char host[] = "192.168.1.8"; // Socket.IO Server Address
const int port = 3333; // Socket.IO Port Address
const char path[] = "/"; // Socket.IO Base Path

void controlled(const char* message, size_t length) {
  Serial.println(message);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  delay(5000);
  Serial.print("Iniciando BlendBus Terminal");
  delay(10);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.print(".");
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
  }
  delay(500);
  Serial.println();
  Serial.println("Conectado a Rede Sem Fio");

  SerialGPS.begin(9600);

}

void loop() {

  float flat, flon;
  unsigned long age, date, time, chars = 0;
  unsigned short sentences = 0, failed = 0;
  static const double POINT_LAT = 51.508131, POINT_LON = -0.128002;
  String json;

  gps.f_get_position(&flat, &flon, &age);

  float altitude = gps.f_altitude();
  float latitude = flat;
  float longitude = flon;
  float velocidademedia = gps.f_speed_kmph();

  HTTPClient http;
  
  http.begin("http://192.168.1.8:3333/");
  
  doc["device_id"] = uuid_device;
  doc["latitude"] = latitude;
  doc["longitude"] = longitude;
  doc["velocidademedia"] = velocidademedia;
  doc["altitude"] = altitude;

  serializeJson(doc, json);
  http.POST(json);
  
  smartdelay(5000);
}

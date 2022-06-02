#include <Arduino.h>
#include <wifi.h>
#include <PubSubClient.h>

const char* ssid = "V2027";
const char* password = "123abc4d";
const char* mqtt_server = "192.168.223.119";
const int Aread = 34;

WiFiClient espClient;
PubSubClient client(espClient);

long Msg = 0;
char m[10], mm[10];
float Amp = 0.0;
float W = 0.0;

void setup_wifi();
void callback(char* topic, byte* message, unsigned int length);
void reconnect();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback); 
}

void loop() {
  // put your main code here, to run repeatedly:
  if(!client.connected()){
    reconnect();
  }
  client.loop();

  //unsigned int x = 0;
  float Smpl = 0.0, Avg = 0.0, AF = 0.0;

  for (int x=0; x<20; x++){
    Amp = analogRead(Aread);
    Smpl = Smpl + Amp; 
    delay(500);
  } //Un sample cada mili-segundo de un segundo

  Avg = Smpl/1000.0;

  AF = (Avg * (3.3/1024.0))/10;
  W = AF * 5.0;

  dtostrf(AF, 1, 2, m);
  Serial.println(AF);
  //Serial.println(m);
  client.publish("esp32/AMP", m);

  dtostrf(W, 1, 2, mm);
  Serial.println(W);
  //Serial.println(mm);
  client.publish("esp32/WATT", mm);
  //delay(50);
}

void setup_wifi() {
  delay(10);
  //WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting... ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.println("Mensaje recibido en Topic: ");
  Serial.print(topic);
  Serial.println(". Mensaje: ");
  String Mess;

  for(int i = 0; i< length; i++){
    Serial.println((char)message[i]);
    Mess += (char)message[i];
  }

  if(String(topic) == "esp32/Prove"){
    Serial.print("Prueba...");
  }
}

void reconnect() {
  while(!client.connected()){
    Serial.print("Intentando conexión MQTT... ");
    if(client.connect("ESP8266Client")){
      client.subscribe("esp32/Prove");
    }else{
      Serial.println(client.state());
      delay(5000);
    }
  }
}

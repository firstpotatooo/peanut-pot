#include <WiFi.h>
#include <WebSocketsServer.h>

// 1. Wi-Fi 設定
const char* ssid     = "*****";
const char* password = "********";

WebSocketsServer webSocket = WebSocketsServer(81);

// 2. 腳位定義
const int joystickX = 34;  
const int joystickY = 35;  
const int joystickSW = 32; 
const int bigButton = 23;  

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if(type == WStype_CONNECTED) { 
    Serial.printf("[%d] 網頁已成功連線！\n", num); 
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(joystickX, INPUT);
  pinMode(joystickY, INPUT);
  pinMode(joystickSW, INPUT_PULLUP);
  
  // 🛠️ 修正：改為內建上拉電阻，防短路、防干擾
  pinMode(bigButton, INPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWi-Fi 已連線！IP 位址: ");
  Serial.println(WiFi.localIP());

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();

  // 讀取所有感測器數值
  int xVal = analogRead(joystickX);
  int yVal = analogRead(joystickY);
  int jSW = (digitalRead(joystickSW) == LOW) ? 1 : 0; 
  
  // 🛠️ 修正：因為接了 GND，所以讀取到 LOW 才是真的被按下去
  int btnState = (digitalRead(bigButton) == HIGH) ? 1 : 0; 

  // 打包成 JSON 字串傳給網頁
  String jsonString = "{\"x\":" + String(xVal) + 
                      ",\"y\":" + String(yVal) + 
                      ",\"sw\":" + String(jSW) + 
                      ",\"btn\":" + String(btnState) + "}";

  // 廣播給所有連線的網頁
  webSocket.broadcastTXT(jsonString);
  
  delay(50); 
}

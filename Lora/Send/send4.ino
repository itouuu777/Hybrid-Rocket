#include <Arduino.h>
#include <LoRa_E220.h>

// LoRaモジュールの設定
LoRa_E220 e220ttl(&Serial2, 33, 25, 26); // RX AUX M0 M1
#define M0_PIN 25
#define M1_PIN 26

void setup() {
  Serial.begin(9600); // デバッグ用シリアル通信
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // LoRa用シリアル通信

  // M0, M1ピンを設定
  pinMode(M0_PIN, OUTPUT);
  pinMode(M1_PIN, OUTPUT);

  // ループバックモードの設定
  digitalWrite(M0_PIN, LOW); // M0 = LOW
  digitalWrite(M1_PIN, HIGH); // M1 = HIGH

  // LoRaモジュールの初期化
  if (e220ttl.begin()) {
    Serial.println("LoRa module initialized successfully in Loopback mode");
  } else {
    Serial.println("Failed to initialize LoRa module");
  }

  // 現在の設定を確認
  ResponseStructContainer c = e220ttl.getConfiguration();
  if (c.status.code == 1) {
    Configuration receivedConfiguration = *(Configuration *)c.data;
    Serial.println("Current Configuration:");
    Serial.print("Mode: ");
    Serial.println(receivedConfiguration.TRANSMISSION_MODE.fixedTransmission ? "Fixed" : "Transparent");
    Serial.print("Channel: ");
    Serial.println(receivedConfiguration.CHAN, HEX);
  } else {
    Serial.println("Failed to retrieve configuration: " + c.status.getResponseDescription());
  }
}

void loop() {
  if (digitalRead(M0_PIN) == LOW && digitalRead(M1_PIN) == HIGH) {
    Serial.println("LoRa module is in Loopback mode.");
  } else {
    Serial.println("LoRa module is NOT in Loopback mode. Check M0/M1 connections.");
  }

  // デバッグ用送信データ
  const char* testMessage = "Hello, LoRa!";
  ResponseStatus rs = e220ttl.sendMessage((uint8_t *)testMessage, strlen(testMessage));
  // データを送信
  ResponseStatus rs = e220ttl.sendMessage(testMessage);
  if (rs.code == 1) {
    Serial.println("Message sent: " + String(testMessage));
  } else {
    Serial.println("Failed to send message");
    Serial.println(rs.getResponseDescription());
  }

  delay(1000);

  // データを受信
  if (e220ttl.available() > 0) {
    Serial.println("Data available for receiving!");
    ResponseContainer rc = e220ttl.receiveMessage();
    if (rc.status.code == 1) {
      Serial.println("Received message: " + String(rc.data));
    } else {
      Serial.println("Failed to receive message");
      Serial.println(rc.status.getResponseDescription());
    }
  } else {
    Serial.println("No data available for receiving.");
  }

  delay(1000); // 1秒間隔で送信・受信
}

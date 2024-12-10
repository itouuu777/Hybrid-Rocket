#include <Arduino.h>
#include <LoRa_E220.h>
#include <LoRa.h>
#include <Wire.h>
#include <Ticker.h>

// LoRaモジュールのピン設定
LoRa_E220 e220ttl(&Serial2, 33, 25, 26); // RX AUX M0 M1

Configuration configuration; // Configurationインスタンスの作成



void setup() {
  // デバッグ用シリアル通信開始
  Serial.begin(9600);

  // LoRaモジュールの初期化
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX, TX設定
  e220ttl.begin();
  if (e220ttl.begin()) {
    Serial.println("LoRa module initialized successfully");
  } 
  else {
    Serial.println("Failed to initialize LoRa module");
  }

  // LoRaモジュールの設定
  configuration.ADDL = 0x00; // アドレスの下位
  configuration.ADDH = 0x00; // アドレスの上位
  configuration.CHAN = 0x0F;    // 通信チャネル
  configuration.SPED.uartBaudRate = UART_BPS_9600; // UART通信速度
  configuration.SPED.airDataRate = AIR_DATA_RATE_010_24; // 空中通信速度
  configuration.SPED.uartParity = MODE_00_8N1; // パリティビット
  configuration.OPTION.subPacketSetting = SPS_200_00; // パケットサイズ
  configuration.OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_DISABLED; // RSSI設定
  configuration.OPTION.transmissionPower = POWER_13; // 送信電力
  configuration.TRANSMISSION_MODE.enableRSSI = RSSI_DISABLED; // RSSI無効化
  configuration.TRANSMISSION_MODE.fixedTransmission = FT_TRANSPARENT_TRANSMISSION; // 透明通信
  configuration.TRANSMISSION_MODE.enableLBT = LBT_DISABLED; // 干渉チェック無効化
  configuration.TRANSMISSION_MODE.WORPeriod = WOR_2000_011; // WORタイミング

  
ResponseStatus rs = e220ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
if (rs.code == 1) {
    Serial.println("Configuration written successfully!");
} else {
    Serial.println("Failed to write configuration:");
    Serial.println(rs.getResponseDescription());
}


ResponseStructContainer c = e220ttl.getConfiguration();
if (c.status.code == 1) {
    Configuration receivedConfiguration = *(Configuration *)c.data;
    Serial.println("Configuration read successfully!");
    Serial.println(receivedConfiguration.ADDH, HEX);
    Serial.println(receivedConfiguration.ADDL, HEX);
    Serial.println(receivedConfiguration.CHAN, HEX);
} else {
    Serial.println("Failed to read configuration:");
    Serial.println(c.status.getResponseDescription());
}

}



void input() {
  /*
  do {
    String command = Serial.readStringUntil('\n');
    ResponseStatus rs = e220ttl.sendMessage(command);
    if (rs.code == 1) { 
        Seiral.println("command: " + command); 
        Serial.println("Message sent successfully!");
    }    
  }while(Serial.available()) ;
*/

  while(Serial.available()) {
    String command = Serial.readStringUntil('\n');
    ResponseStatus rs = e220ttl.sendMessage(command);
    if (rs.code == 1) { 
        Serial.println(command); 
        Serial.println("Message sent successfully!");
    }
  
 
}
}

void receive() {
  while(e220ttl.available()>1) {
      //digitalWrite(ledPin, HIGH);
      Serial.println("Message received!");
      // read the String message
      ResponseContainer rc = e220ttl.receiveMessageRSSI();
      String message = e220ttl.receiveMessageRSSI();

      if (message == "send") {
        Serial.println("message is [send]");
        Serial.print("message: ");
        Serial.prinln(message);
      }
      else if (message == "kimura") {
        Serial.println("message is [kimura]");
        Serial.print(message);
        Serial.println(" sinichi");
      }
      else() {  
        Serial.print("message: ");
        Serial.prinln(message);
    }
}

void loop(){
  input();
  receive();
}

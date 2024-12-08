#include <Arduino.h>
#include <LoRa_E220.h>

// LoRaモジュールのピン設定
LoRa_E220 e220ttl(&Serial2, 33, 25, 26); // RX AUX M0 M1

Configuration configuration; // Configurationインスタンスの作成


void setup() {
  // デバッグ用シリアル通信開始
  Serial.begin(9600);

  // LoRaモジュールの初期化
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // RX, TX設定
  e220ttl.begin();

  // LoRaモジュールの設定
  configuration.ADDL = 0x00; // アドレスの下位
  configuration.ADDH = 0x00; // アドレスの上位
  configuration.CHAN = 0;    // 通信チャネル
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

  // 設定の適用
/*  ResponseStatus rs = e220ttl.setConfiguration(configuration, WRITE_CFG_PU);
  if (rs.code != 1) {
    Serial.println(rs.getResponseDescription()); // エラー内容を表示
  } else {
    Serial.println("Configuration set successfully!");
  }*/
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("debug");
  delay(1000);
    if (e220ttl.available()>1) {
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

    // Is something goes wrong print error
    if (rc.status.code!=1){
      Serial.println(rc.status.getResponseDescription());
    }else{
      // Print the data received
      Serial.println(rc.status.getResponseDescription());
      Serial.println(rc.data);
      Serial.print("RSSI: "); Serial.println(rc.rssi, DEC);
    }

    delay(500);
    e220ttl.begin();
    
    Serial.print("send message");
    // Send message
    ResponseStatus rs = e220ttl.sendMessage("OK!");
    // Check If there is some problem of succesfully send
    Serial.println(rs.getResponseDescription());

    //digitalWrite(ledPin, LOW);

  }
}

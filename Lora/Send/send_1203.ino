#include <Arduino.h>
#include <LoRa_E220.h>
#include <LoRa.h>

// LoRaモジュールのピン設定
LoRa_E220 e220ttl(&Serial2, 33, 25, 26); // RX AUX M0 M1
#define send_button_pin 27 // 送信ボタンの入力ピン

int button_no = 0;
int char_button = 0;
int binary_button = 0;
Configuration configuration; // Configurationインスタンスの作成

void IRAM_ATTR sendbtn() { // 割り込み関数
  delayMicroseconds(250); // チャタリング防止
  if ((digitalRead(send_button_pin) == LOW) && (button_no == 0)) {
    button_no = 1;
  }
}

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


  // ボタンピンの初期化
  pinMode(send_button_pin, INPUT_PULLUP); // ボタンピンをプルアップ設定
  attachInterrupt(digitalPinToInterrupt(send_button_pin), sendbtn, FALLING); // 割り込み設定




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
/*
ResponseStructContainer c = e220ttl.getConfiguration();
if (c.status.code == 1) {
    Configuration receivedConfiguration = *(Configuration *)c.data;
    Serial.println(receivedConfiguration.ADDH, HEX);
    Serial.println(receivedConfiguration.ADDL, HEX);
    Serial.println(receivedConfiguration.CHAN, HEX);
} else {
    Serial.println(c.status.getResponseDescription());
}*/

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

void loop() {
  Serial.println("oh yeah");
  delay(1000);
  //LoRa.beginPacket();
  //LoRa.print("hello");
  //LoRa.endPacket();
  Serial.println("ok");
  // メッセージ受信
  if (e220ttl.available() > 1) {
    Serial.println("Message received!");

    // メッセージの受信
    ResponseContainer rc = e220ttl.receiveMessageRSSI();
    
    if (rc.status.code != 1) {
      Serial.println(rc.status.getResponseDescription());
    } else {
      Serial.println(rc.status.getResponseDescription());
      Serial.println(rc.data);
      Serial.print("RSSI: ");
      Serial.println(rc.rssi, DEC);
    }
    
  }
  if (Serial.available()) {
        String command = Serial.readStringUntil('\n'); // 改行まで読み取る

        // コマンドに応じた処理を実行
        if (command == "s") {
          button_no = 1;
        } 
        else if (command == "c") {
          char_button = 1;
        }
        else if (command == "p") {
          Serial.println("ononoa");
          binary_button = 1;
        }
        else {
          Serial.print("no_command");
        }
    }



  // メッセージ送信
/*  if (button_no == 1) {
    ResponseStatus rs = e220ttl.sendMessage("test message");
    Serial.println(rs.getResponseDescription()); // 送信結果の確認

    button_no = 0;
  }*/

// strig型
  if (button_no == 1) {
      ResponseStatus rs = e220ttl.sendMessage("between_oppai_and_oppai ");
      if (rs.code == 1) { 
          Serial.println("Message sent successfully!");
      }
  }

  // char型
  else if (char_button == 1){ 
    const char message[] = "between_oppai_and_oppai";
    ResponseStatus rs = e220ttl.sendMessage(message);
    Serial.println("I can fly");
    if (rs.code == 1) { 
          Serial.println("Message sent successfully!");
    } 

    }

  //バイナリそのまま
  else if (binary_button == 1) {
      Serial.println("I don't ");
      uint8_t binaryMessage[] = {0x41, 0x42, 0x43, 0x44}; // 'ABCD'を送信
      ResponseStatus rs = e220ttl.sendMessage(binaryMessage, sizeof(binaryMessage));
      Serial.println("I don't know");
      if (rs.code == 1) { 
          Serial.println("Message sent successfully!");
      } 

    }
  //Serial.println(rs.getResponseDescription()); // 結果の詳細を表示
  button_no = 0;
  char_button = 0;
  binary_button = 0;  
  
}

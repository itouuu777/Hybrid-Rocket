#include <Arduino.h>
#include <LoRa_E220.h>

// LoRaモジュールのピン設定
LoRa_E220 e220ttl(&Serial2, 33, 25, 26); // RX AUX M0 M1
#define send_button_pin 27 // 送信ボタンの入力ピン

int button_no = 0;
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

  // ボタンピンの初期化
  pinMode(send_button_pin, INPUT_PULLUP); // ボタンピンをプルアップ設定
  attachInterrupt(digitalPinToInterrupt(send_button_pin), sendbtn, FALLING); // 割り込み設定

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
  Serial.println("oh yeah");
  delay(1000);
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
        if (command == "o") {
          button_no = 1;
        } 
        else if (command == "f") {
          button_no = 0;
        }
    }


  // メッセージ送信
/*  if (button_no == 1) {
    ResponseStatus rs = e220ttl.sendMessage("test message");
    Serial.println(rs.getResponseDescription()); // 送信結果の確認

    button_no = 0;
  }*/
if (button_no == 1) {
    ResponseStatus rs = e220ttl.sendMessage("test message");
    if (rs.code == 1) { // 送信成功
        Serial.println("Message sent successfully!");
    } else { // 送信失敗
        Serial.println("Message failed to send:");
    }
    Serial.println(rs.getResponseDescription()); // 結果の詳細を表示
    button_no = 0;
}

}

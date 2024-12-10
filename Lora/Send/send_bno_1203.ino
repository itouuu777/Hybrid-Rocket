#include <Arduino.h>
#include <LoRa_E220.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_BNO055.h>
#include <Ticker.h>


// LoRaモジュールのピン設定
LoRa_E220 e220ttl(&Serial2, 33, 25, 26); // RX AUX M0 M1
#define send_button_pin 27 // 送信ボタンの入力ピン

int send = 0;
Configuration configuration; // Configurationインスタンスの作成

void IRAM_ATTR sendbtn() { // 割り込み関数
  delayMicroseconds(250); // チャタリング防止
  if ((digitalRead(send_button_pin) == LOW) && (button_no == 0)) {
    button_no = 1;
  }
}

Ticker bno055ticker; //タイマー割り込み用のインスタンス
#define BNO055interval 10 //何ms間隔でデータを取得するか

//Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire); //ICSの名前, デフォルトアドレス, 謎
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

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

  pinMode(21, INPUT_PULLUP); //SDA 21番ピンのプルアップ(念のため)
  pinMode(22, INPUT_PULLUP); //SDA 22番ピンのプルアップ(念のため)

  Serial.begin(9600);
  Serial.println("Orientation Sensor Raw Data Test"); Serial.println("");

  if (!bno.begin()) // センサの初期化
  {
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  delay(10000);

  /* Display the current temperature */
  int8_t temp = bno.getTemp();
  Serial.print("Current Temperature: ");
  Serial.print(temp);
  Serial.println(" C");
  Serial.println("");

  bno.setExtCrystalUse(false);

  Serial.println("Calibration status values: 0=uncalibrated, 3=fully calibrated");
  //bno055ticker.attach_ms(BNO055interval, get_bno055_data);

}

void loop() {
  Serial.println("wait");
  imu::Quaternion quat = bno.getQuat();
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
          send = 1;
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
  double w = quat.w();
  double x = quat.x();
  double y = quat.y();
  double z = quat.z();

  String w_str = String(w, 6); // 小数点以下6桁まで
  String x_str = String(x, 6);
  String y_str = String(y, 6);
  String z_str = String(z, 6);
  Serial.println(w_str);

// strig型
  if (send == 1) {
      ResponseStatus rs = e220ttl.sendMessage(w_str);
      if (rs.code == 1) { 
          Serial.println("Message sent successfully!");
      }
  }

    }
  //Serial.println(rs.getResponseDescription()); // 結果の詳細を表示
  send = 0;
  
}

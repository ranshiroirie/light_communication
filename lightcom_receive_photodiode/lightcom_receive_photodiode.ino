#define DMS 75
#define passlength 8 //パスの長さ　変更が可能だが送信側と合わせる必要あり　デフォルトは8
#define ASCII 100 //読み込む最大の文字数
#define READPIN A0 //光センサーのアナログピン
#define startpass 0b10101001 //始まりの合図のフレーズ
#define endpass 255 //終わりの合図のフレーズ
#define threshold 200 //光センサーの閾値
#define TIMEOUT 3000 //計測のタイムアウト時間
#define timer_active 0

#if(timer_active)
unsigned long stime, etime;
#endif

void setup() {
  DDRB = B00000000; //PIN8をINPUTに設定
}

void loop() {
  int trigger = 0;
  for (int i = 0; i < passlength; i++) { //あらかじめ指定したパスの配列と読み取った配列が一致するかどうかみる
    if (SensorRead() == (startpass & (1 << passlength - 1 - i) ? HIGH : LOW)) {
      trigger++;
    } else {
      break;
    }
    delayMicroseconds(DMS);
  }
  if (trigger == passlength) { //もしパスの配列と一致したら
    //    Serial.println("success");
    int spel = 0;
    char ascii[ASCII]; //ASCIIコードの配列
    unsigned long timeout = millis();
    while (millis() - timeout <= TIMEOUT) { //もし読み取った値が特定のASCIIコードもしくは一致時間経過するまで繰り返す
#if(timer_active)
      stime = micros();
#endif
      int byte_to_ascii = 0;
      for (int i = 0; i < passlength; i++) {
        if (SensorRead() == 1) {
          int byte_to_dec = 1;
          for (int j = 0; j < passlength - i - 1; j++) {
            byte_to_dec *= 2;
          }
          byte_to_ascii += byte_to_dec;

          //          byte_to_ascii += pow(2, passlength - i - 1);

        }
        delayMicroseconds(DMS);
      }

#if(timer_active)
      etime = micros();
#endif
      //      Serial.print("byte_to_ascii = ");
      //      Serial.println(byte_to_ascii);
      if (byte_to_ascii == endpass) {
        break;
      }
      ascii[spel] = byte_to_ascii;
      spel += 1;
#if(timer_active)
      etime = micros();
#endif
    }

    Serial.begin(1000000);
#if(timer_active)
    Serial.print("かかった時間:");
    Serial.println(etime - stime);
#endif
    Serial.print("受信した文字は...");

    for (int k = 0; k <= spel; k++) {
      Serial.print((char)ascii[k]);
    }
    Serial.println();
    Serial.end();
    delay(100);
  }
}

// 読み込みの処理 ----------------------------------------------------------------//
int SensorRead() {
  int sensorValue = PINB & _BV(0);
  return sensorValue;
}

//300us 計測結果: 2476us
//100us 計測結果: 864us
//50us 計測結果: 464us

//デジタル化
//500ms 4044us 505.8us
//50us 416~424us 52~53us

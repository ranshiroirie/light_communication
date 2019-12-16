#define DMS 48.3
#define passlength 8 //パスの長さ　変更が可能だが送信側と合わせる必要あり　デフォルトは8
#define ASCII 255 //読み込む最大の文字数
#define startpass 0b10101001 //始まりの合図のフレーズ
#define endpass 255 //終わりの合図のフレーズ
#define TIMEOUT 300 //計測のタイムアウト時間
#define timer_active 0

#define F_CPU 16000000UL

#if(timer_active)
unsigned long stime, etime;
#endif

void setup() {
  DDRB = 0b00000000; //PIN8をINPUTに設定
}

void loop() {
  int trigger = 0;
  for (unsigned char i = 0; i < passlength; i++) { //あらかじめ指定したパスの配列と読み取った配列が一致するかどうかみる
    if ((PINB & 0x01) == (startpass & (1 << passlength - 1 - i) ? HIGH : LOW)) {
      trigger++;
    } else {
      break;
    }
    _delay_us(DMS);
  }

  if (trigger == passlength) { //もしパスの配列と一致したら
    unsigned char spel = 0;
    unsigned char ascii[ASCII]; //ASCIIコードの配列
    unsigned long timeout = millis();
    while (millis() - timeout <= TIMEOUT) { //もし読み取った値が特定のASCIIコードもしくは一致時間経過するまで繰り返す

#if(timer_active)
      stime = micros();
#endif

      ascii[spel] = 0x00;
      for (unsigned char i = 0; i < passlength; i++) {
        ascii[spel] |= (PINB & 0x01) << (passlength - 1 - i);
        _delay_us(DMS);
      }

#if(timer_active)
      etime = micros();
#endif

      if (ascii[spel] == endpass) {
        break;
      }
      spel += 1;
    }

    Serial.begin(1000000);
#if(timer_active)
    Serial.print("かかった時間:");
    Serial.println(etime - stime);
#endif
    Serial.print("受信した文字は...");

    for (unsigned char k = 0; k <= spel; k++) {
      Serial.print((char)ascii[k]);
    }
    Serial.println();
    Serial.end();
    delay(100);
  }
}

//300us 計測結果: 2476us
//100us 計測結果: 864us
//50us 計測結果: 464us

/*
  デジタル化
  500us 4044us 505.8us
  100us 820~828us 102.5~103.5us
  50us 420~428us 52.5~53.5us
  25us 220~228us 27.5~28.5us
*/

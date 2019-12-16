#define passlength 8 //パスの長さ　変更が可能だが送信側と合わせる必要あり　デフォルトは8
#define ASCII 255 //読み込む最大の文字数
#define startpass 0b10101001 //始まりの合図のフレーズ
#define endpass 255 //終わりの合図のフレーズ
#define TIMEOUT 300 //計測のタイムアウト時間

#define F_CPU 16000000UL

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
    asm volatile(
      "dly0:         \n"
      "ldi r16, 140  \n" //クロック数:2
      "mov r0, r16   \n" //クロック数:1
      "rjmp dly1     \n" //クロック数:2

      "dly1:         \n"
      "nop           \n" //クロック数:1
      "nop           \n" //クロック数:1
      "dec r0        \n" //クロック数:1
      "brne dly1     \n" //クロック数:2（成立時）/1（不成立時）
      "nop           \n" //クロック数:1
    );
  }

  if (trigger == passlength) { //もしパスの配列と一致したら
    unsigned char spel = 0;
    unsigned char ascii[ASCII]; //ASCIIコードの配列
    unsigned long timeout = millis();
    while (millis() - timeout <= TIMEOUT) { //もし読み取った値が特定のASCIIコードもしくは一致時間経過するまで繰り返す

      ascii[spel] = 0x00;
      for (unsigned char i = 0; i < passlength; i++) {
        ascii[spel] |= (PINB & 0x01) << (passlength - 1 - i);
        asm volatile(
          "dly3:         \n"
          "ldi r16, 140  \n" //クロック数:2
          "mov r0, r16   \n" //クロック数:1
          "rjmp dly4     \n" //クロック数:2

          "dly4:         \n"
          "nop           \n" //クロック数:1
          "nop           \n" //クロック数:1
          "dec r0        \n" //クロック数:1
          "brne dly4     \n" //クロック数:2（成立時）/1（不成立時）
          "nop           \n" //クロック数:1
        );
      }

      if (ascii[spel] == endpass) {
        break;
      }
      spel += 1;
    }

    Serial.begin(1000000);
    Serial.print("受信した文字は...");

    for (unsigned char k = 0; k <= spel; k++) {
      Serial.print((char)ascii[k]);
    }
    Serial.println();
    Serial.end();
    delay(100);
  }
}

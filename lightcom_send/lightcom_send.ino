#define startpass 0b10101001 //始まりの合図のフレーズ
#define endpass 0b11111111 //終わりの合図のフレーズ
#define DMS 50
#define DMS_FIX 0
#define SPEL 50
#define passlength 8 //パスの長さ　変更が可能だが送信側と合わせる必要あり　デフォルトは8
#define print_go 0

byte datasize = 0;
byte digit[SPEL];

#if(print_go)
unsigned long stime, etime;
#endif

void setup() {
  DDRB = B00000001; //PIN8をHIGHに設定
  Serial.begin(1000000);
  Serial.println("送りたい文字を入力して送信ボタンを押してください");
  while (Serial.available() == 0) {
    delay(20);
    datasize = Serial.available();
    for (byte i = 0; i < datasize; i++) {
      digit[i] = Serial.read();
      Serial.print("文字のASCIIコード:");
      Serial.println(digit[i]);
    }
    if (datasize != 0) {
#if(!print_go)
      Serial.end();
#endif
      break;
    }
  }
}

void loop() {
#if(print_go)
  Serial.println("Start");
#endif
  lighting(startpass); //開始の合図
  for (byte i = 0; i < datasize; i++) { //文字の送信
    lighting(digit[i]);
  }
  lighting(endpass); //終わりの合図
#if(print_go)
  Serial.println("Finish");
#endif
}

//フレーズ点滅------------------------------------------------------------
void lighting(byte phrase) {
#if(print_go)
  stime = micros();
#endif
  for (int j = 0; j < passlength; j++) {
    if ((phrase & (1 << passlength - 1 - j) ? HIGH : LOW) == 1) {
      PORTB = B00000001; //8PINをHIGH
    } else {
      PORTB = B00000000; //8PINをLOW
    }
    delayMicroseconds(DMS + DMS_FIX);
  }

#if(print_go)
  etime = micros();
  Serial.println(etime - stime);
#endif
}

//300us 計測結果: 2468us
//100us 計測結果: 864us
//50us 計測結果: 464us

//デジタル化
//50us 416~424us 52~53us

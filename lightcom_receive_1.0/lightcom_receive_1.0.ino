#include <LiquidCrystal.h> //LCDパネルのライブラリ
#define DMS 30000
#define BIT 300 
#define passlength 8 //パスの長さ　変更が可能だが送信側と合わせる必要あり　デフォルトは8
#define ASCII 100 //読み込む最大の文字数

const int audioInPin = A6;  // 光センサーのピン番号

int sensorValue = 0;        // 光センサーの値
int sensorMax, sensorMin;
// unsigned long passTime;
int threshold;
int iteration;
int flag = 0;
unsigned char pulse[BIT];

int pass[passlength] = {1, 0, 1, 0, 1, 0, 0, 1}; //パスの配列　配列の個数はpasslengthで指定した数と、中身は送信側のものと合わせる　
int endpass = 171; //読み込みの終了を合図するASCIIコードの値
int pulsenumber; //始めに読み込む配列番号
int bits = 0;
int spel = 0;
char bar[ASCII]; //ASCIIコードの配列
int finish = 0;
int trigger = 0;
bool result = false;
unsigned long timeout, timenow;

LiquidCrystal lcd( 4, 6, 9, 10, 11, 12); //LCDパネルのピン番号　詳しくはネットで調べる

void setup() {
  lcd.begin(16, 2); //LCDパネルの有効化
  lcd.clear(); //LCDパネルのリセット
  lcd.setCursor(0, 0); //LCDパネルに出力する位置の指定
  lcd.print("Welcome, HOSEI");
  // initialize serial communications at 9600 bps:
  Serial.begin(19200);
}

void loop() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Receiving...");

  // identfy the threshold, which is set between the bright value and dark value
  // START threshold setting -------------------------------------------------------------//
  sensorMax = 850;
  sensorMin = 400;
  //  passTime = micros();
  for (int i = 0; i < BIT; i++) {
    sensorValue = analogRead(audioInPin);
    if (sensorValue < sensorMin) sensorMin = sensorValue;
    if (sensorValue > sensorMax) sensorMax = sensorValue;
  }
  threshold = (sensorMax + sensorMin) / 2;
  Serial.print("Threshold =  ");
  Serial.println(threshold);
  // END threshold setting ----------------------------------------------------------------//

  reading(); //読み込みの処理

  for (int i = 0; i < BIT / 10; i++) {
    for (int j = 0; j < 10; j++) {
      Serial.print("[");
      Serial.print(pulse[i * 10 + j]);
      Serial.print("]");
    }
    Serial.println("");
  }
  
  receive(); //受信時の処理
  delay(2000);

  /* passTime = micros()-passTime;
    Serial.print(passTime);
    Serial.print("\t");
    Serial.print(sensorMin);
    Serial.print("\t");
    Serial.println(sensorMax);
  */
}

// 読み込みの処理 ----------------------------------------------------------------//
void reading() {
  for (int i = 0; i < BIT; i++) {
    sensorValue = analogRead(audioInPin);
    if (sensorValue > threshold) pulse[i] = 1;
    else pulse[i] = 0;
    delayMicroseconds(DMS);
  }
}

// 受信時の処理 ----------------------------------------------------------------//
void receive() {
  result = false;

  for (int j = 0; j < BIT; j++) {
    trigger = 0;
    for (int i = 0; i < passlength; i++) {
      if (pulse[j + i] == pass[i]) {
        trigger += 1;
      }
    }
    if (trigger == passlength) {
      pulsenumber = j + passlength;
      Serial.println("success");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Received!");
      lcd.setCursor(0, 1);
      lcd.print("Prease wait...");
      grouping(); //配列を文字に起こす処理
      break;
    }
  }
  if (result == false) { //もし何も受信できなかったら
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("NO Message");
    delay(1000);
  }
}

//配列の取り扱い ----------------------------------------------------------------//
void grouping() {
  timeout = millis();
  while (finish != endpass || timenow - timeout <= 1) {
    timenow = millis();
    Serial.print("bits = ");
    Serial.println(bits);
    bits = 0;
    for (int n = 0 + 8 * spel; n < 8 * (spel + 1); n++) {
      for ( int m = 0; m < 7 - (n - 8 * spel); m++) {
      }
      bits += pulse[pulsenumber + n];
    }
    finish = bits;
    bar[spel] = bits;
    //    Serial.println((char)bits);
    spel += 1;
  }
  Serial.print("受信した文字は...");
  lcd.clear();
  lcd.setCursor(0, 0);
  for (int k = 0; k < spel - 1; k++) {
    Serial.print((char)bar[k]);
    lcd.print((char)bar[k]);
  }
  Serial.println();
  delay(2000);
  result = true;
}


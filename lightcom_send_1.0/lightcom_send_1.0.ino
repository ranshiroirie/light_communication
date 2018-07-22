#define DMS 30000
#define BIT 300
#define passlength 8 //パスの長さ　変更が可能だが送信側と合わせる必要あり　デフォルトは8

const int ledPin = 12;  //LEDのピン番号

unsigned char pulse[BIT];
int pass[passlength] = {1, 0, 1, 0, 1, 0, 0, 1}; //パスの配列　配列の個数はpasslengthで指定した数と、中身は受信側のものと合わせる　
int endpass[8] = {1, 0, 1, 0, 1, 0, 1, 1}; //読み込みの終了を合図するASCIIコードの二進数にした配列
byte datasize;
byte sendsize;
byte digit[9];
int binary[8] = {128, 64, 32, 16, 8, 4, 2, 1};

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(19200);
  Serial.println("送りたい文字を入力して送信ボタンを押してください");
}

void loop() {
  Serial.println("Start");
  
  for (int j = 0; j < passlength; j++) {
    if (pass[j] == 1) {
      digitalWrite(ledPin, HIGH);
      delayMicroseconds(DMS);
    } else {
      digitalWrite(ledPin, LOW);
      delayMicroseconds(DMS);
    }
    Serial.print(pass[j]);
  }
  Serial.println();

  spels(); //入力された文字の変換
  
  sending(); //文字の送信
  
  for (int k = 0; k < 8; k++) {
    if (endpass[k] == 1) {
      digitalWrite(ledPin, HIGH);
      delayMicroseconds(DMS);
    } else {
      digitalWrite(ledPin, LOW);
      delayMicroseconds(DMS);
    }
    Serial.print(endpass[k]);
  }
  Serial.println();
  
  Serial.println("Finish");
}

//入力された文字の変換------------------------------------------------------------
void spels() {
  datasize = Serial.available();
  if (datasize > 0) {
    delay(20);
    datasize = Serial.available();
    sendsize = datasize;

    Serial.print("入力された文字数:");
    Serial.println(datasize);

    for (byte i = 0; i < sendsize; i++) {
      digit[0] = Serial.read();
      Serial.print("文字のASCIIコード:");
      Serial.println(digit[0]);
      for (int j = 0; j < 8; j++) {
        if (digit[j] >= binary[j]) {
          pulse[i * 8 + j] = 1;
          digit[j + 1] = digit[j] - binary[j];
        } else {
          pulse[i * 8 + j] = 0;
          digit[j + 1] = digit[j];
        }
        Serial.print(pulse[i * 8 + j]);
      }
      Serial.println();
    }
  }
}

//変換した文字の送信------------------------------------------------------------
void sending() {
  for (byte i = 0; i < sendsize; i++) {
    for (int j = 0; j < 8; j++) {
      if (digit[j] == 1) {
        digitalWrite(ledPin, HIGH);
        delayMicroseconds(DMS);
      } else {
        digitalWrite(ledPin, LOW);
        delayMicroseconds(DMS);
      }
      Serial.print(pulse[i * 8 + j]);
    }
    Serial.println();
  }
}

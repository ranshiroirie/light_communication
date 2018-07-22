# light_communication
光センサー(Cdsセル)とLEDを使った光通信
このプログラムでは、２台のArduinoを用いて、送信用と受信用のプログラムを書き込んで通信を行う。

・lightcom_send
シリアルモニターから任意の文字を入力して、送信ボタンを押すことで、文字をASCIIコードに変換して二進数にして、LEDの点滅による送信を行う。

・lightcom_receive
送信用Arduinoから発するLEDの点滅を、光センサーを用いて、特定時間毎にLEDの点滅具合から配列に1か０を収納する。
そこから、特定のパターンを見出して、ASCIIコードに直して文字として出力する。

int ledPin = 13; // chọn chân 13 báo hiệu LED
int inputPin = 2; // chọn ngõ tín hiệu vào cho PIR
int pirState = LOW; // Bắt đầu với không có báo động
int val = 0;
int pinSpeaker = 10; //chọn chân cho chuông khi có đột nhập

void setup() {
    pinMode(ledPin, OUTPUT);
    pinMode(inputPin, INPUT);
    pinMode(pinSpeaker, OUTPUT);
    Serial.begin(9600);
}

void loop() {
    val = digitalRead(inputPin); // đọc giá trị đầu vào.
    if (val == HIGH) // nếu giá trị ở mức cao.(1)
    {
        digitalWrite(ledPin, HIGH); // LED On
        playTone(300, 160); // thời gian chuông kêu
        delay(150);

        if (pirState == LOW) {
            Serial.println("Motion detected!");
            pirState = HIGH;
        }
    } else {
        digitalWrite(ledPin, LOW);
        playTone(0, 0);
        delay(300);
        if (pirState == HIGH) {
            Serial.println("Motion ended!");
            pirState = LOW;
        }
    }
}

void playTone(long duration, int freq) {
    duration *= 1000;
    int period = (1.0 / freq) * 1000000;
    long elapsed_time = 0;
    while (elapsed_time < duration) {
        digitalWrite(pinSpeaker, HIGH);
        delayMicroseconds(period / 2);
        digitalWrite(pinSpeaker, LOW);
        delayMicroseconds(period / 2);
        elapsed_time += (period);
    }
}

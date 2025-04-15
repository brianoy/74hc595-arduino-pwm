const int data_pin = 2;
const int latch_pin = 3;
const int clock_pin = 4;

const int NUM_LEDS = 8;
uint8_t brightness[NUM_LEDS];     // 各燈亮度 (0~255)
int8_t fadeDirection[NUM_LEDS];   // +1: 變亮，-1: 變暗
unsigned long lastUpdate[NUM_LEDS]; // 記錄上次更新時間
unsigned int fadeSpeed = 5;       // 呼吸速度 (ms)

// 初始化
void setup() {
  pinMode(data_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(latch_pin, OUTPUT);

  for (int i = 0; i < NUM_LEDS; i++) {
    brightness[i] = random(0, 255);         // 每顆燈從不同亮度開始
    fadeDirection[i] = (random(0, 2) == 0) ? 1 : -1;
    lastUpdate[i] = millis();
  }
}

// 主迴圈
void loop() {
  // 更新每顆燈的亮度
  for (int i = 0; i < NUM_LEDS; i++) {
    if (millis() - lastUpdate[i] >= fadeSpeed) {
      brightness[i] += fadeDirection[i];
      if (brightness[i] == 0 || brightness[i] == 255) {
        fadeDirection[i] *= -1; // 到頂或到底反轉方向
      }
      lastUpdate[i] = millis();
    }
  }

  // 進行軟體 PWM 掃描
  for (int pwm_level = 0; pwm_level < 256; pwm_level++) {
    byte data = 0;
    for (int i = 0; i < NUM_LEDS; i++) {
      if (brightness[i] > pwm_level) {
        data |= (1 << i);  // 該燈在此階段亮
      }
    }
    // 傳送資料到 74HC595
    digitalWrite(latch_pin, LOW);
    shiftOut(data_pin, clock_pin, MSBFIRST, data);
    digitalWrite(latch_pin, HIGH);
    
    delayMicroseconds(5); // 控制 PWM 頻率，越小越平滑
  }
}

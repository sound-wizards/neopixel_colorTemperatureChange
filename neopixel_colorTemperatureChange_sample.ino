#include <Adafruit_NeoPixel.h>

#define PIN A5        // NeoPixel 데이터 핀
#define NUM_LEDS 12   // NeoPixel LED 개수

Adafruit_NeoPixel strip(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

// RGB 구조체
struct RGB {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

// 색온도를 RGB로 변환하는 함수
RGB colorTemperatureToRgb(uint16_t kelvin) {
  float temperature = kelvin / 100.0;
  float r, g, b;

  // 적색 계산
  if (temperature <= 66) {
    r = 255;
  } else {
    r = temperature - 60;
    r = 329.698727446 * pow(r, -0.1332047592);
    r = constrain(r, 0, 255);
  }

  // 녹색 계산
  if (temperature <= 66) {
    g = temperature;
    g = 99.4708025861 * log(g) - 161.1195681661;
    g = constrain(g, 0, 255);
  } else {
    g = temperature - 60;
    g = 288.1221695283 * pow(g, -0.0755148492);
    g = constrain(g, 0, 255);
  }

  // 청색 계산
  if (temperature >= 66) {
    b = 255;
  } else {
    if (temperature <= 19) {
      b = 0;
    } else {
      b = temperature - 10;
      b = 138.5177312231 * log(b) - 305.0447927307;
      b = constrain(b, 0, 255);
    }
  }

  return RGB{(uint8_t)r, (uint8_t)g, (uint8_t)b};
}

void setup() {
  strip.begin();    // NeoPixel 초기화
  strip.show();     // 모든 LED 끄기
}

void loop() {
  int tempValue = smoothAnalogRead(A1); // A0에서 가변저항 값 읽기 (색온도)
  int brightnessValue = smoothAnalogRead(A3); // A1에서 가변저항 값 읽기 (밝기)

  // 색온도 범위: 1000K ~ 10000K
  uint16_t kelvin = map(tempValue, 0, 1023, 1000, 10000);

  // 밝기 범위: 0 ~ 1.0
  float brightness = map(brightnessValue, 0, 1023, 0, 100) / 100.0;

  // 색온도를 RGB로 변환
  RGB rgb = colorTemperatureToRgb(kelvin);

  // 밝기 조절
  uint8_t r = rgb.r * brightness;
  uint8_t g = rgb.g * brightness;
  uint8_t b = rgb.b * brightness;

  // 모든 LED에 동일한 색 적용
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }

  // LED 업데이트
  strip.show();

  // delay(50); // 약간의 지연 추가
}

// 아날로그 입력 값을 안정화하는 함수
int smoothAnalogRead(int pin) {
  const int numReadings = 10; // 평균을 위한 샘플 수
  int total = 0;

  for (int i = 0; i < numReadings; i++) {
    total += analogRead(pin);
    delay(5); // 샘플 간의 간격
  }

  return total / numReadings;
}

#include <Arduino.h>
// #include <Ultrasonic.h>
#include <NewPing.h>
#include "FastLED.h"

// Aqui fica o total de fitas led
const int TotalStrips = 5;

// Quantidade de leds por fita
const int LedsByStrip = 150 / 3;
// const int LedsByStrip = (quantidade aqui) / 3;

// A cada nova fita de led deve ser adicionado uma nova entrada aqui de acordo com o GPIO usado do esp32
const uint8_t Strip_0 = 21;
const uint8_t Strip_1 = 22;
const uint8_t Strip_2 = 23;
const uint8_t Strip_3 = 18;
const uint8_t Strip_4 = 5;

// A cada nova fita de led é adicionado nessa lista o novo Strip_n criado
const uint8_t StripPins[TotalStrips] = {
    Strip_0,
    Strip_1,
    Strip_2,
    Strip_3,
    Strip_4,
};
// const uint8_t StripPins[TotalStrips] = {Strip_0, Strip_1, Strip_2};

int defaultColor[TotalStrips][3] = {
    {255, 0, 0},
    {255, 165, 0},
    {0, 0, 255},
    {0, 255, 0},
    {173, 216, 230},
};

// A cada novo sensor adicionado deve ser adicionado nessa lista um novo NewPing(trigger, echo, maxDistance)
NewPing ultras[TotalStrips] = {
    NewPing(13, 15, 400),
    NewPing(12, 14, 400),
    NewPing(27, 26, 400),
    NewPing(25, 33, 400),
    NewPing(32, 35, 400),
};

// A cada novo sensor adicionado deve ser adicionado nesse array a distancia maxima em cm que esse sensor deve pegar
const int maxDistances[TotalStrips] = {
    50,
    50,
    50,
    50,
    50,
};

// Indica qual a distancia em cm que ira causar o acionar de uma fita led
int TriggerDistanceInCm = 5;

CRGB leds[TotalStrips][LedsByStrip];

void setPixel(int sonarIndex, int Pixel, byte red, byte green, byte blue)
{
  leds[sonarIndex][Pixel].r = red;
  leds[sonarIndex][Pixel].g = green;
  leds[sonarIndex][Pixel].b = blue;
}

void setAll(int sonarIndex, byte red, byte green, byte blue)
{
  for (int i = 0; i < LedsByStrip; i++)
  {
    setPixel(sonarIndex, i, red, green, blue);
  }
  FastLED.show();
}

void setup()
{
  Serial.begin(9600);
  delay(3000);

  FastLED.addLeds<WS2811, Strip_0, GRB>(leds[0], LedsByStrip).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2811, Strip_1, GRB>(leds[1], LedsByStrip).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2811, Strip_2, GRB>(leds[2], LedsByStrip).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2811, Strip_3, GRB>(leds[3], LedsByStrip).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2811, Strip_4, GRB>(leds[4], LedsByStrip).setCorrection(TypicalLEDStrip);

  // A cada nova fita de led deve ser criado uma nova entrada aqui de acordo com os valores já criados posteriormente
  // FastLED.addLeds<WS2811, Strip_5, GRB>(leds[5], LedsByStrip).setCorrection(TypicalLEDStrip);
}

bool triggered[TotalStrips] = {
    false,
    false,
    false,
    false,
    false,
};

void loop()
{
  for (int i = 0; i < TotalStrips; i++)
  {
    int distance = ultras[i].ping_cm();
    if (distance <= maxDistances[i] - TriggerDistanceInCm)
    {
      Serial.printf("Distance(%d): %dcm\n", i, distance);
      triggered[i] = true;
      continue;
    }

    triggered[i] = false;
  }

  // quantidade maxima de leds que ficarão iluminados
  int size = 10;
  int speedInMs = 5;

  for (int i = 0; i < LedsByStrip + size; i++)
  {
    unsigned long time_now = millis();
    for (int s = 0; s < TotalStrips; s++)
    {

      if (!triggered[s])
      {
        setAll(s, defaultColor[s][0], defaultColor[s][1], defaultColor[s][2]);
        continue;
      }
      if ((i - size) * 1 >= 0)
      {
        leds[s][i - size].setRGB(0, 0, 0);
      }

      if (i + 1 < LedsByStrip)
      {
        leds[s][i + 1].setRGB(defaultColor[s][0], defaultColor[s][1], defaultColor[s][2]);
      }
    }

    FastLED.show();
    while (millis() < time_now + speedInMs)
      ;
  }
}
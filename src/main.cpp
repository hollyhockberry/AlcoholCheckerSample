// Sample for M5CORE2 + MQ-3 (Alcohol sensor)
//
// Copyright (c) 2022 Inaba
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include <vector>

#define LGFX_AUTODETECT
#include "SD.h"
#include <M5Core2.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <LGFX_AUTODETECT.hpp>

static constexpr int PORT = 35;

static LGFX lcd;
static LGFX_Sprite graph;
static std::vector<int> points;
static int counter = 0;
static uint16_t l_threshold = 2000;
static constexpr int GRAPH_HEIGHT = 100;

int value(uint16_t v) {
  return ::map(v, 0, 4095, 0, GRAPH_HEIGHT);
}

void show_image(bool ok) {
  static bool curr_ok = false;
  if (curr_ok == ok) {
    return;
  }
  lcd.drawJpgFile(SD, ok ? "/ok.jpg" : "/ng.jpg", 93, 0);
  curr_ok = ok;
}

int get_color(int v) {
  return v < value(2000) ? TFT_WHITE : TFT_RED;
}

void drawFiled() {
  lcd.startWrite();
  lcd.fillRect(0, lcd.height() - GRAPH_HEIGHT,
               lcd.width(), GRAPH_HEIGHT, TFT_DARKGREEN);
  const auto th = ::value(l_threshold);
  lcd.fillRect(0, lcd.height() - GRAPH_HEIGHT,
               lcd.width(), GRAPH_HEIGHT - th, TFT_RED);
  lcd.endWrite();
}

void setup() {
  M5.begin();
  l_threshold = analogRead(PORT) + 100;

  lcd.init();
  lcd.setRotation(1);
  lcd.setColorDepth(24);
  lcd.clear(TFT_WHITE);
  points.resize(lcd.width());
  ::drawFiled();
  ::show_image(true);
}

int get(int index, int x) {
  return points[(index + points.size() + x) % points.size()];
}

void loop() {
  // read MQ-3
  const auto lv = ::analogRead(PORT);

  M5.update();
  if (M5.BtnA.wasPressed()) {
    // update threshold
    l_threshold = lv + 100;
    ::drawFiled();
  }

  ::show_image(lv < l_threshold);

  points[counter] = ::value(lv);
  counter = (counter + 1) % points.size();
  lcd.startWrite();
  for (auto x = 0; x < lcd.width(); ++x) {
    auto last = ::get(counter - 1, x);
    auto curr = ::get(counter, x);
    auto color = last < value(l_threshold) ? TFT_DARKGREEN : TFT_RED;
    lcd.writePixel(x, lcd.height() - last - 1, color);
    lcd.writePixel(x, lcd.height() - curr - 1, TFT_WHITE);
  }
  lcd.endWrite();
  ::delay(10);
}

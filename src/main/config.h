#pragma once
#include <Arduino.h>

namespace Config {
  // Exemplos: substitua pelos GPIOs escolhidos para sua placa.
  constexpr uint8_t PIN_BOTAO_1 = 18;
  constexpr uint8_t PIN_BOTAO_2 = 19;

  constexpr uint8_t MODO_BOTOES = INPUT_PULLUP;
  constexpr uint8_t NIVEL_PRESSIONADO = LOW;
  constexpr uint32_t DEBOUNCE_MS = 30;
}
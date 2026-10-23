#pragma once
#include <Arduino.h>

namespace Config {
  // Exemplos: substitua pelos GPIOs escolhidos para sua placa.
  constexpr uint8_t PIN_BOTAO = 18;
  constexpr uint8_t PIN_ENCODER_A = 19;
  constexpr uint8_t PIN_ENCODER_B = 21;

  constexpr uint8_t MODO_BOTOES = INPUT_PULLUP;
  constexpr uint8_t NIVEL_PRESSIONADO = LOW;
  constexpr uint32_t DEBOUNCE_MS = 30;

  constexpr int8_t PIN_TELA_CS = 5;
  constexpr int8_t PIN_TELA_DC = 27;
  constexpr int8_t PIN_TELA_RST = 26;

  constexpr int volumeSeginga = 5;
  constexpr float limiteInferiorVazao = 2.0f;
  constexpr float limiteSuperiorVazao = 750.0f;
  constexpr uint8_t limiteInferiorTempo = 0;
  constexpr uint8_t limiteSuperiorTempo = 61;
}

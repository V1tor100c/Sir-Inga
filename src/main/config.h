#pragma once
#include <Arduino.h>
#include <stdint.h>

namespace Config {

  //=-=-=-=-= BOTÃO -=-=-=-=-=
  constexpr uint8_t MODO_BOTOES = INPUT_PULLUP;
  constexpr uint8_t NIVEL_PRESSIONADO_BOTAO = LOW;
  constexpr uint32_t DEBOUNCE_MS = 30;

  //=-=-=-=-= FIM DE CURSO -=-=-=-=-=
  constexpr uint8_t PIN_FIM_SUPERIOR = 4;
  constexpr uint8_t PIN_FIM_INFERIOR = 2;

  constexpr uint8_t MODO_FIM = INPUT_PULLUP;
  constexpr uint8_t NIVEL_ATIVO_FIM = LOW;
  constexpr uint32_t DEBOUNCE_MS_FIM = 30;

  //=-=-=-=-= TELA -=-=-=-=-=
  constexpr int8_t PIN_TELA_CS  = 5;
  constexpr int8_t PIN_TELA_DC  = 27;
  constexpr int8_t PIN_TELA_RST = 26;

  //=-=-=-=-= SERINGA -=-=-=-=-=
  constexpr int volumeSeginga = 5;
  constexpr float limiteInferiorVazao   = 2.0f;
  constexpr float limiteSuperiorVazao   = 750.0f;
  constexpr uint8_t limiteInferiorTempo = 0;
  constexpr uint8_t limiteSuperiorTempo = 61;

  constexpr float volume_seringa_ml = 5.0f; // (ml)
  constexpr float area_seringa_mm   = 1.0f; // (mm)

  //=-=-=-=-= CALCULO VELOCIDADE -=-=-=-=-=
  constexpr float    fuso_avanco_mm_por_volta = 1.0f;   // fuso de 1 mm/volta
  constexpr uint16_t passos_por_volta_motor   = 200;    // NEMA17 1.8°
  constexpr uint16_t microsteps               = 16;     // configurado no driver

  //=-=-=-=-= MOTOR -=-=-=-=-=
  constexpr uint8_t resolucao_motor = 8;
  constexpr uint8_t PIN_MOTOR = 18;
  constexpr uint8_t LIGA_MOTOR = 127;
  constexpr uint8_t DESLIGA_MOTOR = 0;

  //=-=-=-=-= DRIVE -=-=-=-=-=
  constexpr uint8_t PIN_STEP = 19;
  constexpr uint8_t PIM_DIR = 20;
  constexpr uint8_t PIN_SLEEP = 21;
}

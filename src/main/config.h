#pragma once

#include <Arduino.h>
#include <stdint.h>

// Mapeamento dos pinos conforme o mapa de conexoes do Sir Inga.
namespace Config {

// Encoder EC11 usado para navegar na interface. Os resistores de pull-up
// estao na placa; GPIO36 e somente entrada.
constexpr uint8_t PIN_BOTAO = 36;
constexpr uint8_t PIN_BOTAO_ENCODER_A = 22;
constexpr uint8_t PIN_BOTAO_ENCODER_B = 21;
constexpr uint8_t MODO_BOTOES = INPUT;
constexpr uint8_t NIVEL_PRESSIONADO_BOTAO = LOW;
constexpr uint32_t DEBOUNCE_MS = 30;

// Fins de curso ENDSTOP1 e ENDSTOP2. Entradas externas com pull-up.
constexpr uint8_t PIN_FIM_SUPERIOR = 39;
constexpr uint8_t PIN_FIM_INFERIOR = 34;
constexpr uint8_t MODO_FIM = INPUT;
constexpr uint8_t NIVEL_ATIVO_FIM = LOW;
constexpr uint32_t DEBOUNCE_MS_FIM = 30;

// Display ILI9341 ligado ao barramento SPI VSPI padrao do ESP32.
constexpr uint8_t PIN_TELA_CS = 5;
constexpr uint8_t PIN_TELA_DC = 17;
constexpr uint8_t PIN_TELA_RST = 16;
constexpr uint8_t PIN_TELA_SCK = 18;
constexpr uint8_t PIN_TELA_MISO = 19;
constexpr uint8_t PIN_TELA_MOSI = 23;

// Driver DRV8825: STEP, DIR, nENBL e nFAULT.
constexpr uint8_t PIN_STEP = 25;
constexpr uint8_t PIN_DIR = 26;
constexpr uint8_t PIN_ENABLE_MOTOR_N = 27;
constexpr uint8_t PIN_FALHA_DRIVER_N = 33;
constexpr uint8_t NIVEL_MOTOR_HABILITADO = LOW;
constexpr uint8_t NIVEL_MOTOR_DESABILITADO = HIGH;
constexpr uint8_t NIVEL_FALHA_DRIVER = LOW;

constexpr uint8_t CANAL_MOTOR = 0;
constexpr uint8_t RESOLUCAO_MOTOR = 8;
constexpr uint8_t LIGA_MOTOR = 127;
constexpr uint8_t DESLIGA_MOTOR = 0;
constexpr uint8_t FREQUENCIA_MOTOR_VOLTANDO = 1;

// Encoder incremental AS5040 no eixo: canais A/B e indice.
constexpr uint8_t PIN_ENCODER_EIXO_A = 14;
constexpr uint8_t PIN_ENCODER_EIXO_B = 15;
constexpr uint8_t PIN_ENCODER_EIXO_INDEX = 13;

// Sinalizacao.
constexpr uint8_t PIN_BUZZER = 32;
constexpr uint8_t PIN_LED_STATUS_1 = 4;
constexpr uint8_t PIN_LED_STATUS_2 = 2;

// Seringa e parametros usados pelo calculo da velocidade (mantidos do projeto).
constexpr int volumeSeginga = 5;
constexpr float limiteInferiorVazao = 2.0f;
constexpr float limiteSuperiorVazao = 750.0f;
constexpr uint8_t limiteInferiorTempo = 0;
constexpr uint8_t limiteSuperiorTempo = 61;
constexpr float volume_seringa_ml = 5.0f;
constexpr float area_seringa_mm = 1.0f;
constexpr float fuso_avanco_mm_por_volta = 1.0f;
constexpr uint16_t passos_por_volta_motor = 200;
// Ajustar para o microstep selecionado fisicamente nos jumpers H2 do DRV8825.
constexpr uint16_t microsteps = 16;

} // namespace Config

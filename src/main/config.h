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
constexpr uint8_t MODO_FALHA_DRIVER = INPUT_PULLUP;

constexpr uint8_t CANAL_MOTOR = 0;
constexpr uint8_t RESOLUCAO_MOTOR = 12;
constexpr uint16_t DUTY_MOTOR_PASSO = 1 << (RESOLUCAO_MOTOR - 1); // 50%
constexpr uint32_t FREQUENCIA_PASSO_MIN_HZ = 20;
constexpr uint32_t FREQUENCIA_PASSO_MAX_HZ = 10000;
constexpr uint32_t RAMPA_PASSO_HZ_POR_SEGUNDO = 1000;

// Encoder incremental AS5040 no eixo: canais A/B e indice.
constexpr uint8_t PIN_ENCODER_EIXO_A = 14;
constexpr uint8_t PIN_ENCODER_EIXO_B = 15;
constexpr uint8_t PIN_ENCODER_EIXO_INDEX = 13;
// Placeholder: confirmar quantas transicoes A/B a classe conta por volta.
constexpr uint32_t TRANSICOES_ENCODER_POR_VOLTA = 4096;
// Ajustar na bancada para que uma contagem positiva corresponda ao avanco do embolo.
// Use +1 ou -1 conforme a orientacao eletrica/mecanica do encoder.
constexpr int8_t SINAL_ENCODER_AVANCO = 1;

// Controle e tarefas FreeRTOS. Valores iniciais para prototipo de bancada.
constexpr uint32_t PERIODO_TAREFA_CONTROLE_MS = 10;
constexpr uint32_t PERIODO_MEDICAO_VELOCIDADE_MS = 100;
constexpr uint32_t TEMPO_MAXIMO_PARTIDA_MS = 3000;
constexpr uint32_t TEMPO_MAXIMO_SEM_MOVIMENTO_MS = 1000;
constexpr float GANHO_PI_KP = 40.0f; // Hz por RPM de erro; precisa ser ajustado
constexpr float GANHO_PI_KI = 20.0f; // Hz por (RPM*s) de erro; precisa ser ajustado

// Sinalizacao.
constexpr uint8_t PIN_BUZZER = 32;
constexpr uint8_t PIN_LED_STATUS_1 = 4;
constexpr uint8_t PIN_LED_STATUS_2 = 2;

// PLACEHOLDERS MECANICOS: valores ficticios para estruturar o firmware.
// Substituir e calibrar quando seringa, fuso, microstepping e encoder forem definidos.
constexpr float volume_seringa_ml = 10.0f;
constexpr float area_seringa_mm2 = 80.0f;
constexpr float fuso_avanco_mm_por_volta = 1.0f;
constexpr uint16_t passos_por_volta_motor = 200;
constexpr uint16_t microsteps = 16;

// Interface: vazao em mL/h e duracao em minutos.
constexpr float limiteInferiorVazao = 2.0f;
constexpr float limiteSuperiorVazao = 750.0f;
constexpr uint8_t limiteInferiorTempoMin = 1;
constexpr uint8_t limiteSuperiorTempoMin = 60;

// Ganhos e geometria acima sao placeholders de software, nao valores calibrados.

} // namespace Config

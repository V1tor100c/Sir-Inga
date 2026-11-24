#pragma once

#include "config.h"
#include <stdint.h>

// Leitura do encoder incremental instalado no eixo (canais A/B e indice).
// A contagem aumenta ou diminui de acordo com o sentido de rotacao.
class EncoderEixo {
 public:
  explicit EncoderEixo(
      uint8_t pinoA = Config::PIN_ENCODER_EIXO_A,
      uint8_t pinoB = Config::PIN_ENCODER_EIXO_B,
      uint8_t pinoIndice = Config::PIN_ENCODER_EIXO_INDEX);

  // Inicializa os pinos e ativa interrupcoes nos tres sinais.
  void iniciar();

  // Retorna a contagem acumulada de transicoes A/B (com sinal).
  int32_t lerContagem() const;

  // Retorna true uma vez para cada pulso de indice recebido desde a ultima leitura.
  bool lerIndice();

  EncoderEixo(const EncoderEixo &) = delete;
  EncoderEixo &operator=(const EncoderEixo &) = delete;

 private:
  static void IRAM_ATTR interrupcaoAB();
  static void IRAM_ATTR interrupcaoIndice();

  void IRAM_ATTR atualizarContagem();

  uint8_t pinoA_;
  uint8_t pinoB_;
  uint8_t pinoIndice_;
  volatile int32_t contagem_ = 0;
  volatile bool indiceDetectado_ = false;
  volatile uint8_t estadoAnterior_ = 0;

  // Este projeto usa um unico encoder de eixo.
  static EncoderEixo *instancia_;
};

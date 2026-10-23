#pragma once

#include "config.h"
#include <stdint.h>

// Evento emitido uma vez quando o estado eletrico do botao estabiliza.
enum class EventoBotao : uint8_t {
  Nenhum,
  Pressionado,
  Solto,
  Horario,
  AntiHorario,
  SemGirar
};

class Botao {
 public:
  // modoPino e nivelPressionado dependem de como o botao foi ligado.
  // Com INPUT_PULLUP, normalmente nivelPressionado e LOW.
  explicit Botao(uint8_t pino_botao,
                 uint8_t pino_encoderA,
                 uint8_t pino_encoderB,
                 uint8_t modoPino = INPUT_PULLUP,
                 uint8_t nivelPressionado = LOW,
                 uint32_t debounceMs = 30);

  void iniciar();

  EventoBotao atualizar(uint32_t agoraMs = millis());

 private:
  uint8_t pino_;
  uint8_t pino_encoderA_;
  uint8_t pino_encoderB_;
  uint8_t modoPino_;
  uint8_t nivelPressionado_;
  uint32_t debounceMs_;
  uint32_t instanteUltimaMudanca_ = 0;
  uint8_t leituraAnterior_Botao = HIGH;
  uint8_t estadoEstavel_ = HIGH;
  bool iniciado_ = false;

  int estadoAnterior_Encoder = 0;
  int estadoAtual_Encoder = 0;
  int transicoesEncoder_ = 0;

  int lerEncoder();
  EventoBotao lerRotacao();
};

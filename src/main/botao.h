#pragma once

#include "config.h"
#include <stdint.h>

// Evento emitido uma vez quando o estado eletrico do botao estabiliza.
enum class EventoBotao : uint8_t {
  Nenhum,
  Pressionado,
  Solto
};

class Botao {
 public:
  // modoPino e nivelPressionado dependem de como o botao foi ligado.
  // Com INPUT_PULLUP, normalmente nivelPressionado e LOW.
  explicit Botao(uint8_t pino,
                 uint8_t modoPino = INPUT_PULLUP,
                 uint8_t nivelPressionado = LOW,
                 uint32_t debounceMs = 30);

  void iniciar();

  // Chamar frequentemente no loop(). Nao usa delay().
  EventoBotao atualizar(uint32_t agoraMs = millis());

 private:
  uint8_t pino_;
  uint8_t modoPino_;
  uint8_t nivelPressionado_;
  uint32_t debounceMs_;
  uint32_t instanteUltimaMudanca_ = 0;
  uint8_t leituraAnterior_ = HIGH;
  uint8_t estadoEstavel_ = HIGH;
  bool iniciado_ = false;
};

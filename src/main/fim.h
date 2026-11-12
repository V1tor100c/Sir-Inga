#pragma once

#include "config.h"
#include <stdint.h>

enum class EventoFim : uint8_t {
  Nenhum,
  Acionado,
  Liberado
};

class Fim {
 public:

  explicit Fim(uint8_t pino,
               uint8_t modoPino = INPUT_PULLUP,
               uint8_t nivelAtivo = LOW,
               uint32_t debounceMs = 30);

  void iniciar();

  bool ler() const;

  // Emite Acionado/Liberado uma vez quando a nova leitura estabiliza.
  EventoFim atualizar(uint32_t agoraMs = millis());

 private:
  uint8_t pino_;
  uint8_t modoPino_;
  uint8_t nivelAtivo_;
  uint32_t debounceMs_;
  uint32_t instanteUltimaMudanca_ = 0;
  uint8_t leituraAnterior_ = HIGH;
  uint8_t estadoEstavel_ = HIGH;
  bool iniciado_ = false;
};

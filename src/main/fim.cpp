#include "fim.h"

Fim::Fim(uint8_t pino,
         uint8_t modoPino,
         uint8_t nivelAtivo,
         uint32_t debounceMs)
    : pino_(pino),
      modoPino_(modoPino),
      nivelAtivo_(nivelAtivo),
      debounceMs_(debounceMs) {}

void Fim::iniciar() {
  pinMode(pino_, modoPino_);

  // Usa o nivel atual como estado inicial, sem inventar uma transicao.
  leituraAnterior_ = digitalRead(pino_);
  estadoEstavel_ = leituraAnterior_;
  instanteUltimaMudanca_ = millis();
  iniciado_ = true;
}

bool Fim::ler() const {
  return digitalRead(pino_) == nivelAtivo_;
}

EventoFim Fim::atualizar(uint32_t agoraMs) {
  if (!iniciado_) {
    return EventoFim::Nenhum;
  }

  const uint8_t leituraAtual = digitalRead(pino_);
  if (leituraAtual != leituraAnterior_) {
    leituraAnterior_ = leituraAtual;
    instanteUltimaMudanca_ = agoraMs;
  }

  // A subtracao sem sinal continua funcionando quando millis() retorna a zero.
  if ((uint32_t)(agoraMs - instanteUltimaMudanca_) < debounceMs_ ||
      leituraAtual == estadoEstavel_) {
    return EventoFim::Nenhum;
  }

  estadoEstavel_ = leituraAtual;
  return estadoEstavel_ == nivelAtivo_
             ? EventoFim::Acionado
             : EventoFim::Liberado;
}

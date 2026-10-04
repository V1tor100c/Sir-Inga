#include "botao.h"

Botao::Botao(uint8_t pino,
             uint8_t modoPino,
             uint8_t nivelPressionado,
             uint32_t debounceMs)
    : pino_(pino),
      modoPino_(modoPino),
      nivelPressionado_(nivelPressionado),
      debounceMs_(debounceMs) {}

void Botao::iniciar() {
  pinMode(pino_, modoPino_);

  // Assume o nivel atual como inicial; nao gera clique falso ao ligar.
  leituraAnterior_ = digitalRead(pino_);
  estadoEstavel_ = leituraAnterior_;
  instanteUltimaMudanca_ = millis();
  iniciado_ = true;
}

EventoBotao Botao::atualizar(uint32_t agoraMs) {
  if (!iniciado_) {
    return EventoBotao::Nenhum;
  }

  const uint8_t leituraAtual = digitalRead(pino_);
  if (leituraAtual != leituraAnterior_) {
    leituraAnterior_ = leituraAtual;
    instanteUltimaMudanca_ = agoraMs;
  }

  // Subtracao sem sinal continua correta quando millis() retorna a zero.
  if ((uint32_t)(agoraMs - instanteUltimaMudanca_) < debounceMs_ ||
      leituraAtual == estadoEstavel_) {
    return EventoBotao::Nenhum;
  }

  estadoEstavel_ = leituraAtual;
  return estadoEstavel_ == nivelPressionado_
             ? EventoBotao::Pressionado
             : EventoBotao::Solto;
}

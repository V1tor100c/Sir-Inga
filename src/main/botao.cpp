#include "botao.h"

Botao::Botao(uint8_t pino,
             uint8_t pino_encoderA,
             uint8_t pino_encoderB,
             uint8_t modoPino,
             uint8_t nivelPressionado,
             uint32_t debounceMs)
    : pino_(pino),
      pino_encoderA_(pino_encoderA),
      pino_encoderB_(pino_encoderB),
      modoPino_(modoPino),
      nivelPressionado_(nivelPressionado),
      debounceMs_(debounceMs) {}

void Botao::iniciar() {
  pinMode(pino_, modoPino_);
  pinMode(pino_encoderA_, INPUT_PULLUP);
  pinMode(pino_encoderB_, INPUT_PULLUP);

  // Assume o nivel atual como inicial; nao gera clique falso ao ligar.
  leituraAnterior_Botao = digitalRead(pino_);
  estadoEstavel_ = leituraAnterior_Botao;
  instanteUltimaMudanca_ = millis();
  iniciado_ = true;

  estadoAnterior_Encoder = lerEncoder();
}

EventoBotao Botao::atualizar(uint32_t agoraMs) {
  if (!iniciado_) {
    return EventoBotao::Nenhum;
  }

  const EventoBotao eventoRotacao = lerRotacao();
  if (eventoRotacao != EventoBotao::SemGirar) {
    return eventoRotacao;
  }

  const uint8_t leituraAtual = digitalRead(pino_);
  if (leituraAtual != leituraAnterior_Botao) {
    leituraAnterior_Botao = leituraAtual;
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

int Botao::lerEncoder(){
  int A = digitalRead(pino_encoderA_);
  int B = digitalRead(pino_encoderB_);
  return (A << 1) | B;
}

EventoBotao Botao::lerRotacao(){
  static const int8_t tabelaTransicoes[16] = {
     0, -1,  1,  0,
     1,  0,  0, -1,
    -1,  0,  0,  1,
     0,  1, -1,  0
  };

  estadoAtual_Encoder = lerEncoder();
  const uint8_t indice = (estadoAnterior_Encoder << 2) | estadoAtual_Encoder;
  transicoesEncoder_ += tabelaTransicoes[indice];
  estadoAnterior_Encoder = estadoAtual_Encoder;

  if (transicoesEncoder_ >= 4) {
    transicoesEncoder_ = 0;
    return EventoBotao::Horario;
  }
  if (transicoesEncoder_ <= -4) {
    transicoesEncoder_ = 0;
    return EventoBotao::AntiHorario;
  }

  return EventoBotao::SemGirar;
}

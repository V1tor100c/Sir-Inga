#include "encoder.h"

EncoderEixo *EncoderEixo::instancia_ = nullptr;

EncoderEixo::EncoderEixo(uint8_t pinoA, uint8_t pinoB, uint8_t pinoIndice)
    : pinoA_(pinoA), pinoB_(pinoB), pinoIndice_(pinoIndice) {}

void EncoderEixo::iniciar() {
  pinMode(pinoA_, INPUT);
  pinMode(pinoB_, INPUT);
  pinMode(pinoIndice_, INPUT);

  // So existe uma instancia deste encoder, portanto as interrupcoes estaticas
  // conseguem encaminhar a leitura para o objeto correto.
  instancia_ = this;
  estadoAnterior_ = (digitalRead(pinoA_) << 1) | digitalRead(pinoB_);
  contagem_ = 0;
  indiceDetectado_ = false;

  attachInterrupt(digitalPinToInterrupt(pinoA_), interrupcaoAB, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinoB_), interrupcaoAB, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pinoIndice_), interrupcaoIndice, RISING);
}

int32_t EncoderEixo::lerContagem() const {
  noInterrupts();
  const int32_t contagem = contagem_;
  interrupts();
  return contagem;
}

bool EncoderEixo::lerIndice() {
  noInterrupts();
  const bool detectado = indiceDetectado_;
  indiceDetectado_ = false;
  interrupts();
  return detectado;
}

void IRAM_ATTR EncoderEixo::interrupcaoAB() {
  if (instancia_ != nullptr) {
    instancia_->atualizarContagem();
  }
}

void IRAM_ATTR EncoderEixo::interrupcaoIndice() {
  if (instancia_ != nullptr) {
    instancia_->indiceDetectado_ = true;
  }
}

void IRAM_ATTR EncoderEixo::atualizarContagem() {
  const uint8_t estadoAtual = (digitalRead(pinoA_) << 1) | digitalRead(pinoB_);
  const uint8_t estadoAnterior = estadoAnterior_;

  if (estadoAtual != estadoAnterior) {
    const bool aAnterior = (estadoAnterior & 0b10) != 0;
    const bool bAnterior = (estadoAnterior & 0b01) != 0;
    const bool aAtual = (estadoAtual & 0b10) != 0;
    const bool bAtual = (estadoAtual & 0b01) != 0;

    // Uma mudanca valida altera apenas A ou B. A sequencia Gray determina
    // o sentido; mudancas simultaneas geralmente indicam uma leitura perdida.
    if (aAnterior != aAtual && bAnterior == bAtual) {
      contagem_ += (aAnterior != bAnterior) ? 1 : -1;
    } else if (bAnterior != bAtual && aAnterior == aAtual) {
      contagem_ += (aAnterior == bAnterior) ? 1 : -1;
    }
  }

  estadoAnterior_ = estadoAtual;
}

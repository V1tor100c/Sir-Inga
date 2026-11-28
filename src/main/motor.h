#pragma once

#include "config.h"

class Motor {
 public:
  explicit Motor(uint8_t pinoStep, uint8_t resolucao);

  // Configura LEDC e deixa o driver desabilitado, sem pulsos STEP.
  bool iniciar();

  // A frequencia e expressa em pulsos STEP por segundo (passos/s).
  bool definirVelocidade(uint32_t frequenciaPassosHz, bool sentidoHorario = true);
  void parar();
  bool falhaDriverAtiva() const;
  bool habilitado() const { return habilitado_; }

 private:
  uint8_t pinoStep_;
  uint8_t resolucao_;
  uint32_t frequenciaAtualHz_ = 0;
  bool inicializado_ = false;
  bool habilitado_ = false;
};

#pragma once

#include <stdint.h>

// PI de velocidade: o valor de saida e frequencia de pulsos STEP em Hz.
class ControladorPI {
 public:
  ControladorPI(float kp, float ki, uint32_t saidaMinima, uint32_t saidaMaxima);

  void reiniciar();
  uint32_t atualizar(float referenciaFeedForwardHz,
                     float erroVelocidadeRpm,
                     float intervaloSegundos);

 private:
  float kp_;
  float ki_;
  float integrador_ = 0.0f;
  uint32_t saidaMinima_;
  uint32_t saidaMaxima_;
};

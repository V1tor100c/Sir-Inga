#include "controlador_pi.h"

#include <math.h>

ControladorPI::ControladorPI(float kp,
                             float ki,
                             uint32_t saidaMinima,
                             uint32_t saidaMaxima)
    : kp_(kp),
      ki_(ki),
      saidaMinima_(saidaMinima),
      saidaMaxima_(saidaMaxima) {}

void ControladorPI::reiniciar() {
  integrador_ = 0.0f;
}

uint32_t ControladorPI::atualizar(float referenciaFeedForwardHz,
                                  float erroVelocidadeRpm,
                                  float intervaloSegundos) {
  if (intervaloSegundos <= 0.0f || !isfinite(erroVelocidadeRpm)) {
    return saidaMinima_;
  }

  const float integradorTentativo =
      integrador_ + ki_ * erroVelocidadeRpm * intervaloSegundos;
  const float saidaTentativa = referenciaFeedForwardHz +
                               kp_ * erroVelocidadeRpm +
                               integradorTentativo;

  // Anti-windup: nao acumula integral enquanto o erro empurra mais para fora
  // de um limite de frequencia ja atingido.
  const bool saturandoAcima = saidaTentativa > saidaMaxima_ && erroVelocidadeRpm > 0.0f;
  const bool saturandoAbaixo = saidaTentativa < saidaMinima_ && erroVelocidadeRpm < 0.0f;
  if (!saturandoAcima && !saturandoAbaixo) {
    integrador_ = integradorTentativo;
  }

  const float saida = referenciaFeedForwardHz +
                      kp_ * erroVelocidadeRpm + integrador_;
  if (!isfinite(saida) || saida <= saidaMinima_) {
    return saidaMinima_;
  }
  if (saida >= saidaMaxima_) {
    return saidaMaxima_;
  }
  return static_cast<uint32_t>(saida + 0.5f);
}

#include "controle_motor.h"

#include "controlador_pi.h"

#include <math.h>

volatile bool ControleMotor::fimDeCursoInterrompeu_ = false;

namespace {
ControladorPI controladorPI(Config::GANHO_PI_KP,
                            Config::GANHO_PI_KI,
                            Config::FREQUENCIA_PASSO_MIN_HZ,
                            Config::FREQUENCIA_PASSO_MAX_HZ);
}

ControleMotor::ControleMotor(Motor &motor,
                             EncoderEixo &encoder,
                             Fim &fimSuperior,
                             Fim &fimInferior)
    : motor_(motor),
      encoder_(encoder),
      fimSuperior_(fimSuperior),
      fimInferior_(fimInferior) {}

bool ControleMotor::iniciarTarefa(QueueHandle_t filaComandos,
                                  QueueHandle_t filaEstado,
                                  BaseType_t nucleo) {
  if (filaComandos == nullptr || filaEstado == nullptr) {
    return false;
  }

  filaComandos_ = filaComandos;
  filaEstado_ = filaEstado;

  attachInterrupt(digitalPinToInterrupt(Config::PIN_FIM_SUPERIOR),
                  interrupcaoFimDeCurso, CHANGE);
  attachInterrupt(digitalPinToInterrupt(Config::PIN_FIM_INFERIOR),
                  interrupcaoFimDeCurso, CHANGE);

  return xTaskCreatePinnedToCore(tarefa,
                                 "controle_motor",
                                 4096,
                                 this,
                                 4,
                                 &tarefa_,
                                 nucleo) == pdPASS;
}

void ControleMotor::tarefa(void *contexto) {
  static_cast<ControleMotor *>(contexto)->executar();
}

void IRAM_ATTR ControleMotor::interrupcaoFimDeCurso() {
  fimDeCursoInterrompeu_ = true;
}

float ControleMotor::calcularRpmAlvo(float vazaoMlPorHora) const {
  const float volumePorVoltaMl =
      Config::area_seringa_mm2 * Config::fuso_avanco_mm_por_volta / 1000.0f;
  if (volumePorVoltaMl <= 0.0f) {
    return 0.0f;
  }

  // RPM = (mL/h) / (mL/volta) / 60 min/h.
  return vazaoMlPorHora / (volumePorVoltaMl * 60.0f);
}

float ControleMotor::calcularVazaoEstimada(float rpm) const {
  const float volumePorVoltaMl =
      Config::area_seringa_mm2 * Config::fuso_avanco_mm_por_volta / 1000.0f;
  return rpm * volumePorVoltaMl * 60.0f;
}

void ControleMotor::processarComando(const ComandoMotor &comando,
                                     uint32_t agoraMs) {
  switch (comando.tipo) {
  case TipoComandoMotor::Iniciar: {
    const float volumeComandadoMl = comando.vazaoMlPorHora *
                                    comando.duracaoSegundos / 3600.0f;
    if (!isfinite(comando.vazaoMlPorHora) ||
        comando.vazaoMlPorHora < Config::limiteInferiorVazao ||
        comando.vazaoMlPorHora > Config::limiteSuperiorVazao ||
        comando.duracaoSegundos <
            static_cast<uint32_t>(Config::limiteInferiorTempoMin) * 60UL ||
        comando.duracaoSegundos >
            static_cast<uint32_t>(Config::limiteSuperiorTempoMin) * 60UL ||
        volumeComandadoMl > Config::volume_seringa_ml ||
        Config::TRANSICOES_ENCODER_POR_VOLTA == 0 ||
        Config::passos_por_volta_motor == 0 || Config::microsteps == 0 ||
        (Config::SINAL_ENCODER_AVANCO != 1 &&
         Config::SINAL_ENCODER_AVANCO != -1)) {
      registrarFalha(FalhaMotor::ParametrosInvalidos);
      return;
    }

    if (estado_.falha != FalhaMotor::Nenhuma ||
        motor_.falhaDriverAtiva() || fimSuperior_.ler() || fimInferior_.ler() ||
        comando.vazaoMlPorHora <= 0.0f || comando.duracaoSegundos == 0) {
      if (motor_.falhaDriverAtiva()) {
        registrarFalha(FalhaMotor::Driver);
      } else if (fimSuperior_.ler() || fimInferior_.ler()) {
        registrarFalha(FalhaMotor::FimDeCurso);
      }
      return;
    }

    rpmAlvo_ = calcularRpmAlvo(comando.vazaoMlPorHora);
    const float passosPorVolta = static_cast<float>(Config::passos_por_volta_motor) *
                                 Config::microsteps;
    const float frequenciaFeedForward = rpmAlvo_ * passosPorVolta / 60.0f;
    if (!isfinite(rpmAlvo_) || rpmAlvo_ <= 0.0f ||
        frequenciaFeedForward > Config::FREQUENCIA_PASSO_MAX_HZ ||
        frequenciaFeedForward < Config::FREQUENCIA_PASSO_MIN_HZ) {
      registrarFalha(FalhaMotor::FrequenciaInvalida);
      return;
    }

    motor_.parar();
    controladorPI.reiniciar();
    frequenciaComandadaHz_ = 0;
    duracaoMs_ = comando.duracaoSegundos * 1000UL;
    instanteInicioMs_ = agoraMs;
    instanteUltimoMovimentoMs_ = agoraMs;
    acumuladoMedicaoMs_ = 0;
    contagemAnterior_ = encoder_.lerContagem();
    movimentoDetectado_ = false;
    fimDeCursoInterrompeu_ = false;

    estado_.executando = true;
    estado_.concluido = false;
    estado_.vazaoAlvoMlPorHora = comando.vazaoMlPorHora;
    estado_.vazaoEstimadaMlPorHora = 0.0f;
    estado_.rpmMedido = 0.0f;
    estado_.frequenciaPassosHz = 0;
    estado_.contagemEncoder = contagemAnterior_;
    break;
  }

  case TipoComandoMotor::Parar:
    motor_.parar();
    estado_.executando = false;
    estado_.concluido = false;
    frequenciaComandadaHz_ = 0;
    controladorPI.reiniciar();
    break;

  case TipoComandoMotor::ReconhecerFalha:
    motor_.parar();
    if (!motor_.falhaDriverAtiva() && !fimSuperior_.ler() &&
        !fimInferior_.ler()) {
      estado_.falha = FalhaMotor::Nenhuma;
      estado_.concluido = false;
      fimDeCursoInterrompeu_ = false;
    }
    break;
  }
}

void ControleMotor::registrarFalha(FalhaMotor falha) {
  motor_.parar();
  controladorPI.reiniciar();
  frequenciaComandadaHz_ = 0;
  estado_.executando = false;
  estado_.concluido = false;
  estado_.falha = falha;
  estado_.frequenciaPassosHz = 0;
}

void ControleMotor::publicarEstado() {
  if (filaEstado_ != nullptr) {
    xQueueOverwrite(filaEstado_, &estado_);
  }
}

void ControleMotor::executar() {
  const TickType_t periodoTicks = pdMS_TO_TICKS(Config::PERIODO_TAREFA_CONTROLE_MS);
  TickType_t instanteDespertar = xTaskGetTickCount();
  uint32_t instanteUltimoCicloMs = millis();
  for (;;) {
    ComandoMotor comando;
    while (xQueueReceive(filaComandos_, &comando, 0) == pdTRUE) {
      processarComando(comando, millis());
    }

    const uint32_t agoraMs = millis();
    const uint32_t dtCicloMs = agoraMs - instanteUltimoCicloMs;
    instanteUltimoCicloMs = agoraMs;

    if (motor_.falhaDriverAtiva() && estado_.falha == FalhaMotor::Nenhuma) {
      registrarFalha(FalhaMotor::Driver);
    }

    if (estado_.executando &&
        (fimDeCursoInterrompeu_ || fimSuperior_.ler() || fimInferior_.ler())) {
      registrarFalha(FalhaMotor::FimDeCurso);
    }

    if (estado_.executando && (uint32_t)(agoraMs - instanteInicioMs_) >= duracaoMs_) {
      motor_.parar();
      estado_.executando = false;
      estado_.concluido = true;
      frequenciaComandadaHz_ = 0;
      controladorPI.reiniciar();
    }

    if (estado_.executando) {
      acumuladoMedicaoMs_ += dtCicloMs;
    }
    if (estado_.executando &&
        acumuladoMedicaoMs_ >= Config::PERIODO_MEDICAO_VELOCIDADE_MS) {
      const float dtSegundos = acumuladoMedicaoMs_ / 1000.0f;
      acumuladoMedicaoMs_ = 0;

      const int32_t contagemAtual = encoder_.lerContagem();
      const int32_t diferencaContagem = contagemAtual - contagemAnterior_;
      contagemAnterior_ = contagemAtual;
      estado_.contagemEncoder = contagemAtual;

      const float contagensNoSentidoAvanco =
          static_cast<float>(diferencaContagem) * Config::SINAL_ENCODER_AVANCO;
      const float rpmMedido = contagensNoSentidoAvanco * 60.0f /
                              (Config::TRANSICOES_ENCODER_POR_VOLTA * dtSegundos);
      estado_.rpmMedido = rpmMedido;
      estado_.vazaoEstimadaMlPorHora = calcularVazaoEstimada(rpmMedido);

      if (contagensNoSentidoAvanco < 0.0f) {
        registrarFalha(FalhaMotor::SentidoIncorreto);
      } else if (contagensNoSentidoAvanco > 0.0f) {
        movimentoDetectado_ = true;
        instanteUltimoMovimentoMs_ = agoraMs;
      }

      const uint32_t limiteSemMovimentoMs = movimentoDetectado_
          ? Config::TEMPO_MAXIMO_SEM_MOVIMENTO_MS
          : Config::TEMPO_MAXIMO_PARTIDA_MS;
      if (estado_.executando &&
          (uint32_t)(agoraMs - instanteUltimoMovimentoMs_) >= limiteSemMovimentoMs) {
        registrarFalha(FalhaMotor::SemMovimento);
      } else if (estado_.executando) {
        const float erroRpm = rpmAlvo_ - rpmMedido;
        const float passosPorVolta = static_cast<float>(Config::passos_por_volta_motor) *
                                     Config::microsteps;
        const float feedForwardHz = rpmAlvo_ * passosPorVolta / 60.0f;
        const uint32_t frequenciaCalculadaHz = controladorPI.atualizar(
            feedForwardHz, erroRpm, dtSegundos);

        const float variacaoMaxima =
            Config::RAMPA_PASSO_HZ_POR_SEGUNDO * dtSegundos;
        float frequenciaComRampa = frequenciaCalculadaHz;
        if (frequenciaComandadaHz_ == 0) {
          frequenciaComRampa = fminf(frequenciaCalculadaHz, variacaoMaxima);
        } else if (frequenciaCalculadaHz > frequenciaComandadaHz_ + variacaoMaxima) {
          frequenciaComRampa = frequenciaComandadaHz_ + variacaoMaxima;
        } else if (frequenciaCalculadaHz + variacaoMaxima < frequenciaComandadaHz_) {
          frequenciaComRampa = frequenciaComandadaHz_ - variacaoMaxima;
        }
        frequenciaComandadaHz_ = static_cast<uint32_t>(fmaxf(
            Config::FREQUENCIA_PASSO_MIN_HZ,
            fminf(frequenciaComRampa, Config::FREQUENCIA_PASSO_MAX_HZ)));

        if (!motor_.definirVelocidade(frequenciaComandadaHz_)) {
          registrarFalha(FalhaMotor::FrequenciaInvalida);
        } else {
          estado_.frequenciaPassosHz = frequenciaComandadaHz_;
        }
      }
    }

    publicarEstado();
    vTaskDelayUntil(&instanteDespertar, periodoTicks);
  }
}

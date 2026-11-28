#pragma once

#include "config.h"
#include "encoder.h"
#include "fim.h"
#include "motor.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

enum class TipoComandoMotor : uint8_t {
  Iniciar,
  Parar,
  ReconhecerFalha
};

struct ComandoMotor {
  TipoComandoMotor tipo = TipoComandoMotor::Parar;
  float vazaoMlPorHora = 0.0f;
  uint32_t duracaoSegundos = 0;
};

enum class FalhaMotor : uint8_t {
  Nenhuma,
  Driver,
  FimDeCurso,
  SemMovimento,
  SentidoIncorreto,
  FrequenciaInvalida,
  ParametrosInvalidos
};

struct EstadoControleMotor {
  bool executando = false;
  bool concluido = false;
  FalhaMotor falha = FalhaMotor::Nenhuma;
  float vazaoAlvoMlPorHora = 0.0f;
  float vazaoEstimadaMlPorHora = 0.0f;
  float rpmMedido = 0.0f;
  uint32_t frequenciaPassosHz = 0;
  int32_t contagemEncoder = 0;
};

// Esta classe e dona do motor, encoder e fins de curso enquanto a tarefa roda.
class ControleMotor {
 public:
  ControleMotor(Motor &motor,
                EncoderEixo &encoder,
                Fim &fimSuperior,
                Fim &fimInferior);

  bool iniciarTarefa(QueueHandle_t filaComandos,
                     QueueHandle_t filaEstado,
                     BaseType_t nucleo);

 private:
  static void tarefa(void *contexto);
  static void IRAM_ATTR interrupcaoFimDeCurso();
  void executar();
  void processarComando(const ComandoMotor &comando, uint32_t agoraMs);
  void registrarFalha(FalhaMotor falha);
  void publicarEstado();
  float calcularRpmAlvo(float vazaoMlPorHora) const;
  float calcularVazaoEstimada(float rpm) const;

  Motor &motor_;
  EncoderEixo &encoder_;
  Fim &fimSuperior_;
  Fim &fimInferior_;
  QueueHandle_t filaComandos_ = nullptr;
  QueueHandle_t filaEstado_ = nullptr;
  TaskHandle_t tarefa_ = nullptr;

  EstadoControleMotor estado_;
  float rpmAlvo_ = 0.0f;
  uint32_t duracaoMs_ = 0;
  uint32_t instanteInicioMs_ = 0;
  uint32_t instanteUltimoMovimentoMs_ = 0;
  uint32_t acumuladoMedicaoMs_ = 0;
  int32_t contagemAnterior_ = 0;
  bool movimentoDetectado_ = false;
  uint32_t frequenciaComandadaHz_ = 0;

  static volatile bool fimDeCursoInterrompeu_;
};

#include "botao.h"
#include "buzzer.h"
#include "config.h"
#include "controle_motor.h"
#include "encoder.h"
#include "fim.h"
#include "motor.h"
#include "tela.h"

#include <Adafruit_ILI9341.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

namespace {

enum class EstadoInterface : uint8_t {
  Inicio,
  ConfigurarVazao,
  ConfigurarTempo,
  Confirmar,
  ConfiguracaoInvalida,
  Execucao,
  Falha,
  Concluido
};

Botao botao(Config::PIN_BOTAO,
            Config::PIN_BOTAO_ENCODER_A,
            Config::PIN_BOTAO_ENCODER_B,
            Config::MODO_BOTOES,
            Config::NIVEL_PRESSIONADO_BOTAO,
            Config::DEBOUNCE_MS);

Adafruit_ILI9341 display(Config::PIN_TELA_CS,
                         Config::PIN_TELA_DC,
                         Config::PIN_TELA_RST);
Tela tela(display);

Fim fimSuperior(Config::PIN_FIM_SUPERIOR,
                Config::MODO_FIM,
                Config::NIVEL_ATIVO_FIM,
                Config::DEBOUNCE_MS_FIM);
Fim fimInferior(Config::PIN_FIM_INFERIOR,
                Config::MODO_FIM,
                Config::NIVEL_ATIVO_FIM,
                Config::DEBOUNCE_MS_FIM);

Motor motor(Config::PIN_STEP, Config::RESOLUCAO_MOTOR);
EncoderEixo encoderEixo;
Buzzer buzzer(Config::PIN_BUZZER);
ControleMotor controleMotor(motor, encoderEixo, fimSuperior, fimInferior);

QueueHandle_t filaComandosMotor = nullptr;
QueueHandle_t filaEstadoMotor = nullptr;

bool enviarComando(const ComandoMotor &comando) {
  return filaComandosMotor != nullptr &&
         xQueueSend(filaComandosMotor, &comando, 0) == pdTRUE;
}

const char *mensagemFalha(FalhaMotor falha) {
  switch (falha) {
  case FalhaMotor::Driver:
    return "Falha no driver do motor";
  case FalhaMotor::FimDeCurso:
    return "Fim de curso acionado";
  case FalhaMotor::SemMovimento:
    return "Eixo sem movimento; verifique o mecanismo";
  case FalhaMotor::SentidoIncorreto:
    return "Eixo girando no sentido incorreto";
  case FalhaMotor::FrequenciaInvalida:
    return "Frequencia de passos invalida";
  case FalhaMotor::ParametrosInvalidos:
    return "Parametros fora dos limites";
  case FalhaMotor::Nenhuma:
    break;
  }
  return "Falha desconhecida";
}

bool vazaoCabeNaFaixaDoMotor(float vazaoMlPorHora) {
  if (Config::area_seringa_mm2 <= 0.0f ||
      Config::fuso_avanco_mm_por_volta <= 0.0f) {
    return false;
  }

  const float passosPorVolta =
      static_cast<float>(Config::passos_por_volta_motor) * Config::microsteps;
  const float rpm = vazaoMlPorHora * 1000.0f /
                    (60.0f * Config::area_seringa_mm2 *
                     Config::fuso_avanco_mm_por_volta);
  const float frequenciaPassosHz = rpm * passosPorVolta / 60.0f;

  return frequenciaPassosHz >= Config::FREQUENCIA_PASSO_MIN_HZ &&
         frequenciaPassosHz <= Config::FREQUENCIA_PASSO_MAX_HZ;
}

void desenharEstado(EstadoInterface estado,
                    float vazaoMlPorHora,
                    uint8_t tempoMinutos,
                    const EstadoControleMotor &estadoMotor,
                    const char *erroConfiguracao,
                    uint32_t segundosRestantes) {
  switch (estado) {
  case EstadoInterface::Inicio:
    tela.Inicio();
    break;
  case EstadoInterface::ConfigurarVazao:
    tela.ConfiguracaoVazao(vazaoMlPorHora);
    break;
  case EstadoInterface::ConfigurarTempo:
    tela.ConfiguracaoTempo(tempoMinutos);
    break;
  case EstadoInterface::Confirmar:
    tela.Confirmacao(vazaoMlPorHora, tempoMinutos);
    break;
  case EstadoInterface::ConfiguracaoInvalida:
    tela.ConfiguracaoInvalida(erroConfiguracao);
    break;
  case EstadoInterface::Execucao: {
    const uint8_t minutosRestantes = static_cast<uint8_t>(
        (segundosRestantes + 59) / 60);
    tela.Execucao(estadoMotor.vazaoAlvoMlPorHora,
                  estadoMotor.vazaoEstimadaMlPorHora,
                  estadoMotor.rpmMedido,
                  minutosRestantes);
    break;
  }
  case EstadoInterface::Falha:
    tela.Falha(mensagemFalha(estadoMotor.falha));
    break;
  case EstadoInterface::Concluido:
    tela.Concluido();
    break;
  }
}

void tarefaInterface(void *) {
  EstadoInterface estado = EstadoInterface::Inicio;
  EstadoControleMotor estadoMotor;
  float vazaoSetadaMlPorHora = Config::limiteInferiorVazao;
  uint8_t tempoSetadoMinutos = Config::limiteInferiorTempoMin;
  const char *erroConfiguracao = "Verifique os parametros";
  uint32_t inicioExecucaoMs = 0;
  uint32_t duracaoExecucaoSegundos = 0;
  uint32_t ultimaAtualizacaoTelaMs = 0;
  bool telaSuja = true;
  bool motorIniciouEstaExecucao = false;

  for (;;) {
    const uint32_t agoraMs = millis();
    EstadoControleMotor novoEstadoMotor;
    if (xQueuePeek(filaEstadoMotor, &novoEstadoMotor, 0) == pdTRUE) {
      estadoMotor = novoEstadoMotor;
    }
    if (estado == EstadoInterface::Execucao && estadoMotor.executando) {
      motorIniciouEstaExecucao = true;
    }

    if (estadoMotor.falha != FalhaMotor::Nenhuma &&
        estado != EstadoInterface::Falha) {
      estado = EstadoInterface::Falha;
      buzzer.tocar();
      telaSuja = true;
    } else if (estado == EstadoInterface::Execucao &&
               motorIniciouEstaExecucao && estadoMotor.concluido) {
      estado = EstadoInterface::Concluido;
      buzzer.parar();
      telaSuja = true;
    } else if (estado == EstadoInterface::Execucao &&
               motorIniciouEstaExecucao && !estadoMotor.executando) {
      // Uma parada solicitada pelo usuario retorna a tela ao inicio.
      estado = EstadoInterface::Inicio;
      buzzer.parar();
      telaSuja = true;
    } else if (estado == EstadoInterface::Falha &&
               estadoMotor.falha == FalhaMotor::Nenhuma) {
      estado = EstadoInterface::Inicio;
      buzzer.parar();
      telaSuja = true;
    }

    const EventoBotao evento = botao.atualizar(agoraMs);
    const bool pressionado = evento == EventoBotao::Pressionado;
    const bool girouHorario = evento == EventoBotao::Horario;
    const bool girouAntiHorario = evento == EventoBotao::AntiHorario;
    const int8_t direcaoRotacao = girouHorario ? 1 : (girouAntiHorario ? -1 : 0);

    switch (estado) {
    case EstadoInterface::Inicio:
      if (pressionado) {
        vazaoSetadaMlPorHora = Config::limiteInferiorVazao;
        estado = EstadoInterface::ConfigurarVazao;
        telaSuja = true;
      }
      break;

    case EstadoInterface::ConfigurarVazao:
      if (direcaoRotacao != 0) {
        vazaoSetadaMlPorHora = constrain(
            vazaoSetadaMlPorHora + direcaoRotacao,
            Config::limiteInferiorVazao,
            Config::limiteSuperiorVazao);
        telaSuja = true;
      }
      if (pressionado) {
        tempoSetadoMinutos = Config::limiteInferiorTempoMin;
        estado = EstadoInterface::ConfigurarTempo;
        telaSuja = true;
      }
      break;

    case EstadoInterface::ConfigurarTempo:
      if (direcaoRotacao != 0) {
        const int tempoNovo = static_cast<int>(tempoSetadoMinutos) + direcaoRotacao;
        tempoSetadoMinutos = constrain(
            tempoNovo,
            static_cast<int>(Config::limiteInferiorTempoMin),
            static_cast<int>(Config::limiteSuperiorTempoMin));
        telaSuja = true;
      }
      if (pressionado) {
        const float volumeEstimadoMl =
            vazaoSetadaMlPorHora * tempoSetadoMinutos / 60.0f;
        if (volumeEstimadoMl > Config::volume_seringa_ml) {
          erroConfiguracao = "Volume excede o volume da seringa";
          estado = EstadoInterface::ConfiguracaoInvalida;
        } else if (!vazaoCabeNaFaixaDoMotor(vazaoSetadaMlPorHora)) {
          erroConfiguracao = "Vazao fora da faixa do motor";
          estado = EstadoInterface::ConfiguracaoInvalida;
        } else {
          estado = EstadoInterface::Confirmar;
        }
        telaSuja = true;
      }
      break;

    case EstadoInterface::Confirmar:
      if (pressionado) {
        ComandoMotor comando;
        comando.tipo = TipoComandoMotor::Iniciar;
        comando.vazaoMlPorHora = vazaoSetadaMlPorHora;
        comando.duracaoSegundos = static_cast<uint32_t>(tempoSetadoMinutos) * 60UL;
        if (enviarComando(comando)) {
          inicioExecucaoMs = agoraMs;
          duracaoExecucaoSegundos = comando.duracaoSegundos;
          motorIniciouEstaExecucao = false;
          estado = EstadoInterface::Execucao;
        } else {
          erroConfiguracao = "Nao foi possivel enviar o comando";
          estado = EstadoInterface::ConfiguracaoInvalida;
        }
        telaSuja = true;
      }
      break;

    case EstadoInterface::ConfiguracaoInvalida:
      if (pressionado) {
        estado = EstadoInterface::ConfigurarVazao;
        telaSuja = true;
      }
      break;

    case EstadoInterface::Execucao:
      if (pressionado) {
        ComandoMotor comando;
        comando.tipo = TipoComandoMotor::Parar;
        enviarComando(comando);
      }
      break;

    case EstadoInterface::Falha:
      if (pressionado) {
        ComandoMotor comando;
        comando.tipo = TipoComandoMotor::ReconhecerFalha;
        enviarComando(comando);
      }
      break;

    case EstadoInterface::Concluido:
      if (pressionado) {
        estado = EstadoInterface::Inicio;
        telaSuja = true;
      }
      break;
    }

    const bool atualizacaoPeriodica =
        estado == EstadoInterface::Execucao && (uint32_t)(agoraMs - ultimaAtualizacaoTelaMs) >= 1000;
    if (telaSuja || atualizacaoPeriodica) {
      uint32_t segundosRestantes = 0;
      if (estado == EstadoInterface::Execucao) {
        const uint32_t segundosDecorridos = (agoraMs - inicioExecucaoMs) / 1000UL;
        segundosRestantes = segundosDecorridos >= duracaoExecucaoSegundos
                                ? 0
                                : duracaoExecucaoSegundos - segundosDecorridos;
      }
      desenharEstado(estado,
                     vazaoSetadaMlPorHora,
                     tempoSetadoMinutos,
                     estadoMotor,
                     erroConfiguracao,
                     segundosRestantes);
      ultimaAtualizacaoTelaMs = agoraMs;
      telaSuja = false;
    }

    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

} // namespace

void setup() {
  Serial.begin(115200);

  botao.iniciar();
  tela.iniciar();
  buzzer.iniciar();
  fimSuperior.iniciar();
  fimInferior.iniciar();
  encoderEixo.iniciar();

  if (!motor.iniciar()) {
    motor.parar();
    buzzer.tocar();
    tela.Falha("Falha ao iniciar saida STEP");
    return;
  }

  filaComandosMotor = xQueueCreate(4, sizeof(ComandoMotor));
  filaEstadoMotor = xQueueCreate(1, sizeof(EstadoControleMotor));
  if (filaComandosMotor == nullptr || filaEstadoMotor == nullptr) {
    motor.parar();
    buzzer.tocar();
    tela.Falha("Falha ao criar filas RTOS");
    return;
  }

  EstadoControleMotor estadoInicial;
  xQueueOverwrite(filaEstadoMotor, &estadoInicial);

  // Interface no nucleo 0; controle periodico do motor no nucleo 1.
  const BaseType_t nucleoInterface = 0;
  const BaseType_t nucleoControle = 1;
  const bool controleCriado = controleMotor.iniciarTarefa(
      filaComandosMotor, filaEstadoMotor, nucleoControle);
  if (!controleCriado) {
    motor.parar();
    buzzer.tocar();
    tela.Falha("Falha ao criar tarefas");
    return;
  }

  const BaseType_t interfaceCriada = xTaskCreatePinnedToCore(
      tarefaInterface, "interface", 6144, nullptr, 1, nullptr, nucleoInterface);
  if (interfaceCriada != pdPASS) {
    motor.parar();
    buzzer.tocar();
    tela.Falha("Falha ao criar tarefas");
  }
}

void loop() {
  // A interface e o controle rodam em tarefas FreeRTOS dedicadas.
  vTaskDelay(pdMS_TO_TICKS(1000));
}

#include "config.h"
#include "botao.h"
#include "tela.h"

Botao botao(Config::PIN_BOTAO,
            Config::PIN_ENCODER_A,
            Config::PIN_ENCODER_B,
            Config::MODO_BOTOES,
            Config::NIVEL_PRESSIONADO,
            Config::DEBOUNCE_MS);

Adafruit_ILI9341 display(
    Config::PIN_TELA_CS,
    Config::PIN_TELA_DC,
    Config::PIN_TELA_RST);

Tela tela(display);

enum class Estado
{
  Inicio,
  Configuracao,
  Validacao,
  Invalido,
  Falha,
  Calculo,
  Execucao,
  Obstrucao,
  Concluido
};

enum class EtapaConfiguracao
{
  Vazao,
  Tempo
};

enum class ResultadoValidacao
{
  Valido,
  Invalido
};

enum class OpcaoConfirmacao
{
  Confirmar,
  Retornar
};

struct Eventos
{
  bool configuracaoConfirmada = false;
  bool execucaoConcluida = false;
  bool falhaDetectada = false;
  bool falhaReconhecida = false;
  bool obstrucaoDetectada = false;
  bool obstrucaoResolvida = false;
  bool concluido = false;

  bool botaoPressionado = false;
  bool botaoHorario = false;
  bool botaoAntiHorario = false;

  bool vazaoConfirmada = false;
};

float vazaoSetada = 0;
uint8_t tempoSetado = 0;
float frequencia_motor = 0;

namespace
{

  Estado estadoAtual = Estado::Inicio;

  const char *nomeDoEstado(Estado estado)
  {
    switch (estado)
    {
    case Estado::Inicio:
      return "Inicio";
    case Estado::Configuracao:
      return "Configuracao";
    case Estado::Validacao:
      return "Validacao";
    case Estado::Invalido:
      return "Invalido";
    case Estado::Calculo:
      return "Calculo";
    case Estado::Execucao:
      return "Execucao";
    case Estado::Falha:
      return "Falha";
    case Estado::Obstrucao:
      return "Obstrucao";
    case Estado::Concluido:
      return "Concluido";
    }
    return "Estado desconhecido";
  }

  EtapaConfiguracao etapaConfiguracao = EtapaConfiguracao::Vazao;

  const char *nomeDaEtapaConfiguracao(EtapaConfiguracao etapa)
  {
    switch (etapa)
    {
    case EtapaConfiguracao::Vazao:
      return "Vazao";
    case EtapaConfiguracao::Tempo:
      return "Tempo";
    }
    return "Configuracao desconhecida";
  }

  void mudarEstado(Estado novoEstado)
  {

    if (novoEstado == estadoAtual)
    {
      return;
    }

    estadoAtual = novoEstado;
    Serial.print("Estado: ");
    Serial.println(nomeDoEstado(estadoAtual));
  }

  Eventos lerEventos()
  {
    Eventos eventos;

    const EventoBotao eventoBotao = botao.atualizar();
    eventos.botaoPressionado = eventoBotao == EventoBotao::Pressionado;
    eventos.botaoHorario = eventoBotao == EventoBotao::Horario;
    eventos.botaoAntiHorario = eventoBotao == EventoBotao::AntiHorario;

    return eventos;
  }

  int atualizarConfiguracaoVazao()
  {
    Eventos evento;
    int vazao = 0;
    while (evento.botaoPressionado == false)
    {
      evento = lerEventos();
      if (evento.botaoHorario || evento.botaoAntiHorario && vazao < Config::limiteSuperiorVazao && vazao > Config::limiteInferiorVazao)
      {
        vazao += evento.botaoHorario ? 1 : -1;
      }
    }
    return vazao;
  }

  int atualizarConfiguracaoTempo()
  {
    Eventos evento;
    int tempo = 0;
    while (evento.botaoPressionado == false)
    {
      evento = lerEventos();
      if (evento.botaoHorario || evento.botaoAntiHorario && tempo > 0 && tempo < Config::limiteSuperiorTempo)
      {
        tempo += evento.botaoHorario ? 1 : -1;
      }
    }
    return tempo;
  }

  uint32_t calcular_intervalo_passo(float vazao_mL_min)
  {
    if (vazao_mL_min <= 0.0f || Config::area_seringa_mm <= 0.0f || Config::volume_seringa_ml <= 0.0f)
      return 0;
    if (Config::fuso_avanco_mm_por_volta <= 0.0f)
      return 0;

    const uint32_t passos_por_volta =
        (uint32_t)Config::passos_por_volta_motor * Config::microsteps;

    // f [passos/s] = Q * N_m / (0,06 * A * p)
    const float f = vazao_mL_min * passos_por_volta /
                    (0.06f * Config::area_seringa_mm * Config::fuso_avanco_mm_por_volta);

    if (f <= 0.0f)
      return 0;

    const float intervalo_us = 1e6f / f;

    // saturação para não estourar o tipo
    if (intervalo_us >= 4.0e9f)
      return 0;

    return (uint32_t)(intervalo_us + 0.5f);
  }

  void atualizarMaquinaDeEstados(const Eventos &eventos)
  {
    if ((estadoAtual == Estado::Configuracao || estadoAtual == Estado::Calculo) && eventos.falhaDetectada)
    {
      mudarEstado(Estado::Falha);
      return;
    }

    switch (estadoAtual)
    {
    case Estado::Inicio:
      tela.Inicio();
      if (eventos.botaoPressionado == true)
      {
        mudarEstado(Estado::Configuracao);
      }
      break;

    case Estado::Configuracao:
    {
      Eventos eventosConfiguracao;

      switch (etapaConfiguracao)
      {
      case EtapaConfiguracao::Vazao:
      {
        vazaoSetada = atualizarConfiguracaoVazao();
        OpcaoConfirmacao opcaoVazao = OpcaoConfirmacao::Confirmar;

        if (!eventosConfiguracao.botaoPressionado ||
            opcaoVazao == OpcaoConfirmacao::Retornar)
        {
          eventosConfiguracao = lerEventos();
          if (eventosConfiguracao.botaoHorario || eventosConfiguracao.botaoAntiHorario)
          {
            opcaoVazao = opcaoVazao == OpcaoConfirmacao::Confirmar
                             ? OpcaoConfirmacao::Retornar
                             : OpcaoConfirmacao::Confirmar;
          }
        }
        else
        {
          eventosConfiguracao = lerEventos();
          if (eventosConfiguracao.botaoPressionado && vazaoSetada > Config::limiteInferiorVazao && vazaoSetada < Config::limiteSuperiorVazao && opcaoVazao == OpcaoConfirmacao::Confirmar)
          {
            etapaConfiguracao = EtapaConfiguracao::Tempo;
          }
          else if (eventosConfiguracao.botaoPressionado && opcaoVazao == OpcaoConfirmacao::Confirmar)
          {
            mudarEstado(Estado::Invalido);
          }
        }
        break;
      }

      case EtapaConfiguracao::Tempo:
      {
        tempoSetado = atualizarConfiguracaoTempo();
        OpcaoConfirmacao opcaoTempo = OpcaoConfirmacao::Confirmar;

        if (!eventosConfiguracao.botaoPressionado || opcaoTempo == OpcaoConfirmacao::Retornar)
        {
          eventosConfiguracao = lerEventos();
          if (eventosConfiguracao.botaoHorario || eventosConfiguracao.botaoAntiHorario)
          {
            opcaoTempo = opcaoTempo == OpcaoConfirmacao::Confirmar
                             ? OpcaoConfirmacao::Retornar
                             : OpcaoConfirmacao::Confirmar;
          }
        }
        break;
      }
      }
      break;
    }

    case Estado::Validacao:
      if ((vazaoSetada * tempoSetado) < Config::volumeSeginga)
      {
        tela.ConfiguracaoValida(vazaoSetada, tempoSetado);
        if (eventos.botaoPressionado == true)
        {
          mudarEstado(Estado::Calculo);
        }
      }
      else
      {
        mudarEstado(Estado::Invalido);
      }
      break;

    case Estado::Invalido:
      tela.ConfiguracaoInvalida("Volume de medicamento maior q o volume da seringa");
      if (eventos.botaoPressionado == true)
      {
        mudarEstado(Estado::Configuracao);
      }
      break;

    case Estado::Calculo:
      if (Config::volume_seringa_ml >= (vazaoSetada * tempoSetado))
      {
        frequencia_motor = calcular_intervalo_passo(vazaoSetada);
        if(frequencia_motor > 0){
          mudarEstado(Estado::Execucao);
        }
        else{
          mudarEstado(Estado::Invalido);
        }
      }
      else
      {
        mudarEstado(Estado::Invalido);
      }
      break;

    case Estado::Execucao:
      if (eventos.execucaoConcluida)
      {
        mudarEstado(Estado::Concluido);
      }
      break;

    case Estado::Falha:
      if (eventos.falhaReconhecida)
      {
        // Depois de resolver e reconhecer a falha, voltar a configurar.

        mudarEstado(Estado::Configuracao);
      }
      break;

    case Estado::Concluido:
      // Estado terminal da operacao atual. Um novo ciclo pode ser definido depois.
      break;
    }
  }

} // namespace

void setup()
{
  Serial.begin(115200);
  Serial.print("Estado: ");
  Serial.println(nomeDoEstado(estadoAtual));

  botao.iniciar();
  botao.configurarEntradaSerial(Serial);

  tela.iniciar();
}

void loop()
{
  const Eventos eventos = lerEventos();
  atualizarMaquinaDeEstados(eventos);
  delay(10);
}

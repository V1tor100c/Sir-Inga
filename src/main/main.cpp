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
  Execucao,
  Falha,
  Obstrucao,
  Concluido
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

    eventos.botaoPressionado =
      botao.atualizar() == EventoBotao::Pressionado;

      eventos.botaoHorario = botao.atualizar() == EventoBotao::Horario;
      eventos.botaoAntiHorario = botao.atualizar() == EventoBotao::AntiHorario;

    return eventos;
  }

  // float atualizarConfiguracaoVazao()
  // {
  //   uint8_t digitosVazao[4] = {0, 0, 0, 0};
  //   tela.ConfiguracaoVazao(digitosVazao, 0);
  //   for (int i = 0; i < 4; i++)
  //   {
  //     Eventos eventos;
  //     while (eventos.botaoPressionado == false)
  //     {
  //       eventos = lerEventos();
  //       if (eventos.botao1Pressionado && !eventos.botao2Pressionado)
  //       {
  //         digitosVazao[i] = (digitosVazao[i] + 1) % 10;
  //         tela.ConfiguracaoVazao(digitosVazao, i);
  //       }
  //     }
  //   }
  //   return digitosVazao[0] * 100 + digitosVazao[1] * 10 + digitosVazao[2] * 1 + digitosVazao[3] * 0.1;
  }

 int atualizarConfiguracaoVazao()
  {
    int valorVazao = Config::limiteInferiorVazao;
    tela.ConfiguracaoVazao(valorVazao);
    Eventos eventos;
      while (eventos.botaoPressionado == false)
      {
        eventos = lerEventos();
        if (valorVazao >= Config::limiteInferiorVazao && valorVazao <= Config::limiteSuperiorVazao){
          if (eventos.botaoHorario)
          {
            valorVazao = valorVazao + 1;
          }
          else if(eventos.botaoAntiHorario)
          {
            valorVazao = valorVazao - 1;
          }
          tela.ConfiguracaoVazao(valorVazao);
        }
        if (eventos.botaoPressionado){
          break;
        }
      }
    return valorVazao;
    }
  

  uint8_t atualizarConfiguracaoTempo()
  {
    uint8_t digitosTempo = 0;
    tela.ConfiguracaoTempo(digitosTempo);
    Eventos eventos;
    while (eventos.botao2Pressionado == false)
    {
      eventos = lerEventos();
      if (eventos.botao1Pressionado && !eventos.botao2Pressionado)
      {
        digitosTempo = (digitosTempo + 1) % 61;
        tela.ConfiguracaoTempo(digitosTempo);
      }
    }
    return digitosTempo;
  }

  void atualizarMaquinaDeEstados(const Eventos &eventos)
  {
    if ((estadoAtual == Estado::Configuracao || estadoAtual == Estado::Execucao) && eventos.falhaDetectada)
    {
      mudarEstado(Estado::Falha);
      return;
    }

    switch (estadoAtual)
    {
    case Estado::Inicio:
      tela.Inicio();
      if (eventos.botao2Pressionado == true)
      {
        mudarEstado(Estado::Configuracao);
      }
      break;

    case Estado::Configuracao:

      vazaoSetada = 0;
      vazaoSetada = atualizarConfiguracaoVazao();

      if (eventos.botao1Pressionado == true && vazaoSetada > Config::limiteInferiorVazao && vazaoSetada < Config::limiteSuperiorVazao)
      {
        mudarEstado(Estado::Execucao);
        break;
      }
      else if (eventos.botao1Pressionado == true)
      {
        mudarEstado(Estado::Invalido);
        break;
      }
      else if (eventos.botao2Pressionado == true)
      {
        break;
      }

      tempoSetado = 0;
      tempoSetado = atualizarConfiguracaoTempo();

      if (eventos.botao1Pressionado == true)
      {
        mudarEstado(Estado::Validacao);
        break;
      }
      else if (eventos.botao2Pressionado == true)
      {
        break;
      }
      break;

    case Estado::Validacao:
      if ((vazaoSetada * tempoSetado) < Config::volumeSeginga)
      {
        tela.ConfiguracaoValida(vazaoSetada, tempoSetado);
        if(eventos.botao2Pressionado == true){
          mudarEstado(Estado::Execucao);
        }
      }
      else
      {
        mudarEstado(Estado::Invalido);
      }
      break;

    case Estado::Invalido:
      tela.ConfiguracaoInvalida("Volume de medicamento maior q o volume da seringa");
      if (eventos.botao2Pressionado == true){
        mudarEstado(Estado::Configuracao);
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

  tela.iniciar();
}

void loop()
{
  const Eventos eventos = lerEventos();
  atualizarMaquinaDeEstados(eventos);
  delay(10);
}

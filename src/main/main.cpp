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

enum class Configuracao
{
  Vazao,
  Tempo
};

enum class Validacao
{
  Valido,
  Invalido
};

enum class Confirmacao
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

  Configuracao EstadoConfiguracao = Configuracao::Vazao;

  const char *nomeDaConfiguracao(Configuracao configuracao)
  {
    switch (configuracao)
    {
    case Configuracao::Vazao:
      return "Vazao";
    case Configuracao::Tempo:
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

    eventos.botaoPressionado =
      botao.atualizar() == EventoBotao::Pressionado;

      eventos.botaoHorario = botao.atualizar() == EventoBotao::Horario;
      eventos.botaoAntiHorario = botao.atualizar() == EventoBotao::AntiHorario;

    return eventos;
  }

  }

  int atualizarConfiguracaoVazao(){
    Eventos evento;
    int vazao = 0;
    while(evento.botaoPressionado == false){
      evento = lerEventos();
      if(evento.botaoHorario || evento.botaoAntiHorario && vazao < Config::limiteSuperiorVazao && vazao > Config::limiteInferiorVazao){
        vazao += evento.botaoHorario ? 1 : -1;
      }
    }
    return vazao;
  }

  int atualizarConfiguracaoTempo(){
    Eventos evento;
    int tempo = 0;
    while(evento.botaoPressionado == false){
      evento = lerEventos();
      if(evento.botaoHorario || evento.botaoAntiHorario && tempo > 0 && tempo < Config::limiteSuperiorTempo){
        tempo += evento.botaoHorario ? 1 : -1;
      }
    }
    return tempo;
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
      if (eventos.botaoPressionado == true)
      {
        mudarEstado(Estado::Configuracao);
      }
      break;

    case Estado::Configuracao:
      switch (EstadoConfiguracao)
      {
        case EstadoConfiguracao::Vazao:

        break;

        case EstadoConfiguracao::Tempo:

        break;
      }

      break;

    // case Estado::Configuracao:

    //   while(vazaoSetada == 0){
    //     Eventos eventos;

    //     vazaoSetada = atualizarConfiguracaoVazao();

    //     Confirmacao confirmarVazao = Confirmacao::Confirmar;

    //       while(eventos.botaoPressionado == false || confirmarVazao == Confirmacao::Retornar || confirmarVazao == Confirmacao::Invalido){
    //         eventos = lerEventos();
    //         if(eventos.botaoHorario || eventos.botaoAntiHorario){
    //           if(confirmarVazao == Confirmacao::Confirmar){
    //             confirmarVazao = Confirmacao::Retornar;
    //           }
    //           else{
    //             confirmarVazao = Confirmacao::Confirmar;
    //           }
    //         }
    //       }
          
    //       Validacao validacaoVazao;
    //       eventos = lerEventos();
    //       if (eventos.botaoPressionado == true && vazaoSetada > Config::limiteInferiorVazao && vazaoSetada < Config::limiteSuperiorVazao && confirmarVazao == Confirmacao::Confirmar)
    //       {
    //         validacaoVazao = Confirmacao::Valido;
    //         break;
    //       }
    //       else if (eventos.botaoPressionado == true && confirmarVazao == Confirmacao::Confirmar)
    //       {
    //         validacaoVazao = Confirmacao::Invalido;
    //         // tela::ConfiguracaoInvalida("Valor da vazao invalido");
    //         mudarEstado(Estado::Invalido);
    //         break;
    //       }
    //     }
    //   }

    //   tempoSetado = 0;
    //   tempoSetado = atualizarConfiguracaoTempo();

    //   while (tempoSetado == 0){
    //     Eventos eventos;

    //     tempoSetado = atualizarConfiguracaoTempo();

    //     Confirmacao confirmarTempo = Confirmacao::Confirmar;

    //       while(eventos.botaoPressionado == false){
    //         eventos = lerEventos();
    //         if(eventos.botaoHorario || eventos.botaoAntiHorario){
    //           if(confirmarTempo == Confirmacao::Confirmar){
    //             confirmarTempo = Confirmacao::Retornar;
    //           }
    //           else{
    //             confirmarTempo = Confirmacao::Confirmar;
    //           }
    //         }
    //       }
          
    //       Validacao validacaoTempo;
    //       eventos = lerEventos();
    //       if (eventos.botaoPressionado == true && tempoSetado > 0 && tempoSetado < Config::limiteSuperiorTempo && confirmarTempo == Confirmacao::Confirmar)
    //       {
    //         validacaoTempo = Validacao::Valido;
    //         break;
    //       }
    //       else if (eventos.botaoPressionado == true && confirmarTempo == Confirmacao::Confirmar){
    //         validacaoTempo = Validacao::Invalido;
    //         // tela::ConfiguracaoInvalida("Valor do tempo invalido");
    //         mudarEstado(Estado::Invalido);
    //         break;
    //       }
    //       else if (confirmarTempo == Confirmacao::Retornar)
    //     }

    //   if (validacaoVazao == Validacao::Valido && validacaoTempo == Validacao::Valido)
    //   {
    //     tela.ConfiguracaoValida(vazaoSetada, tempoSetado);
    //     mudarEstado(Estado::Validacao);
    //   }
    // break;

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

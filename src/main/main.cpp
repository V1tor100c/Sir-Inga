#include "config.h"
#include "botao.h"

Botao botao1(Config::PIN_BOTAO_1, Config::MODO_BOTOES, Config::NIVEL_PRESSIONADO, Config::DEBOUNCE_MS);
Botao botao2(Config::PIN_BOTAO_2, Config::MODO_BOTOES, Config::NIVEL_PRESSIONADO, Config::DEBOUNCE_MS);

enum class Estado {
  Inicio,
  Configuracao,
  Invalido,
  Execucao,
  Falha,
  Obstrucao,
  Concluido
};

struct Eventos {
  bool configuracaoConfirmada = false;
  bool execucaoConcluida = false;
  bool falhaDetectada = false;
  bool falhaReconhecida = false;
  bool obstrucaoDetectada = false;
  bool obstrucaoResolvida = false;
  bool concluido = false;

  bool botao1Pressionado = false;
  bool botao2Pressionado = false;

  bool vazaoConfirmada = false;
};

float vazaoSetada = 0;

namespace {

Estado estadoAtual = Estado::Inicio;

const char* nomeDoEstado(Estado estado) {
  switch (estado) {
    case Estado::Inicio:       return "Inicio";
    case Estado::Configuracao: return "Configuracao";
    case Estado::Execucao:     return "Execucao";
    case Estado::Falha:        return "Falha";
    case Estado::Obstrucao:    return "Obstrucao";
    case Estado::Concluido:    return "Concluido";
  }
  return "Estado desconhecido";
}

void mudarEstado(Estado novoEstado) {

  if (novoEstado == estadoAtual) {
    return;
  }

  estadoAtual = novoEstado;
  Serial.print("Estado: ");
  Serial.println(nomeDoEstado(estadoAtual));

}

Eventos lerEventos() {
  Eventos eventos;

  eventos.botao1Pressionado =
    botao1.atualizar() == EventoBotao::Pressionado;

  eventos.botao2Pressionado =
    botao2.atualizar() == EventoBotao::Pressionado;

  return eventos;
}

float atualizarConfiguracaoVazao(){
  uint8_t digitosVazao[4] = {0, 0, 0, 0};

  for(int i = 0; i < 4; i++){
    Eventos eventos;
    while(eventos.botao2Pressionado == false){
      eventos = lerEventos();
      if(eventos.botao1Pressionado && !eventos.botao2Pressionado){
        digitosVazao[i] = (digitosVazao[i] + 1) % 10;
        // atualizar tela display
      }
    }
  }
  return  digitosVazao[0] * 100 
        + digitosVazao[1] * 10 
        + digitosVazao[2] * 1 
        + digitosVazao[3] * 0.1;
}

uint8_t atualizarConfiguracaoTempo(){
  uint8_t digitosTempo = 0;
  
    Eventos eventos;
    while(eventos.botao2Pressionado == false){
      eventos = lerEventos();
      if(eventos.botao1Pressionado && !eventos.botao2Pressionado){
        digitosTempo = (digitosTempo + 1) % 61;
        // atualizar tela display
      }
    
  }
  return  digitosTempo;
}

void atualizarMaquinaDeEstados(const Eventos& eventos) {
  // Uma falha tem prioridade sobre a transicao normal durante a operacao.
  if ((estadoAtual == Estado::Configuracao || estadoAtual == Estado::Execucao) && eventos.falhaDetectada) {
    mudarEstado(Estado::Falha);
    return;
  }

  switch (estadoAtual) {
    case Estado::Inicio:
      // Inicializacao concluida: apresentar a etapa de configuracao.
      // Tela logo
      if (eventos.botao2Pressionado == true) {
        mudarEstado(Estado::Configuracao);
      }
      break;

    case Estado::Configuracao:
      vazaoSetada = 0; 

      vazaoSetada = atualizarConfiguracaoVazao();

      while(true){
        // Tela de confimação da configuracao vazao
        if (eventos.botao1Pressionado == true && vazaoSetada > Config::limiteInferiorVazao && vazaoSetada < Config::limiteSuperiorVazao){ 
          mudarEstado(Estado::Execucao);
          break;
        }
        else if(eventos.botao1Pressionado == true){
          mudarEstado(Estado::Invalido);
          break;
        }
        else if(eventos.botao2Pressionado == true){
          break;
        }
      }

      while(true){
        // Tela de confimação da configuracao tempo
        if (eventos.botao1Pressionado == true){ 
          mudarEstado(Estado::Execucao);
          break;
        }
        // else if(eventos.botao1Pressionado == true){
        //   mudarEstado(Estado::Invalido);
        //   break;
        // }
        else if(eventos.botao2Pressionado == true){
          break;
        }
      }
      break;

    case Estado::Execucao:
      if (eventos.execucaoConcluida) {
        mudarEstado(Estado::Concluido);
      }
      break;

    case Estado::Falha:
      if (eventos.falhaReconhecida) {
        // Depois de resolver e reconhecer a falha, voltar a configurar.
        mudarEstado(Estado::Configuracao);
      }
      break;

    case Estado::Concluido:
      // Estado terminal da operacao atual. Um novo ciclo pode ser definido depois.
      break;
  }
}

}  // namespace

void setup() {
  Serial.begin(115200);
  Serial.print("Estado: ");
  Serial.println(nomeDoEstado(estadoAtual));

  botao1.iniciar();
  botao2.iniciar();
}

void loop() {
  const Eventos eventos = lerEventos();
  atualizarMaquinaDeEstados(eventos);
  delay(10);
}

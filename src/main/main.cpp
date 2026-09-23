#include <Arduino.h>

enum class Estado {
  Inicio,
  Configuracao,
  Execucao,
  Falha,
  Concluido
};

struct Eventos {
  bool inicializacaoConcluida = false;
  bool configuracaoConfirmada = false;
  bool execucaoConcluida = false;
  bool falhaDetectada = false;
  bool falhaReconhecida = false;
};

namespace {

Estado estadoAtual = Estado::Inicio;

const char* nomeDoEstado(Estado estado) {
  switch (estado) {
    case Estado::Inicio:       return "Inicio";
    case Estado::Configuracao: return "Configuracao";
    case Estado::Execucao:     return "Execucao";
    case Estado::Falha:        return "Falha";
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

  // TODO: preencher com os botoes, sensores e criterios definidos pela equipe.
  // Por enquanto, nenhum evento externo e gerado.

  return eventos;
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
      if (eventos.inicializacaoConcluida){
        mudarEstado(Estado::Configuracao);
      }
      break;

    case Estado::Configuracao:
      if (eventos.configuracaoConfirmada) {
        mudarEstado(Estado::Execucao);
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
}

void loop() {
  const Eventos eventos = lerEventos();
  atualizarMaquinaDeEstados(eventos);
  delay(10);
}

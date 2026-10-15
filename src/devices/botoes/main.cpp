#include <Arduino.h>

#include "../../main/botao.h"

namespace {

Botao botao1(Config::PIN_BOTAO_1,
             Config::MODO_BOTOES,
             Config::NIVEL_PRESSIONADO,
             Config::DEBOUNCE_MS);
Botao botao2(Config::PIN_BOTAO_2,
             Config::MODO_BOTOES,
             Config::NIVEL_PRESSIONADO,
             Config::DEBOUNCE_MS);

void imprimirEvento(const char* nome, EventoBotao evento) {
  if (evento == EventoBotao::Pressionado) {
    Serial.print(nome);
    Serial.println(" pressionado");
  } else if (evento == EventoBotao::Solto) {
    Serial.print(nome);
    Serial.println(" solto");
  }
}

}  // namespace

void setup() {
  Serial.begin(115200);
  botao1.iniciar();
  botao2.iniciar();

  Serial.println("Teste dos botoes iniciado");
  Serial.println("Aperte e solte cada botao; cada evento deve aparecer uma vez.");
}

void loop() {
  imprimirEvento("Botao 1", botao1.atualizar());
  imprimirEvento("Botao 2", botao2.atualizar());
  delay(2);
}

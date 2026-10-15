#include <Arduino.h>
#include <Adafruit_ILI9341.h>

#include "../../main/tela.h"

// EXEMPLO: troque estes tres pinos para corresponder a sua ligacao real.
// O clock e os dados usam o barramento SPI de hardware padrao da placa.
constexpr int8_t PIN_TELA_CS = 5;
constexpr int8_t PIN_TELA_DC = 27;
constexpr int8_t PIN_TELA_RST = 26;

namespace {

Adafruit_ILI9341 display(PIN_TELA_CS, PIN_TELA_DC, PIN_TELA_RST);
Tela tela(display);

constexpr uint32_t kTempoPorPaginaMs = 3000;
constexpr uint8_t kQuantidadePaginas = 9;

const uint8_t kDigitosExemplo[4] = {1, 2, 3, 4};
uint8_t paginaAtual = 0;
uint32_t instanteUltimaPagina = 0;

void mostrarPagina(uint8_t pagina) {
  Serial.print("Pagina de teste: ");
  Serial.println(pagina);

  switch (pagina) {
    case 0:
      tela.mostrarInicio();
      break;
    case 1:
      tela.mostrarConfiguracaoVazao(kDigitosExemplo, 1);
      break;
    case 2:
      tela.mostrarConfiguracaoTempo(30);
      break;
    case 3:
      tela.mostrarConfirmacao(123.4f, 30);
      break;
    case 4:
      tela.mostrarExecucao(123.4f, 24);
      break;
    case 5:
      tela.mostrarInvalido("Valor de exemplo invalido");
      break;
    case 6:
      tela.mostrarFalha("Falha de exemplo");
      break;
    case 7:
      tela.mostrarObstrucao();
      break;
    case 8:
      tela.mostrarConcluido();
      break;
  }
}

}  // namespace

void setup() {
  Serial.begin(115200);
  tela.iniciar();
  mostrarPagina(paginaAtual);
  instanteUltimaPagina = millis();
}

void loop() {
  const uint32_t agora = millis();
  if ((uint32_t)(agora - instanteUltimaPagina) >= kTempoPorPaginaMs) {
    instanteUltimaPagina = agora;
    paginaAtual = (paginaAtual + 1) % kQuantidadePaginas;
    mostrarPagina(paginaAtual);
  }
}

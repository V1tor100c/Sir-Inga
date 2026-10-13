#pragma once

#include <Adafruit_ILI9341.h>
#include <stdint.h>

// Camada de apresentacao. Recebe o driver configurado pelo codigo da placa
// e desenha as telas da aplicacao sem controlar a maquina de estados.
class Tela {
 public:
  explicit Tela(Adafruit_ILI9341& display);

  void iniciar();

  void mostrarInicio();
  void mostrarConfiguracaoVazao(const uint8_t digitos[4], uint8_t digitoSelecionado);
  void mostrarConfiguracaoTempo(uint8_t tempoMinutos);
  void mostrarConfirmacao(float vazaoMlPorHora, uint8_t tempoMinutos);
  void mostrarExecucao(float vazaoMlPorHora, uint8_t tempoRestanteMinutos);
  void mostrarInvalido(const char* mensagem);
  void mostrarFalha(const char* mensagem);
  void mostrarObstrucao();
  void mostrarConcluido();

 private:
  Adafruit_ILI9341& display_;

  void desenharBase(const char* titulo, uint16_t corCabecalho);
  void desenharMensagem(const char* mensagem, uint16_t cor);
  void desenharVazao(float vazaoMlPorHora, int16_t y);
};

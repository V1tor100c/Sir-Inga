#pragma once

#include <Adafruit_ILI9341.h>
#include <stdint.h>

// Camada de apresentacao. Recebe o driver configurado pelo codigo da placa
// e desenha as telas da aplicacao sem controlar a maquina de estados.
class Tela {
 public:
  explicit Tela(Adafruit_ILI9341& display);

  void iniciar();

  void Inicio();
  void ConfiguracaoVazao(const uint8_t digitos, uint8_t digitoSelecionado);
  void ConfiguracaoTempo(uint8_t tempoMinutos);
  void Confirmacao(float vazaoMlPorHora, uint8_t tempoMinutos);
  void ConfiguracaoValida(float vazaoMlPorHora, uint8_t tempoMinutos);
  void ConfiguracaoInvalida(const char* mensagem);
  void Execucao(float vazaoMlPorHora, uint8_t tempoRestanteMinutos);
  // void Invalido(const char* mensagem);
  void Falha(const char* mensagem);
  void Obstrucao();
  void Concluido();

 private:
  Adafruit_ILI9341& display_;

  void desenharBase(const char* titulo, uint16_t corCabecalho);
  void desenharMensagem(const char* mensagem, uint16_t cor);
  void desenharVazao(float vazaoMlPorHora, int16_t y);
};

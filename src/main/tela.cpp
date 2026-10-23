#include "tela.h"

namespace {
constexpr int16_t kLargura = 320;
constexpr int16_t kAltura = 240;
constexpr uint16_t kCorFundo = ILI9341_BLACK;
constexpr uint16_t kCorTexto = ILI9341_WHITE;
constexpr uint16_t kCorRealce = ILI9341_CYAN;
constexpr uint16_t kCorAlerta = ILI9341_ORANGE;
constexpr uint16_t kCorErro = ILI9341_RED;
constexpr uint16_t kCorSucesso = ILI9341_GREEN;
}

Tela::Tela(Adafruit_ILI9341& display) : display_(display) {}

void Tela::iniciar() {
  display_.begin();
  display_.setRotation(1);  // Landscape: 320 x 240 no ILI9341.
  display_.setTextWrap(true);
  display_.fillScreen(kCorFundo);
}

void Tela::desenharBase(const char* titulo, uint16_t corCabecalho) {
  display_.fillScreen(kCorFundo);
  display_.fillRect(0, 0, kLargura, 38, corCabecalho);
  display_.setTextColor(kCorTexto);
  display_.setTextSize(2);
  display_.setCursor(12, 10);
  display_.print(titulo);
  display_.drawFastHLine(0, kAltura - 24, kLargura, ILI9341_DARKGREY);
}

void Tela::desenharMensagem(const char* mensagem, uint16_t cor) {
  display_.setTextColor(cor);
  display_.setTextSize(2);
  display_.setCursor(16, 72);
  display_.print(mensagem != nullptr ? mensagem : "");
}

void Tela::desenharVazao(float vazaoMlPorHora, int16_t y) {
  display_.setTextColor(kCorTexto);
  display_.setTextSize(2);
  display_.setCursor(18, y);
  display_.print("Vazao: ");
  display_.print(vazaoMlPorHora, 1);
  display_.print(" mL/h");
}

void Tela::Inicio() {
  desenharBase("SIR INGA", ILI9341_DARKCYAN);
  display_.setTextColor(kCorTexto);
  display_.setTextSize(2);
  display_.setCursor(24, 88);
  display_.println("Bomba de seringa");
  display_.setTextColor(kCorRealce);
  display_.setCursor(24, 142);
  display_.println("Pressione o encoder para iniciar");
}

void Tela::ConfiguracaoVazao(float vazaoMlPorHora) {
  desenharBase("CONFIGURAR VAZAO", ILI9341_BLUE);
  display_.setTextColor(kCorTexto);
  display_.setTextSize(2);
  display_.setCursor(18, 58);
  display_.println("Vazao (mL/h)");
  display_.setTextColor(kCorRealce);
  display_.setTextSize(4);
  display_.setCursor(40, 110);
  display_.print(vazaoMlPorHora, 0);
  display_.setTextSize(2);
  display_.setTextColor(kCorTexto);
  display_.setCursor(18, 184);
  display_.println("Gire: ajustar  Pressione: seguir");
}

void Tela::ConfirmacaoVazao(float vazaoMlPorHora, bool confirmarSelecionado) {
  desenharBase("CONFIRMAR VAZAO", ILI9341_BLUE);
  desenharVazao(vazaoMlPorHora, 72);

  display_.setTextSize(2);
  display_.setTextColor(confirmarSelecionado ? kCorRealce : kCorTexto);
  display_.setCursor(28, 132);
  display_.print(confirmarSelecionado ? "> Confirmar" : "  Confirmar");
  display_.setTextColor(confirmarSelecionado ? kCorTexto : kCorRealce);
  display_.setCursor(28, 164);
  display_.print(confirmarSelecionado ? "  Retornar" : "> Retornar");
}

void Tela::ConfiguracaoTempo(uint8_t tempoMinutos) {
  desenharBase("CONFIGURAR TEMPO", ILI9341_BLUE);
  display_.setTextColor(kCorTexto);
  display_.setTextSize(2);
  display_.setCursor(18, 72);
  display_.println("Duracao da infusao");
  display_.setTextSize(4);
  display_.setTextColor(kCorRealce);
  display_.setCursor(75, 118);
  display_.print(tempoMinutos);
  display_.setTextSize(2);
  display_.print(" min");
  display_.setTextSize(1);
  display_.setTextColor(kCorTexto);
  display_.setCursor(18, 184);
  display_.println("Gire: ajustar  Pressione: confirmar");
}

void Tela::Confirmacao(float vazaoMlPorHora, uint8_t tempoMinutos) {
  desenharBase("CONFIRMAR PARAMETROS", ILI9341_BLUE);
  desenharVazao(vazaoMlPorHora, 82);
  display_.setTextColor(kCorTexto);
  display_.setTextSize(2);
  display_.setCursor(18, 122);
  display_.print("Tempo: ");
  display_.print(tempoMinutos);
  display_.println(" min");
  display_.setTextSize(1);
  display_.setCursor(18, 184);
  display_.println("Confira os parametros antes de continuar");
}

void Tela::ConfiguracaoValida(float vazaoMLPorHora, uint8_t tempoMinutos){

}

void Tela::ConfiguracaoInvalida(const char* mensagem) {
  desenharBase("CONFIGURACAO INVALIDA", ILI9341_MAROON);
  desenharMensagem(mensagem != nullptr ? mensagem : "Verifique os valores", kCorAlerta);
}

void Tela::Execucao(float vazaoMlPorHora, uint8_t tempoRestanteMinutos) {
  desenharBase("INFUSAO EM ANDAMENTO", ILI9341_DARKGREEN);
  desenharVazao(vazaoMlPorHora, 82);
  display_.setTextColor(kCorTexto);
  display_.setTextSize(2);
  display_.setCursor(18, 122);
  display_.print("Restante: ");
  display_.print(tempoRestanteMinutos);
  display_.println(" min");
  display_.setTextColor(kCorAlerta);
  display_.setCursor(18, 176);
  display_.println("Somente demonstracao de bancada");
}


void Tela::Falha(const char* mensagem) {
  desenharBase("FALHA", ILI9341_MAROON);
  desenharMensagem(mensagem != nullptr ? mensagem : "Verifique o sistema", kCorErro);
  display_.setTextColor(kCorTexto);
  display_.setTextSize(1);
  display_.setCursor(16, 184);
  display_.println("Motor deve permanecer parado neste estado");
}

void Tela::Obstrucao() {
  desenharBase("OBSTRUCAO", ILI9341_MAROON);
  desenharMensagem("Verifique o caminho do fluido", kCorErro);
  display_.setTextColor(kCorTexto);
  display_.setTextSize(1);
  display_.setCursor(16, 184);
  display_.println("Alarme e parada sao tratados pela aplicacao");
}

void Tela::Concluido() {
  desenharBase("CONCLUIDO", ILI9341_DARKGREEN);
  desenharMensagem("Operacao finalizada", kCorSucesso);
}

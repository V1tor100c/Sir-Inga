#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_ILI9341.h>

// Ligacao de teste para o ESP32 DevKit do projeto.
// GPIO 18/19/21 ficam livres para o botao e o encoder ja configurados.
constexpr int8_t PIN_TELA_CS   = 5;
constexpr int8_t PIN_TELA_DC   = 27;
constexpr int8_t PIN_TELA_RST  = 26;
constexpr int8_t PIN_TELA_SCK  = 22;
constexpr int8_t PIN_TELA_MOSI = 23;

Adafruit_ILI9341 tela(PIN_TELA_CS, PIN_TELA_DC, PIN_TELA_RST);

void mostrarTeste(uint16_t cor, const char* nomeCor) {
  tela.fillScreen(cor);
  tela.setTextColor(ILI9341_WHITE);
  tela.setTextSize(3);
  tela.setCursor(20, 45);
  tela.println("Teste TFT");

  tela.setTextSize(2);
  tela.setCursor(20, 105);
  tela.println("ILI9341 / SPI");
  tela.setCursor(20, 145);
  tela.print("Cor: ");
  tela.println(nomeCor);

  tela.drawRect(10, 10, 300, 220, ILI9341_WHITE);
}

void mostrarTelaInicio() {
  tela.fillScreen(ILI9341_BLACK);      // limpa a tela
  tela.setTextColor(ILI9341_WHITE);    // define a cor do texto
  tela.setTextSize(3);                 // define o tamanho
  tela.setCursor(20, 30);              // define a posição (x, y)
  tela.println("SIR INGA");

  tela.drawRect(20, 100, 280, 60, ILI9341_CYAN);  // x, y, largura, altura, cor
  tela.setCursor(40, 120);
  tela.setTextSize(2);
  tela.println("Iniciar");
}

void setup() {
  Serial.begin(115200);

  // Inicia SPI com os pinos escolhidos acima: SCK, MISO, MOSI, SS.
  // MISO fica desconectado para este teste, que apenas escreve na tela.
  SPI.begin(PIN_TELA_SCK, -1, PIN_TELA_MOSI, PIN_TELA_CS);

  tela.begin();
  tela.setRotation(1);  // Paisagem: 320 x 240.

  Serial.println("Teste da tela iniciado");
  mostrarTeste(ILI9341_RED, "vermelho");
}

void loop() {

  mostrarTelaInicio();
  
  // static uint8_t etapa = 0;
  // static uint32_t ultimaTroca = 0;

  // if (millis() - ultimaTroca >= 2000) {
  //   ultimaTroca = millis();
  //   ++etapa;

  //   switch (etapa % 3) {
  //     case 0:
  //       mostrarTeste(ILI9341_RED, "vermelho");
  //       break;
  //     case 1:
  //       mostrarTeste(ILI9341_GREEN, "verde");
  //       break;
  //     case 2:
  //       mostrarTeste(ILI9341_BLUE, "azul");
  //       break;
  //   }
  // }
}

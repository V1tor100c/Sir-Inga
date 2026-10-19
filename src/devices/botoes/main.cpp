#include <Arduino.h>

// Troque estes pinos pelos que vocês escolherem para a placa.
constexpr uint8_t PIN_ENCODER_A  = 25;
constexpr uint8_t PIN_ENCODER_B  = 26;
constexpr uint8_t PIN_ENCODER_SW = 27;

uint8_t estadoAnteriorEncoder;
int transicoes = 0;

// Tabela para interpretar as mudanças nos sinais A e B.
const int8_t tabelaEncoder[16] = {
   0, -1,  1,  0,
   1,  0,  0, -1,
  -1,  0,  0,  1,
   0,  1, -1,  0
};

// O operador << 1 desloca o valor de A uma posição para a esquerda. 
// O operador | combina esse resultado com B. 
// Assim, as duas leituras viram um número de 0 a 3:
int lerEstadoEncoder() {
  return (digitalRead(PIN_ENCODER_A) << 1) | digitalRead(PIN_ENCODER_B);
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_ENCODER_A, INPUT_PULLUP);
  pinMode(PIN_ENCODER_B, INPUT_PULLUP);
  pinMode(PIN_ENCODER_SW, INPUT_PULLUP);

  estadoAnteriorEncoder = lerEstadoEncoder();
}

void loop() {
  lerRotacao();
  lerBotao();

  // Deixe loop rodar frequentemente para perceber as mudanças do encoder.
}

void lerRotacao() {
  const uint8_t estadoAtualEncoder = lerEstadoEncoder();
  const uint8_t indice = (estadoAnteriorEncoder << 2) | estadoAtualEncoder;

  transicoes += tabelaEncoder[indice];
  estadoAnteriorEncoder = estadoAtualEncoder;

  // Muitos encoders produzem quatro mudanças elétricas por clique.
  // O número pode variar conforme o modelo.
  if (transicoes >= 4) {
    Serial.println("Giro horario");
    transicoes = 0;
  } else if (transicoes <= -4) {
    Serial.println("Giro anti-horario");
    transicoes = 0;
  }
}

void lerBotao() {
  static bool botaoEstavaPressionado = false;
  static bool leituraAnterior = HIGH;
  static unsigned long ultimaMudanca = 0;

  const bool leitura = digitalRead(PIN_ENCODER_SW);

  if (leitura != leituraAnterior) {
    ultimaMudanca = millis();
    leituraAnterior = leitura;
  }

  // Só aceita a mudança depois de estável por 30 ms (debounce).
  if (millis() - ultimaMudanca >= 30) {
    const bool botaoPressionado = (leitura == LOW);

    if (botaoPressionado && !botaoEstavaPressionado) {
      Serial.println("Botao pressionado");
    }

    botaoEstavaPressionado = botaoPressionado;
  }
}
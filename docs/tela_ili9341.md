# Interface de tela ILI9341

A classe `Tela` desenha as paginas da interface. O driver Adafruit ILI9341 faz a comunicacao SPI; a aplicacao continua responsavel pela maquina de estados, botoes, alarmes e acionamento.

## Pinagem

O objeto `Adafruit_ILI9341` e criado pela aplicacao com os pinos `CS`, `DC` e `RST` escolhidos para a placa. O clock e os dados usam o barramento SPI de hardware selecionado pela plataforma. Defina e confira essa pinagem com o diagrama da placa e do modulo antes de ligar. O arquivo de configuracao atual ainda nao possui os pinos do display.

Exemplo de integracao depois de escolher os pinos:

```cpp
#include <Adafruit_ILI9341.h>
#include "tela.h"

Adafruit_ILI9341 display(PINO_CS, PINO_DC, PINO_RST);
Tela tela(display);

void setup() {
  tela.iniciar();
  tela.mostrarInicio();
}
```

Troque os nomes `PINO_CS`, `PINO_DC` e `PINO_RST` pelas constantes da pinagem real. Nao use essa instancia ate verificar se os pinos nao conflitam com os botoes ou outros perifericos.

## Paginas disponiveis

- `mostrarInicio()` — tela inicial.
- `mostrarConfiguracaoVazao(digitos, selecionado)` — mostra os quatro digitos e contorna o selecionado. Os digitos representam centenas, dezenas, unidades e decimos.
- `mostrarConfiguracaoTempo(minutos)` — configuracao do tempo em minutos.
- `mostrarConfirmacao(vazao, minutos)` — resumo dos parametros.
- `mostrarExecucao(vazao, restante)` — vazao e tempo restante.
- `mostrarInvalido(mensagem)` — configuracao rejeitada.
- `mostrarFalha(mensagem)` e `mostrarObstrucao()` — informacao de falha.
- `mostrarConcluido()` — operacao finalizada.

Chame a pagina apropriada ao entrar em um estado ou quando um valor exibido mudar, em vez de redesenhar a tela inteira a cada volta do `loop()`.

## Escopo

Esta interface e apenas apresentacao. Ela nao valida uma configuracao clinica, nao controla o motor e nao toca o buzzer. O firmware atual e um prototipo academico de bancada.

# Testes isolados de botao e tela

Os ambientes `Devices-BOTOES` e `Devices-TELA` compilam um teste independente do programa principal.

## Botoes

1. Confira `Config::PIN_BOTAO_1`, `Config::PIN_BOTAO_2` e a ligacao em `src/main/config.h`.
2. Compile e grave o ambiente `Devices-BOTOES`.
3. Abra o Monitor Serial em 115200 baud.
4. Aperte e solte cada botao. O monitor deve mostrar um evento `pressionado` e um evento `solto` por acao.

O sketch usa a classe `Botao` da pasta `src/main` e faz a leitura repetidamente sem bloquear o `loop()`.

## Tela ILI9341

1. Em `src/devices/tela/main.cpp`, altere `PIN_TELA_CS`, `PIN_TELA_DC` e `PIN_TELA_RST` para os pinos da sua ligacao.
2. Confira tambem os pinos SCK, MOSI e, se usado, MISO do barramento SPI escolhido para a placa.
3. Compile e grave o ambiente `Devices-TELA`.
4. A tela deve percorrer as nove paginas de exemplo, trocando a cada tres segundos.

Os numeros de pino no sketch sao apenas exemplos e nao substituem a verificacao do esquema do display e da placa. O sketch de teste da tela nao deve ser usado como firmware da bomba: ele percorre paginas de demonstracao.

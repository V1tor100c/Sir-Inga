# Exemplo de uso da classe Botao

`Botao` cuida de ler um pino e filtrar o repique mecanico (debounce). Ela nao conhece a tela nem a maquina de estados. Quando detecta uma mudanca estavel, retorna `Pressionado` ou `Solto` uma unica vez.

## Instanciar dois botoes

Depois de decidir a pinagem, crie dois objetos. Os nomes abaixo sao marcadores: substitua `PIN_BOTAO_1` e `PIN_BOTAO_2` pelos GPIOs escolhidos para a placa e o circuito reais.

```cpp
Botao botao1(PIN_BOTAO_1, INPUT_PULLUP, LOW);
Botao botao2(PIN_BOTAO_2, INPUT_PULLUP, LOW);
```

Com `INPUT_PULLUP`, o botao costuma ser ligado entre o GPIO e GND, entao o nivel pressionado e `LOW`. Confirme a ligacao antes de usar essa configuracao.

No `setup()`, chame `iniciar()` para cada botao. Em cada volta do `loop()`, chame `atualizar()` e encaminhe os eventos para a logica da aplicacao:

```cpp
const EventoBotao evento1 = botao1.atualizar();
const EventoBotao evento2 = botao2.atualizar();

if (evento1 == EventoBotao::Pressionado) {
  // Converter para um evento da aplicacao, por exemplo: Avancar.
}

if (evento2 == EventoBotao::Pressionado) {
  // Converter para um evento da aplicacao, por exemplo: Confirmar.
}
```

Os exemplos de eventos sao apenas uma possibilidade. A maquina de estados decide o significado do aperto conforme o estado atual; a classe `Botao` nao muda estados. Chame `atualizar()` frequentemente e evite `delay()` longos, para detectar apertos e solturas em tempo adequado.

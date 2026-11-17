# Sir Inga

Firmware acadêmico para uma bomba de infusão automatizada. Este README descreve o fluxo que está implementado atualmente em `src/main/main.cpp`; estados e eventos ainda incompletos aparecem sinalizados no diagrama.

## Fluxograma do programa atual

```mermaid
flowchart TD
    A([Inicialização]) --> B[setup: iniciar Serial, botão, tela e fins de curso]
    B --> C[loop: chamar lerEventos]
    C --> D[Ler evento do botão e dos dois fins de curso]
    D --> E{Estado atual é Configuração ou Calculo<br/>e falhaDetectada está ativa?}
    E -- Sim --> F[Definir estado Falha e encerrar esta atualização]
    E -- Não --> G{Selecionar estado atual}
    F --> Z[Esperar 10 ms]

    G -- Inicio --> I[Mostrar tela inicial]
    I --> I1{Botão pressionado?}
    I1 -- Sim --> I2[Definir estado Configuração]
    I1 -- Não --> Z
    I2 --> Z

    G -- Configuração --> Q{Etapa da configuração}
    Q -- Vazão --> V1[Chamar atualizarConfiguracaoVazao]
    V1 --> V2[Esperar em while até pressionar o botão<br/>e ajustar variável local de vazão]
    V2 --> V3[Executar lógica local de confirmação]
    V3 --> V4[Voltar ao próximo ciclo ainda em Configuração]
    V4 --> Z
    Q -- Tempo --> T1[Chamar atualizarConfiguracaoTempo]
    T1 --> T2[Esperar em while até pressionar o botão<br/>e ajustar variável local de tempo]
    T2 --> T3[Executar lógica local de confirmação]
    T3 --> T4[Não há transição definida para Validacao]
    T4 --> Z

    G -- Validacao --> VA1{Vazão × tempo < volumeSeginga?}
    VA1 -- Sim --> VA2[Mostrar configuração válida]
    VA2 --> VA3{Botão pressionado?}
    VA3 -- Sim --> VA4[Definir estado Calculo]
    VA3 -- Não --> Z
    VA4 --> Z
    VA1 -- Não --> X[Definir estado Invalido]
    X --> Z

    G -- Invalido --> N1[Mostrar mensagem de configuração inválida]
    N1 --> N2{Botão pressionado?}
    N2 -- Sim --> N3[Definir estado Configuração]
    N2 -- Não --> Z
    N3 --> Z

    G -- Calculo --> K1{Volume da seringa >= vazão × tempo?}
    K1 -- Sim --> K2[Calcular frequencia_motor]
    K2 --> K3{Resultado maior que zero?}
    K3 -- Sim --> K4[Definir estado Execucao]
    K3 -- Não --> X
    K4 --> Z
    K1 -- Não --> X

    G -- Execucao --> EX1{execucaoConcluida?}
    EX1 -- Sim --> EX2[Definir estado Concluido]
    EX1 -- Não --> Z
    EX2 --> Z

    G -- Falha --> F1{falhaReconhecida?}
    F1 -- Sim --> F2[Definir estado Configuração]
    F1 -- Não --> Z
    F2 --> Z

    G -- Obstrucao --> O[Case vazio: nenhuma ação implementada]
    O --> Z
    G -- Concluido --> C1[Estado terminal: nenhuma ação implementada]
    C1 --> Z

    Z --> C
```

## Observações sobre o fluxo

- `lerEventos()` lê o botão e atualiza os dois fins de curso. Os eventos dos fins de curso são armazenados, mas ainda não provocam transição de estado.
- As funções de configuração usam `while`, então bloqueiam a atualização normal da máquina enquanto esperam pelo botão.
- No estado `Configuracao`, as variáveis locais de confirmação são reiniciadas a cada ciclo. A etapa de tempo ainda não leva o programa a `Validacao`.
- `falhaDetectada`, `falhaReconhecida` e `execucaoConcluida` ainda não são preenchidos pela leitura de eventos. Por isso, os caminhos `Falha` e `Concluido` não são acionados pelos sensores atualmente.
- `Estado::Obstrucao` existe, mas seu `case` está vazio.
- O estado `Execucao` ainda não chama o motor; ele apenas verifica `execucaoConcluida`.
- O cálculo chamado `calcular_intervalo_passo()` retorna atualmente uma frequência aproximada em passos por segundo, embora o nome sugira um intervalo entre passos.

## Como atualizar o diagrama

O diagrama está escrito em Mermaid. Ao mudar uma transição ou adicionar uma ação a um estado em `main.cpp`, atualize o fluxo correspondente aqui também. O Mermaid pode ser visualizado em editores que oferecem suporte a diagramas Mermaid, incluindo a visualização do README no GitHub.

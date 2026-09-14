flowchart TD
    A([Início]) --> B[Configurar hardware:<br/>encoder, célula de carga,<br/>motor, serial]
    B --> C[Calibrar:<br/>tara da célula de carga<br/>e relação pulsos/mm]
    C --> D[/Receber vazão desejada<br/>Q ml/h/]
    D --> E[Calcular setpoint de velocidade<br/>v = Q / A_seringa]
    E --> F{Loop principal<br/>a cada Ts}

    F --> G[Ler encoder<br/>e calcular velocidade atual]
    G --> H[Ler célula de carga<br/>força no êmbolo]
    H --> I{Força > limiar?}

    I -- Sim --> J[Parar motor<br/>acionar alarme]
    J --> K([Fim / Alarme])

    I -- Não --> L[Calcular erro:<br/>e = setpoint − velocidade]
    L --> M[Atualizar PID<br/>com anti-windup]
    M --> N[Gerar saída PWM<br/>e definir direção]
    N --> O[Aplicar PWM ao motor]
    O --> F

    style A fill:#4CAF50,color:#fff
    style K fill:#F44336,color:#fff
    style J fill:#FF9800,color:#fff
    style F fill:#2196F3,color:#fff
#pragma once

#include "config.h"

enum class EstadoBuzzer : uint8_t {
    // Alarme,
    Ligar,
    Desligar
};

class Buzzer {
public:

    Buzzer(uint8_t pino) : pino_(pino) {}
    void iniciar();
    void tocar();
    void parar();

private:

    uint8_t pino_;
    uint8_t modoPino_;
    uint8_t nivelAtivo_;
};
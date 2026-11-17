#pragma once

#include "config.h"

class Motor{
    public:
    explicit Motor(uint8_t pino, uint8_t resolucao);

        void iniciar();
        void girar(uint8_t freq);
        void voltar();
        void parar();

    private:
        uint8_t pino_;
        uint8_t resolucao_;
        uint8_t freq_;
};
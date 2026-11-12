#pragma once

#include "config.h"

class Motor{
    public:
    explicit Motor(uint8_t pino);

        void iniciar();
        void girar();
        void parar();

    private:
        uint8_t pino_;
        uint8_t freq_;
};
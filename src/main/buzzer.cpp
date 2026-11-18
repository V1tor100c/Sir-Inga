#include "buzzer.h"

Buzzer::Buzzer(uint8_t pino) : pino_(pino) {}

void Buzzer::iniciar() { pinMode(pino_, OUTPUT); }

void Buzzer::tocar() { digitalWrite(pino_, HIGH); }

void Buzzer::parar() { digitalWrite(pino_, LOW); }

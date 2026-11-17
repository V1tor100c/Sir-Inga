#include "motor.h"

void Motor::iniciar(){
    ledcSetup(Config::CANAL_MOTOR, 1000, Config::RESOLUCAO_MOTOR);
    ledcAttachPin(Config::PIN_MOTOR, Config::CANAL_MOTOR);

    ledWrile(Config::CANAL_MOTOR, Config::DESLIGA_MOTOR);
}

void Motor::girar(uint8_t freq){
    digitalWrite(Config::PIN_TROCA_ROTACAO_MOTOR, LOW);
    ledcChangeFrequency(Config::CANAL_MOTOR, freq, Config::RESOLUCAO_MOTOR); ;
}

void Motor::voltar(){
    digitalWrite(Config::PIN_TROCA_ROTACAO_MOTOR, HIGH);
    ledcChangeFrequency(Config::CANAL_MOTOR, Config::FREQUENCIA_MOTOR_VOLTANDO, Config::RESOLUCAO_MOTOR); ;

}

void Motor::parar(){
    ledcChangeFrequency(Config::CANAL_MOTOR, 0, Config::RESOLUCAO_MOTOR); ;
   
}

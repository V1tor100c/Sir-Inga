#include "motor.h"

Motor::Motor(uint8_t pino, uint8_t resolucao)
            : pino_(pino),
            resolucao_(resolucao),
            freq_(0) {}

void Motor::iniciar(){
    pinMode(Config::PIN_DIR, OUTPUT);
    pinMode(Config::PIN_ENABLE_MOTOR_N, OUTPUT);
    pinMode(Config::PIN_FALHA_DRIVER_N, INPUT);
    digitalWrite(Config::PIN_ENABLE_MOTOR_N, Config::NIVEL_MOTOR_DESABILITADO);
    digitalWrite(Config::PIN_DIR, LOW);
    ledcSetup(Config::CANAL_MOTOR, 1000, Config::RESOLUCAO_MOTOR);
    ledcAttachPin(pino_, Config::CANAL_MOTOR);

    ledcWrite(Config::CANAL_MOTOR, Config::DESLIGA_MOTOR);
}

void Motor::girar(uint8_t freq){
    digitalWrite(Config::PIN_ENABLE_MOTOR_N, Config::NIVEL_MOTOR_HABILITADO);
    digitalWrite(Config::PIN_DIR, LOW);
    ledcChangeFrequency(Config::CANAL_MOTOR, freq, Config::RESOLUCAO_MOTOR); ;
}

void Motor::voltar(){
    digitalWrite(Config::PIN_ENABLE_MOTOR_N, Config::NIVEL_MOTOR_HABILITADO);
    digitalWrite(Config::PIN_DIR, HIGH);
    ledcChangeFrequency(Config::CANAL_MOTOR, Config::FREQUENCIA_MOTOR_VOLTANDO, Config::RESOLUCAO_MOTOR); ;

}

void Motor::parar(){
    ledcChangeFrequency(Config::CANAL_MOTOR, 0, Config::RESOLUCAO_MOTOR); ;
    digitalWrite(Config::PIN_ENABLE_MOTOR_N, Config::NIVEL_MOTOR_DESABILITADO);
   
}

#include "motor.h"

Motor::Motor(uint8_t pinoStep, uint8_t resolucao)
    : pinoStep_(pinoStep), resolucao_(resolucao) {}

bool Motor::iniciar() {
  pinMode(Config::PIN_DIR, OUTPUT);
  pinMode(Config::PIN_ENABLE_MOTOR_N, OUTPUT);
  pinMode(Config::PIN_FALHA_DRIVER_N, Config::MODO_FALHA_DRIVER);
  digitalWrite(Config::PIN_ENABLE_MOTOR_N, Config::NIVEL_MOTOR_DESABILITADO);
  digitalWrite(Config::PIN_DIR, LOW);

  if (ledcSetup(Config::CANAL_MOTOR,
                Config::FREQUENCIA_PASSO_MIN_HZ,
                resolucao_) == 0) {
    return false;
  }
  ledcAttachPin(pinoStep_, Config::CANAL_MOTOR);

  ledcWrite(Config::CANAL_MOTOR, 0);
  inicializado_ = true;
  return true;
}

bool Motor::definirVelocidade(uint32_t frequenciaPassosHz, bool sentidoHorario) {
  if (!inicializado_ || frequenciaPassosHz == 0) {
    parar();
    return false;
  }

  const uint32_t frequenciaLimitada = constrain(
      frequenciaPassosHz,
      Config::FREQUENCIA_PASSO_MIN_HZ,
      Config::FREQUENCIA_PASSO_MAX_HZ);

  digitalWrite(Config::PIN_DIR, sentidoHorario ? LOW : HIGH);
  if (frequenciaLimitada != frequenciaAtualHz_) {
    if (ledcChangeFrequency(Config::CANAL_MOTOR,
                            frequenciaLimitada,
                            resolucao_) == 0) {
      parar();
      return false;
    }
    frequenciaAtualHz_ = frequenciaLimitada;
  }

  // Um duty diferente de zero faz o LEDC produzir os pulsos no pino STEP.
  ledcWrite(Config::CANAL_MOTOR, Config::DUTY_MOTOR_PASSO);
  digitalWrite(Config::PIN_ENABLE_MOTOR_N, Config::NIVEL_MOTOR_HABILITADO);
  habilitado_ = true;
  return true;
}

void Motor::parar() {
  if (inicializado_) {
    ledcWrite(Config::CANAL_MOTOR, 0);
  }
  digitalWrite(Config::PIN_ENABLE_MOTOR_N, Config::NIVEL_MOTOR_DESABILITADO);
  habilitado_ = false;
}

bool Motor::falhaDriverAtiva() const {
  return digitalRead(Config::PIN_FALHA_DRIVER_N) == Config::NIVEL_FALHA_DRIVER;
}

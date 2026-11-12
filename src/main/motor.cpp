#include "motor.h"

void Motor::iniciar(){
    ledcAttachPin(PIN_MOTOR_, 1000, 0);
}

void Motor::girar(){

}

void Motor::parar(){

}

#include "criaturas/criatura.h"

Criatura::Criatura(const std::string& nombre, int posX, int posY, int vidaMax,
                   int nivel, int danioMin, int danioMax, int fuerza,
                   float cooldownAtaque, float cooldownMovimiento):
    Character(nombre, posX, posY, vidaMax),
    nivel(nivel), danioMin(danioMin), danioMax(danioMax), fuerza(fuerza),
    tiempoDesdeUltimoAtaque(cooldownAtaque),
    tiempoDesdeUltimoMovimiento(cooldownMovimiento) {}

int Criatura::getNivel() const { return nivel; }
int Criatura::getDanioMin() const { return danioMin; }
int Criatura::getDanioMax() const { return danioMax; }
int Criatura::getFuerza() const { return fuerza; }

float Criatura::getTiempoDesdeUltimoAtaque() const {
    return tiempoDesdeUltimoAtaque;
}
float Criatura::getTiempoDesdeUltimoMovimiento() const {
    return tiempoDesdeUltimoMovimiento;
}

void Criatura::actualizarTiempoAtaque(float dt) {
    tiempoDesdeUltimoAtaque += dt;
}
void Criatura::actualizarTiempoMovimiento(float dt) {
    tiempoDesdeUltimoMovimiento += dt;
}

void Criatura::resetearCooldownAtaque() { tiempoDesdeUltimoAtaque = 0.0f; }
void Criatura::resetearCooldownMovimiento() {
    tiempoDesdeUltimoMovimiento = 0.0f;
}

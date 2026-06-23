#include "game/characters/criatura.h"

#include "game/config.h"

Criatura::Criatura(const std::string& tipo, const Config& config, int posX,
                   int posY):
    Character(tipo, posX, posY, config.getCriaturaVidaMax(tipo)),
    tipo_(tipo), nivel(config.getCriaturaNivel(tipo)),
    danioMin(config.getCriaturaDanioMin(tipo)),
    danioMax(config.getCriaturaDanioMax(tipo)),
    fuerza(config.getCriaturaFuerza(tipo)),
    tiempoDesdeUltimoAtaque(config.getCriaturaCooldownAtaque()),
    tiempoDesdeUltimoMovimiento(config.getCriaturaCooldownMovimiento()) {}

std::string Criatura::getTipo() const { return tipo_; }

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

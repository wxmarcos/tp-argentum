#pragma once
#include <string>

#include "game/characters/character.h"

class Config;

class Criatura: public Character {
    std::string tipo_;
    int nivel;
    int danioMin;
    int danioMax;
    int fuerza;
    float tiempoDesdeUltimoAtaque;
    float tiempoDesdeUltimoMovimiento;

public:
    Criatura(const std::string& tipo, const Config& config, int posX, int posY);
    std::string getTipo() const;
    bool esCriatura() const override { return true; }

    int getNivel() const;
    int getDanioMin() const;
    int getDanioMax() const;
    int getFuerza() const;

    float getTiempoDesdeUltimoAtaque() const;
    float getTiempoDesdeUltimoMovimiento() const;
    void actualizarTiempoAtaque(float dt);
    void actualizarTiempoMovimiento(float dt);
    void resetearCooldownAtaque();
    void resetearCooldownMovimiento();
};

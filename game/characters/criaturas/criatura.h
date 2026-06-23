#pragma once
#include <string>

#include "game/characters/character.h"

class Criatura: public Character {
protected:
    int nivel;
    int danioMin;
    int danioMax;
    int fuerza;
    float tiempoDesdeUltimoAtaque;
    float tiempoDesdeUltimoMovimiento;

public:
    Criatura(const std::string& nombre, int posX, int posY, int vidaMax,
             int nivel, int danioMin, int danioMax, int fuerza,
             float cooldownAtaque, float cooldownMovimiento);
    virtual ~Criatura() = default;
    virtual std::string getTipo() const = 0;
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

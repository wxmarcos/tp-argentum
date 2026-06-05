#pragma once
#include "item.h"

enum class TipoHechizo { DANIO, CURACION };

class Baculo: public Item {
    std::string nombre;
    std::string nombreHechizo;
    TipoHechizo tipoHechizo;
    int efectoMin;
    int efectoMax;
    int costoMana;

public:
    Baculo(const std::string& nombre, const std::string& nombreHechizo,
           TipoHechizo tipoHechizo, int efectoMin, int efectoMax,
           int costoMana);

    std::string getNombre() const override;
    TipoItem getTipo() const override;
    bool esApilable() const override;

    const std::string& getNombreHechizo() const;
    TipoHechizo getTipoHechizo() const;
    int getEfectoMin() const;
    int getEfectoMax() const;
    int getCostoMana() const;
};

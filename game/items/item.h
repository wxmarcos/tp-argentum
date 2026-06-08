#pragma once
#include <string>

enum class TipoItem { ARMA, BACULO, ARMADURA, CASCO, ESCUDO, POCION, ORO };

class Item {
public:
    virtual std::string getNombre() const = 0;
    virtual TipoItem getTipo() const = 0;
    virtual bool esApilable() const { return false; }
    virtual ~Item() = default;
};

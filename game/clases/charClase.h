#pragma once
#include <string>

class Config;

class charClase {
    std::string nombre_;
    float fClaseVida_;
    float fClaseMana_;
    float fClaseMeditacion_;
    bool puedeMeditar_;
    bool puedeUsarMagia_;

public:
    charClase(const std::string& nombre, const Config& config,
              bool puedeMeditar, bool puedeUsarMagia);

    std::string getNombre() const;
    float getFClaseVida() const;
    float getFClaseMana() const;
    float getFClaseMeditacion() const;
    bool puedeMeditar() const;
    bool puedeUsarMagia() const;
};

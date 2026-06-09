#include "clases/clerigo.h"

Clerigo::Clerigo(const Config& config):
    fClaseVida(config.getClaseFVida("clerigo")),
    fClaseMana(config.getClaseFMana("clerigo")),
    fClaseMeditacion(config.getClaseFMeditacion("clerigo")) {}

std::string Clerigo::getNombre() const { return "Clerigo"; }
float Clerigo::getFClaseVida() const { return fClaseVida; }
float Clerigo::getFClaseMana() const { return fClaseMana; }
float Clerigo::getFClaseMeditacion() const { return fClaseMeditacion; }
bool Clerigo::puedeMeditar() const { return true; }
bool Clerigo::puedeUsarMagia() const { return true; }

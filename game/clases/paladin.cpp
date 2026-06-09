#include "clases/paladin.h"

Paladin::Paladin(const Config& config):
    fClaseVida(config.getClaseFVida("paladin")),
    fClaseMana(config.getClaseFMana("paladin")),
    fClaseMeditacion(config.getClaseFMeditacion("paladin")) {}

std::string Paladin::getNombre() const { return "Paladin"; }
float Paladin::getFClaseVida() const { return fClaseVida; }
float Paladin::getFClaseMana() const { return fClaseMana; }
float Paladin::getFClaseMeditacion() const { return fClaseMeditacion; }
bool Paladin::puedeMeditar() const { return true; }
bool Paladin::puedeUsarMagia() const { return true; }

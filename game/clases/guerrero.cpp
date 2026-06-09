#include "clases/guerrero.h"

Guerrero::Guerrero(const Config& config):
    fClaseVida(config.getClaseFVida("guerrero")),
    fClaseMana(config.getClaseFMana("guerrero")),
    fClaseMeditacion(config.getClaseFMeditacion("guerrero")) {}

std::string Guerrero::getNombre() const { return "Guerrero"; }
float Guerrero::getFClaseVida() const { return fClaseVida; }
float Guerrero::getFClaseMana() const { return fClaseMana; }
float Guerrero::getFClaseMeditacion() const { return fClaseMeditacion; }
bool Guerrero::puedeMeditar() const { return false; }
bool Guerrero::puedeUsarMagia() const { return false; }

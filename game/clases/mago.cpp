#include "clases/mago.h"

Mago::Mago(const Config& config):
    fClaseVida(config.getClaseFVida("mago")),
    fClaseMana(config.getClaseFMana("mago")),
    fClaseMeditacion(config.getClaseFMeditacion("mago")) {}

std::string Mago::getNombre() const { return "Mago"; }
float Mago::getFClaseVida() const { return fClaseVida; }
float Mago::getFClaseMana() const { return fClaseMana; }
float Mago::getFClaseMeditacion() const { return fClaseMeditacion; }
bool Mago::puedeMeditar() const { return true; }
bool Mago::puedeUsarMagia() const { return true; }

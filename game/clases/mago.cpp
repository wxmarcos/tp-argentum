#include "mago.h"

std::string Mago::getNombre() const { return "Mago"; }
int Mago::getMaxVida() const { return 300; }                    // TODO: cargar desde Config
int Mago::getMaxMana() const { return 600; }                    // TODO: cargar desde Config
bool Mago::puedeMeditar() const { return true; }
bool Mago::puedeUsarMagia() const { return true; }

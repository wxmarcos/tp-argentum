#include "paladin.h"

std::string Paladin::getNombre() const { return "Paladin"; }
int Paladin::getMaxVida() const { return 600; }                             // TODO: cargar desde Config
int Paladin::getMaxMana() const { return 200; }                             // TODO: cargar desde Config
bool Paladin::puedeMeditar() const { return true; }
bool Paladin::puedeUsarMagia() const { return true; }
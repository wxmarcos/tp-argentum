#include "clerigo.h"

std::string Clerigo::getNombre() const { return "Clerigo"; }
int Clerigo::getMaxVida() const { return 400; }                           // TODO: cargar desde Config
int Clerigo::getMaxMana() const { return 400; }                           // TODO: cargar desde Config
bool Clerigo::puedeMeditar() const { return true; }
bool Clerigo::puedeUsarMagia() const { return true; }
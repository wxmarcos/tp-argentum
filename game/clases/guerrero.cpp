#include "guerrero.h"

std::string Guerrero::getNombre() const { return "Guerrero"; }
int Guerrero::getMaxVida() const { return 800; }                        // TODO: cargar desde Config
int Guerrero::getMaxMana() const { return 0; }                          // Guerrero siempre tiene mana 0
bool Guerrero::puedeMeditar() const { return false; }
bool Guerrero::puedeUsarMagia() const { return false; }
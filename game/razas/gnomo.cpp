#include "gnomo.h"

std::string Gnomo::getNombre() const { return "Gnomo"; }
int Gnomo::getMaxVida() const { return 350; }                 // TODO: cargar desde Config
int Gnomo::getMaxMana() const { return 450; }                 // TODO: cargar desde Config
float Gnomo::getFRecuperacion() const { return 0.9f; }        // TODO: cargar desde Config
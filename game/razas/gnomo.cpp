#include "gnomo.h"

std::string Gnomo::getNombre() { return "Gnomo"; }
int Gnomo::getMaxVida() { return 350; }                 // TODO: cargar desde Config
int Gnomo::getMaxMana() { return 450; }                 // TODO: cargar desde Config
float Gnomo::getFRecuperacion() { return 0.9f; }        // TODO: cargar desde Config
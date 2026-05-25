#include "enano.h"

std::string Enano::getNombre() const { return "Enano"; }
int Enano::getMaxVida() const { return 600; }                 // TODO: cargar desde Config
int Enano::getMaxMana() const { return 100; }                 // TODO: cargar desde Config
float Enano::getFRecuperacion() const { return 0.8f; }        // TODO: cargar desde Config
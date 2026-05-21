#include "humano.h"

std::string Humano::getNombre() const { return "Humano"; }
int Humano::getMaxVida() const { return 500; }                  // TODO: cargar desde Config
int Humano::getMaxMana() const { return 300; }                  // TODO: cargar desde Config
float Humano::getFRecuperacion() const { return 1.0f; }         // TODO: cargar desde Config
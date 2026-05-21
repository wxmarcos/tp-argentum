#include "elfo.h"

std::string Elfo::getNombre() const { return "Elfo"; }
int Elfo::getMaxVida() const { return 400; }                    // TODO: cargar desde Config
int Elfo::getMaxMana() const { return 500; }                    // TODO: cargar desde Config
float Elfo::getFRecuperacion() const { return 1.2f; }           // TODO: cargar desde Config
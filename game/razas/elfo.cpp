#include "razas/elfo.h"

Elfo::Elfo(const Config& config):
    constitucionBase(config.getRazaConstitucionBase("elfo")),
    inteligenciaBase(config.getRazaInteligenciaBase("elfo")),
    fuerzaBase(config.getRazaFuerzaBase("elfo")),
    agilidadBase(config.getRazaAgilidadBase("elfo")),
    fRazaVida(config.getRazaFVida("elfo")),
    fRazaMana(config.getRazaFMana("elfo")),
    fRazaRecuperacion(config.getRazaFRecuperacion("elfo")) {}

std::string Elfo::getNombre() const { return "Elfo"; }
int Elfo::getConstitucionBase() const { return constitucionBase; }
int Elfo::getInteligenciaBase() const { return inteligenciaBase; }
int Elfo::getFuerzaBase() const { return fuerzaBase; }
int Elfo::getAgilidadBase() const { return agilidadBase; }
float Elfo::getFRazaVida() const { return fRazaVida; }
float Elfo::getFRazaMana() const { return fRazaMana; }
float Elfo::getFRazaRecuperacion() const { return fRazaRecuperacion; }

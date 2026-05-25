#include "razas/humano.h"

Humano::Humano(const Config& config)
    : constitucionBase(config.getRazaConstitucionBase("humano")),
      inteligenciaBase(config.getRazaInteligenciaBase("humano")),
      fuerzaBase(config.getRazaFuerzaBase("humano")),
      agilidadBase(config.getRazaAgilidadBase("humano")),
      fRazaVida(config.getRazaFVida("humano")),
      fRazaMana(config.getRazaFMana("humano")),
      fRazaRecuperacion(config.getRazaFRecuperacion("humano")) {}

std::string Humano::getNombre() const { return "Humano"; }
int Humano::getConstitucionBase() const { return constitucionBase; }
int Humano::getInteligenciaBase() const { return inteligenciaBase; }
int Humano::getFuerzaBase() const { return fuerzaBase; }
int Humano::getAgilidadBase() const { return agilidadBase; }
float Humano::getFRazaVida() const { return fRazaVida; }
float Humano::getFRazaMana() const { return fRazaMana; }
float Humano::getFRazaRecuperacion() const { return fRazaRecuperacion; }
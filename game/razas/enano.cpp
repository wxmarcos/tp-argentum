#include "razas/enano.h"

Enano::Enano(const Config& config)
    : constitucionBase(config.getRazaConstitucionBase("enano")),
      inteligenciaBase(config.getRazaInteligenciaBase("enano")),
      fuerzaBase(config.getRazaFuerzaBase("enano")),
      agilidadBase(config.getRazaAgilidadBase("enano")),
      fRazaVida(config.getRazaFVida("enano")),
      fRazaMana(config.getRazaFMana("enano")),
      fRazaRecuperacion(config.getRazaFRecuperacion("enano")) {}

std::string Enano::getNombre() const { return "Enano"; }
int Enano::getConstitucionBase() const { return constitucionBase; }
int Enano::getInteligenciaBase() const { return inteligenciaBase; }
int Enano::getFuerzaBase() const { return fuerzaBase; }
int Enano::getAgilidadBase() const { return agilidadBase; }
float Enano::getFRazaVida() const { return fRazaVida; }
float Enano::getFRazaMana() const { return fRazaMana; }
float Enano::getFRazaRecuperacion() const { return fRazaRecuperacion; }
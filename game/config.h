#pragma once

#include <string>
#include <stdexcept>

class Config {
public:
    explicit Config(const std::string& rutaArchivo);

    // Inventario
    int getInventarioCapacidadMax() const;

    // Razas - indice por nombre
    int getRazaConstitucionBase(const std::string& raza) const;
    int getRazaInteligenciaBase(const std::string& raza) const;
    int getRazaFuerzaBase(const std::string& raza) const;
    int getRazaAgilidadBase(const std::string& raza) const;
    float getRazaFVida(const std::string& raza) const;
    float getRazaFMana(const std::string& raza) const;
    float getRazaFRecuperacion(const std::string& raza) const;

    // Clases - indice por nombre
    float getClaseFVida(const std::string& clase) const;
    float getClaseFMana(const std::string& clase) const;
    float getClaseFMeditacion(const std::string& clase) const;

    // Formulas
    int getFormulaCriticoPorcentaje() const;
    double getFormulaExpCoeficiente() const;
    double getFormulaExpExponente() const;
    double getFormulaOroMaxCoeficiente() const;
    double getFormulaOroMaxExponente() const;
    int getFormulaOroDropNPCDivisor() const;

private:
    struct Impl;
    Impl* impl;
};
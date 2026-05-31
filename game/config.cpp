#include "config.h"

#include <toml++/toml.hpp>
#include <stdexcept>
#include <string>

// Impl — contiene el árbol TOML parseado

struct Config::Impl {
    toml::table tabla;

    explicit Impl(const std::string& ruta) {
        try {
            tabla = toml::parse_file(ruta);
        } catch (const toml::parse_error& e) {
            throw std::runtime_error(
                std::string("Error al parsear config.toml: ") + e.what());
        }
    }

    // Helper
    template<typename T>
    T get(const std::string& clave, T defecto) const {
        if (auto val = tabla.at_path(clave).value<T>())
            return *val;
        return defecto;
    }
};

// Constructor

Config::Config(const std::string& rutaArchivo)
    : impl(new Impl(rutaArchivo)) {}

// Inventario

int Config::getInventarioCapacidadMax() const {
    return impl->get<int64_t>("inventario.capacidad_max", 30);
}

// Razas

int Config::getRazaConstitucionBase(const std::string& raza) const {
    std::string clave = "razas." + raza + ".constitucion_base";
    if (raza == "humano") return impl->get<int64_t>(clave, 300);
    if (raza == "elfo")   return impl->get<int64_t>(clave, 200);
    if (raza == "enano")  return impl->get<int64_t>(clave, 450);
    if (raza == "gnomo")  return impl->get<int64_t>(clave, 250);
    return impl->get<int64_t>(clave, 300);
}

int Config::getRazaInteligenciaBase(const std::string& raza) const {
    std::string clave = "razas." + raza + ".inteligencia_base";
    if (raza == "humano") return impl->get<int64_t>(clave, 300);
    if (raza == "elfo")   return impl->get<int64_t>(clave, 450);
    if (raza == "enano")  return impl->get<int64_t>(clave, 150);
    if (raza == "gnomo")  return impl->get<int64_t>(clave, 400);
    return impl->get<int64_t>(clave, 300);
}

int Config::getRazaFuerzaBase(const std::string& raza) const {
    std::string clave = "razas." + raza + ".fuerza_base";
    if (raza == "humano") return impl->get<int64_t>(clave, 300);
    if (raza == "elfo")   return impl->get<int64_t>(clave, 250);
    if (raza == "enano")  return impl->get<int64_t>(clave, 400);
    if (raza == "gnomo")  return impl->get<int64_t>(clave, 200);
    return impl->get<int64_t>(clave, 300);
}

int Config::getRazaAgilidadBase(const std::string& raza) const {
    std::string clave = "razas." + raza + ".agilidad_base";
    if (raza == "humano") return impl->get<int64_t>(clave, 300);
    if (raza == "elfo")   return impl->get<int64_t>(clave, 450);
    if (raza == "enano")  return impl->get<int64_t>(clave, 200);
    if (raza == "gnomo")  return impl->get<int64_t>(clave, 350);
    return impl->get<int64_t>(clave, 300);
}

float Config::getRazaFVida(const std::string& raza) const {
    std::string clave = "razas." + raza + ".f_raza_vida";
    if (raza == "humano") return static_cast<float>(impl->get<double>(clave, 1.0));
    if (raza == "elfo")   return static_cast<float>(impl->get<double>(clave, 0.8));
    if (raza == "enano")  return static_cast<float>(impl->get<double>(clave, 1.3));
    if (raza == "gnomo")  return static_cast<float>(impl->get<double>(clave, 0.9));
    return static_cast<float>(impl->get<double>(clave, 1.0));
}

float Config::getRazaFMana(const std::string& raza) const {
    std::string clave = "razas." + raza + ".f_raza_mana";
    if (raza == "humano") return static_cast<float>(impl->get<double>(clave, 1.0));
    if (raza == "elfo")   return static_cast<float>(impl->get<double>(clave, 1.3));
    if (raza == "enano")  return static_cast<float>(impl->get<double>(clave, 0.7));
    if (raza == "gnomo")  return static_cast<float>(impl->get<double>(clave, 1.2));
    return static_cast<float>(impl->get<double>(clave, 1.0));
}

float Config::getRazaFRecuperacion(const std::string& raza) const {
    std::string clave = "razas." + raza + ".f_recuperacion";
    if (raza == "humano") return static_cast<float>(impl->get<double>(clave, 1.0));
    if (raza == "elfo")   return static_cast<float>(impl->get<double>(clave, 1.2));
    if (raza == "enano")  return static_cast<float>(impl->get<double>(clave, 0.8));
    if (raza == "gnomo")  return static_cast<float>(impl->get<double>(clave, 0.9));
    return static_cast<float>(impl->get<double>(clave, 1.0));
}

// Clases

float Config::getClaseFVida(const std::string& clase) const {
    std::string clave = "clases." + clase + ".f_clase_vida";
    if (clase == "guerrero") return static_cast<float>(impl->get<double>(clave, 1.5));
    if (clase == "mago")     return static_cast<float>(impl->get<double>(clave, 0.8));
    if (clase == "clerigo")  return static_cast<float>(impl->get<double>(clave, 1.0));
    if (clase == "paladin")  return static_cast<float>(impl->get<double>(clave, 1.3));
    return static_cast<float>(impl->get<double>(clave, 1.0));
}

float Config::getClaseFMana(const std::string& clase) const {
    std::string clave = "clases." + clase + ".f_clase_mana";
    if (clase == "guerrero") return static_cast<float>(impl->get<double>(clave, 0.0));
    if (clase == "mago")     return static_cast<float>(impl->get<double>(clave, 1.5));
    if (clase == "clerigo")  return static_cast<float>(impl->get<double>(clave, 1.0));
    if (clase == "paladin")  return static_cast<float>(impl->get<double>(clave, 0.7));
    return static_cast<float>(impl->get<double>(clave, 0.0));
}

float Config::getClaseFMeditacion(const std::string& clase) const {
    std::string clave = "clases." + clase + ".f_clase_meditacion";
    if (clase == "guerrero") return static_cast<float>(impl->get<double>(clave, 0.0));
    if (clase == "mago")     return static_cast<float>(impl->get<double>(clave, 2.0));
    if (clase == "clerigo")  return static_cast<float>(impl->get<double>(clave, 1.5));
    if (clase == "paladin")  return static_cast<float>(impl->get<double>(clave, 1.0));
    return static_cast<float>(impl->get<double>(clave, 0.0));
}

// Fórmulas

int Config::getFormulaCriticoPorcentaje() const {
    return impl->get<int64_t>("formulas.critico_porcentaje", 20);
}

double Config::getFormulaExpCoeficiente() const {
    return impl->get<double>("formulas.exp_coeficiente", 1000.0);
}

double Config::getFormulaExpExponente() const {
    return impl->get<double>("formulas.exp_exponente", 1.8);
}

double Config::getFormulaOroMaxCoeficiente() const {
    return impl->get<double>("formulas.oro_max_coeficiente", 100.0);
}

double Config::getFormulaOroMaxExponente() const {
    return impl->get<double>("formulas.oro_max_exponente", 1.1);
}

int Config::getFormulaOroDropNPCDivisor() const {
    return impl->get<int64_t>("formulas.oro_drop_npc_divisor", 10);
}

std::vector<Config::ConfigMapa> Config::getMapas() const {
    std::vector<ConfigMapa> resultado;

    auto* seccionMapas = impl->tabla["mapas"].as_table();
    if (!seccionMapas) return resultado;

    for (auto& [clave, valor] : *seccionMapas) {
        auto* t = valor.as_table();
        if (!t) continue;

        ConfigMapa cm;
        cm.id               = std::stoi(std::string(clave.str()));
        cm.ancho            = t->get("ancho")           ? (int)(*t->get("ancho")->value<int64_t>())         : 100;
        cm.alto             = t->get("alto")            ? (int)(*t->get("alto")->value<int64_t>())          : 100;
        cm.vecinoNorte      = t->get("vecino_norte")    ? (int)(*t->get("vecino_norte")->value<int64_t>())  : -1;
        cm.vecinoSur        = t->get("vecino_sur")      ? (int)(*t->get("vecino_sur")->value<int64_t>())    : -1;
        cm.vecinoEste       = t->get("vecino_este")     ? (int)(*t->get("vecino_este")->value<int64_t>())   : -1;
        cm.vecinoOeste      = t->get("vecino_oeste")    ? (int)(*t->get("vecino_oeste")->value<int64_t>())  : -1;
        resultado.push_back(cm);
    }
    
    return resultado;
}
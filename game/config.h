#pragma once

#include <stdexcept>
#include <string>
#include <vector>

class Config {
public:
    explicit Config(const std::string& rutaArchivo);
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    Config(Config&&) = delete;
    Config& operator=(Config&&) = delete;
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
    int getFormulaExpPenalidadPorcentaje() const;
    double getFormulaExpCoeficiente() const;
    double getFormulaExpExponente() const;
    double getFormulaOroMaxCoeficiente() const;
    double getFormulaOroMaxExponente() const;
    int getFormulaOroDropNPCDivisor() const;
    double getFormulaEsquiveUmbral() const;
    int getFormulaExpNivelOffset() const;
    double getFormulaExpMatarFactor() const;

    struct ConfigTile {
        int x;
        int y;
        std::string tipo;
        bool transitable;
    };
    // IA de criaturas
    int getCriaturaRangoDeteccion() const;
    float getCriaturaCooldownMovimiento() const;
    float getCriaturaCooldownAtaque() const;
    float getSpawnIntervalo() const;
    float getPersistenciaIntervalo() const;
    float getVelocidadResurreccion() const;

    // Criaturas
    int getCriaturaVidaMax(const std::string& tipo) const;
    int getCriaturaNivel(const std::string& tipo) const;
    int getCriaturaDanioMin(const std::string& tipo) const;
    int getCriaturaDanioMax(const std::string& tipo) const;
    int getCriaturaFuerza(const std::string& tipo) const;
    // Servidor
    int getServerMapWidth() const;
    int getServerMapHeight() const;
    int getServerTicksPerSecond() const;
    int getServerMaxClients() const;
    // Spawn
    int getSpawnMapaId() const;
    int getSpawnX() const;
    int getSpawnY() const;
    // Mapas
    struct PosicionNPC {
        int x, y;
    };

    struct ConfigPortal {
        int x, y;
        int mapaDestino;
        int destinoX, destinoY;
    };

    struct ConfigMapa {
        int id, ancho, alto, vecinoNorte, vecinoSur, vecinoEste, vecinoOeste;

        std::string archivoTmx;

        bool esZonaSegura;
        int poblacionMax;

        std::vector<std::string> criaturasPosibles;
        std::vector<PosicionNPC> sacerdotes;
        std::vector<PosicionNPC> comerciantes;
        std::vector<PosicionNPC> banqueros;
        std::vector<ConfigPortal> portales;
    };

    std::vector<ConfigMapa> getMapas() const;

    // Precio de items
    int getPrecioItem(const std::string& nombre) const;

    // Rutas de Persistencia
    std::string getRutaJugadores() const;
    std::string getRutaIndiceJugadores() const;
    std::string getRutaClanes() const;
    ~Config();

private:
    struct Impl;
    Impl* impl;
};

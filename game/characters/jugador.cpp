#include "game/characters/jugador.h"

#include <algorithm>
#include <cmath>
#include <optional>

#include "game/formulas.h"
#include "game/items/item.h"

Jugador::Jugador(const std::string& nombre, int posX, int posY,
                 const Raza* raza, const CharClase* clase,
                 int capacidadInventario):
    Character(nombre, posX, posY, 1),
    raza(raza), clase(clase), constitucion(raza->getConstitucionBase()),
    inteligencia(raza->getInteligenciaBase()), fuerza(raza->getFuerzaBase()),
    agilidad(raza->getAgilidadBase()), manaActual(0), manaMax(0), nivel(1),
    experiencia(0), oro(0), vidaAcumulada(0.0f), manaAcumulado(0.0f),
    meditando(false), inventario(capacidadInventario), resucitando(false),
    tiempoResucitando(0.0f), destinoMapaId(0), destinoPosX(0), destinoPosY(0),
    cheatVidaInfinita(false), cheatManaInfinito(false) {
    recalcularStats();
}

void Jugador::recalcularStats() {
    vidaMax = Formulas::calcularVidaMax(constitucion, clase->getFClaseVida(),
                                        raza->getFRazaVida(), nivel);
    vidaActual = vidaMax;
    manaMax = Formulas::calcularManaMax(inteligencia, clase->getFClaseMana(),
                                        raza->getFRazaMana(), nivel);
    manaActual = manaMax;
}

void Jugador::restaurarEstado(int nuevoNivel, int nuevaVida, int nuevaVidaMax,
                              int nuevoMana, int nuevoManaMax,
                              int nuevaExperiencia, int nuevoOro,
                              int nuevaConstitucion, int nuevaInteligencia,
                              int nuevaFuerza, int nuevaAgilidad) {
    nivel = nuevoNivel;
    vidaMax = nuevaVidaMax;
    vidaActual = nuevaVida;

    manaMax = nuevoManaMax;
    manaActual = nuevoMana;

    experiencia = nuevaExperiencia;
    oro = nuevoOro;

    constitucion = nuevaConstitucion;
    inteligencia = nuevaInteligencia;
    fuerza = nuevaFuerza;
    agilidad = nuevaAgilidad;

    vivo = vidaActual > 0;
}

// ----------------------- Getters básicos -----------------------
const Raza* Jugador::getRaza() const { return raza; }
const CharClase* Jugador::getClase() const { return clase; }
int Jugador::getConstitucion() const { return constitucion; }
int Jugador::getInteligencia() const { return inteligencia; }
int Jugador::getFuerza() const { return fuerza; }
int Jugador::getAgilidad() const { return agilidad; }

// ----------------------- Mana -----------------------
int Jugador::getManaActual() const { return manaActual; }
int Jugador::getManaMax() const { return manaMax; }

bool Jugador::gastarMana(int cantidad) {
    if (!clase->puedeUsarMagia()) return false;
    if (cheatManaInfinito) return true;
    if (manaActual < cantidad) return false;
    manaActual -= cantidad;
    return true;
}

void Jugador::recuperarMana(int cantidad) {
    if (!clase->puedeMeditar()) return;
    manaActual = std::min(manaMax, manaActual + cantidad);
}

// ----------------------- Nivel y Experiencia -----------------------
int Jugador::getNivel() const { return nivel; }
int Jugador::getExperiencia() const { return experiencia; }

int Jugador::expParaSiguienteNivel() const {
    return Formulas::calcularLimiteExp(nivel);
}

void Jugador::verificarSubidaNivel() {
    while (experiencia >= expParaSiguienteNivel()) {
        experiencia -= expParaSiguienteNivel();
        nivel++;
        recalcularStats();
    }
}

void Jugador::ganarExperiencia(int exp) {
    if (exp <= 0) return;
    experiencia += exp;
    verificarSubidaNivel();
}

// ----------------------- Oro -----------------------
int Jugador::getOro() const { return oro; }
int Jugador::getOroMax() const { return Formulas::calcularOroMax(nivel); }

void Jugador::agregarOro(int cantidad) {
    if (cantidad <= 0) return;
    int tope = static_cast<int>(getOroMax() * 1.5);
    oro = std::min(tope, oro + cantidad);
}

bool Jugador::gastarOro(int cantidad) {
    if (cantidad > oro) return false;
    oro -= cantidad;
    return true;
}

// ----------------------- Meditacion -----------------------
void Jugador::iniciarMeditacion() {
    if (!clase->puedeMeditar() || !vivo) return;
    meditando = true;
}

void Jugador::interrumpirMeditacion() { meditando = false; }
bool Jugador::estaMeditando() const { return meditando; }

// ----------------------- Inventario -----------------------
std::optional<int> Jugador::agarrarItem(std::unique_ptr<Item> item,
                                        int cantidad) {
    if (!vivo) {
        return std::nullopt;
    }

    interrumpirMeditacion();

    return inventario.agregar(std::move(item), cantidad);
}

std::optional<SlotInventario> Jugador::soltarItem(int indice, int cantidad) {
    interrumpirMeditacion();
    return inventario.soltar(indice, cantidad);
}

std::vector<SlotInventario> Jugador::soltarTodosLosItems() {
    return inventario.soltarTodo();
}

bool Jugador::equiparArma(int indice) {
    interrumpirMeditacion();
    return inventario.equiparArma(indice);
}

bool Jugador::equiparBaculo(int indice) {
    interrumpirMeditacion();
    return inventario.equiparBaculo(indice);
}

bool Jugador::equiparArmadura(int indice) {
    interrumpirMeditacion();
    return inventario.equiparArmadura(indice);
}

bool Jugador::equiparCasco(int indice) {
    interrumpirMeditacion();
    return inventario.equiparCasco(indice);
}

bool Jugador::equiparEscudo(int indice) {
    interrumpirMeditacion();
    return inventario.equiparEscudo(indice);
}

void Jugador::desequiparArma() {
    interrumpirMeditacion();
    inventario.desequiparArmaOBaculo();
}

void Jugador::desequiparArmadura() {
    interrumpirMeditacion();
    inventario.desequiparArmadura();
}
void Jugador::desequiparCasco() {
    interrumpirMeditacion();
    inventario.desequiparCasco();
}
void Jugador::desequiparEscudo() {
    interrumpirMeditacion();
    inventario.desequiparEscudo();
}

bool Jugador::usarPocion(int indice) {
    if (!vivo) return false;
    interrumpirMeditacion();
    return inventario.usarPocion(indice, *this);
}

const Inventario& Jugador::getInventario() const { return inventario; }

// ----------------------- Recuperacion pasiva -----------------------
void Jugador::recuperacionPasiva(float dt) {
    if (!vivo) return;

    vidaAcumulada +=
        Formulas::calcularRecuperacionVida(raza->getFRazaRecuperacion(), dt);
    if (vidaAcumulada >= 1.0f) {
        curar(static_cast<int>(vidaAcumulada));
        vidaAcumulada -= static_cast<int>(vidaAcumulada);
    }

    float recMana;
    if (meditando) {
        recMana = Formulas::calcularRecuperacionManaMeditando(
            clase->getFClaseMeditacion(), inteligencia, dt);
    } else {
        recMana = Formulas::calcularRecuperacionMana(
            raza->getFRazaRecuperacion(), dt);
    }
    manaAcumulado += recMana;
    if (manaAcumulado >= 1.0f) {
        recuperarMana(static_cast<int>(manaAcumulado));
        manaAcumulado -= static_cast<int>(manaAcumulado);
    }
}

// ----------------------- Resurreccion -----------------------
int Jugador::getDestinoMapaId() const { return destinoMapaId; }
int Jugador::getDestinoPosX() const { return destinoPosX; }
int Jugador::getDestinoPosY() const { return destinoPosY; }

void Jugador::iniciarResurreccion(float tiempo, int mapaId, int posX,
                                  int posY) {
    if (vivo || resucitando) return;
    resucitando = true;
    tiempoResucitando = tiempo;
    destinoMapaId = mapaId;
    destinoPosX = posX;
    destinoPosY = posY;
}

void Jugador::tickResurreccion(float dt) {
    if (!resucitando) return;
    tiempoResucitando -= dt;
}

bool Jugador::estaResucitando() const { return resucitando; }
bool Jugador::resurreccionCompleta() const {
    return resucitando && tiempoResucitando <= 0.0f;
}

// ----------------------- Cheats -----------------------
void Jugador::activarCheatVidaInfinita() { cheatVidaInfinita = true; }
void Jugador::activarCheatManaInfinito() { cheatManaInfinito = true; }

void Jugador::morir() {
    if (cheatVidaInfinita) return;
    Character::morir();
    interrumpirMeditacion();
}

#include "characters/jugador.h"
#include "items/item.h"
#include "formulas.h"

#include <algorithm>
#include <optional>

Jugador::Jugador(const std::string& nombre, int posX, int posY,
                 const Raza* raza, const charClase* clase)
    : Character(nombre, posX, posY, 1),
      raza(raza),
      clase(clase),
      constitucion(raza->getConstitucionBase()),
      inteligencia(raza->getInteligenciaBase()),
      fuerza(raza->getFuerzaBase()),
      agilidad(raza->getAgilidadBase()),
      manaActual(0),
      manaMax(0),
      nivel(1),
      experiencia(0),
      oro(0),
      vidaAcumulada(0.0f),
      manaAcumulado(0.0f),
      meditando(false) {
        recalcularStats();
}

void Jugador::recalcularStats() {
    vidaMax = Formulas::calcularVidaMax(constitucion, clase->getFClaseVida(), raza->getFRazaVida(), nivel);
    vidaActual = vidaMax;
    manaMax = Formulas::calcularManaMax(inteligencia, clase->getFClaseMana(), raza->getFRazaMana(), nivel);
    manaActual = manaMax;
}

const Raza* Jugador::getRaza() const { return raza; }
const charClase* Jugador::getClase() const { return clase; }
int Jugador::getConstitucion() const { return constitucion; }
int Jugador::getInteligencia() const { return inteligencia; }
int Jugador::getFuerza() const { return fuerza; }
int Jugador::getAgilidad() const { return agilidad; }

int Jugador::getManaActual() const { return manaActual; }
int Jugador::getManaMax() const { return manaMax; }

void Jugador::gastarMana(int cantidad) {
    if (!clase->puedeUsarMagia()) return;
    manaActual = std::max(0, manaActual - cantidad);
}

void Jugador::recuperarMana(int cantidad) {
    if (!clase->puedeMeditar()) return;
    manaActual = std::min(manaMax, manaActual + cantidad);
}

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

int Jugador::getOro() const { return oro; }

void Jugador::agregarOro(int cantidad) {
    if (cantidad > 0) oro += cantidad;
}

bool Jugador::gastarOro(int cantidad) {
    if (cantidad > oro) return false;
    oro -= cantidad;
    return true;
}

void Jugador::iniciarMeditacion() {
    if (!clase->puedeMeditar()) return;
    if (!vivo) return;
    meditando = true;
}

void Jugador::interrumpirMeditacion() {
    meditando = false;
}

bool Jugador::estaMeditando() const { return meditando; }

bool Jugador::agarrarItem(std::unique_ptr<Item> item, int cantidad) {
    if (!vivo) return false;
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

void Jugador::recuperacionPasiva(float dt) {
    if (!vivo) return;

    vidaAcumulada += Formulas::calcularRecuperacionVida(raza->getFRazaRecuperacion(), dt);
    if (vidaAcumulada >= 1.0f) {
        curar(static_cast<int>(vidaAcumulada));
        vidaAcumulada -= static_cast<int>(vidaAcumulada);
    }

    float recMana;
    if (meditando) {
        recMana = Formulas::calcularRecuperacionManaMeditando(clase->getFClaseMeditacion(), inteligencia, dt);
    } else {
        recMana = Formulas::calcularRecuperacionMana(raza->getFRazaRecuperacion(), dt);
    }
    manaAcumulado += recMana;
    if (manaAcumulado >= 1.0f) {
        recuperarMana(static_cast<int>(manaAcumulado));
        manaAcumulado -= static_cast<int>(manaAcumulado);
    }
}

void Jugador::morir() {
    Character::morir();
    interrumpirMeditacion();
}
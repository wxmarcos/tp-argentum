#include "clan.h"
#include <algorithm>

Clan::Clan(const std::string& nombre, const std::string& fundador)
    : nombre(nombre), fundador(fundador) {
    miembros.push_back(fundador);
}

const std::string& Clan::getNombre() const { return nombre; }
const std::string& Clan::getFundador() const { return fundador; }

bool Clan::agregarMiembro(const std::string& nombre) {
    if (esMiembro(nombre)) return false;
    miembros.push_back(nombre);
    return true;
}

bool Clan::removerMiembro(const std::string& nombre) {
    if (nombre == fundador) return false;  // el fundador no puede salir
    auto it = std::find(miembros.begin(), miembros.end(), nombre);
    if (it == miembros.end()) return false;
    miembros.erase(it);
    return true;
}

bool Clan::esMiembro(const std::string& nombre) const {
    return std::find(miembros.begin(), miembros.end(), nombre) != miembros.end();
}

const std::vector<std::string>& Clan::getMiembros() const { return miembros; }

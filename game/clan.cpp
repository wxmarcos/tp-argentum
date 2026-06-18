#include "game/clan.h"

#include <algorithm>

Clan::Clan(const std::string& nombre, const std::string& fundador):
    nombre(nombre), fundador(fundador) {
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
    return std::find(miembros.begin(), miembros.end(), nombre) !=
           miembros.end();
}

const std::vector<std::string>& Clan::getMiembros() const { return miembros; }

void Clan::agregarSolicitud(const std::string& nombre) {
    if (!hayPendiente(nombre) && !esMiembro(nombre))
        solicitudes.push_back(nombre);
}

bool Clan::hayPendiente(const std::string& nombre) const {
    return std::find(solicitudes.begin(), solicitudes.end(), nombre) !=
           solicitudes.end();
}

bool Clan::aprobarSolicitud(const std::string& nombre) {
    auto it = std::find(solicitudes.begin(), solicitudes.end(), nombre);
    if (it == solicitudes.end()) return false;
    solicitudes.erase(it);
    miembros.push_back(nombre);
    return true;
}

bool Clan::rechazarSolicitud(const std::string& nombre) {
    auto it = std::find(solicitudes.begin(), solicitudes.end(), nombre);
    if (it == solicitudes.end()) return false;
    solicitudes.erase(it);
    return true;
}

const std::vector<std::string>& Clan::getSolicitudes() const {
    return solicitudes;
}

void Clan::banear(const std::string& nombre) {
    baneados.insert(nombre);
}

bool Clan::esBaneado(const std::string& nombre) const {
    return baneados.count(nombre) > 0;
}

const std::set<std::string>& Clan::getBaneados() const {
    return baneados;
}
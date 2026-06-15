#pragma once
#include <string>
#include <vector>

class Clan {
private:
    std::string nombre;
    std::string fundador;
    std::vector<std::string> miembros;
    std::vector<std::string> solicitudes;  // solicitudes de ingreso pendientes

public:
    Clan(const std::string& nombre, const std::string& fundador);
    const std::string& getNombre() const;
    const std::string& getFundador() const;
    bool agregarMiembro(const std::string& nombre);
    bool removerMiembro(const std::string& nombre);
    bool esMiembro(const std::string& nombre) const;
    const std::vector<std::string>& getMiembros() const;

    // Solicitudes de ingreso
    void agregarSolicitud(const std::string& nombre);
    bool hayPendiente(const std::string& nombre) const;
    bool aprobarSolicitud(const std::string& nombre);
    bool rechazarSolicitud(const std::string& nombre);
    const std::vector<std::string>& getSolicitudes() const;
};

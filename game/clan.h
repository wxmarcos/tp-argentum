#pragma once
#include <string>
#include <vector>

class Clan {
private:
    std::string nombre;
    std::string fundador;
    std::vector<std::string> miembros;

public:
    Clan(const std::string& nombre, const std::string& fundador);
    const std::string& getNombre() const;
    const std::string& getFundador() const;
    bool agregarMiembro(const std::string& nombre);
    bool removerMiembro(const std::string& nombre);
    bool esMiembro(const std::string& nombre) const;
    const std::vector<std::string>& getMiembros() const;
};

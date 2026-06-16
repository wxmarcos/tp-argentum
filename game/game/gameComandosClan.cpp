#include "game/game.h"

#include "common/protocol_defs.h"
#include "game/clan.h"

// ----------------- CLAN CREATE -----------------

void Game::handleClanCreate(const std::string& nombre, const Command& cmd,
                            std::vector<Snapshot>& snapshots) {
    std::string clanNom = cmd.get_clan_name();
    Jugador* j = getJugador(nombre);
    if (!j) return;

    if (clanNom.empty()) {
        snapshots.push_back(Snapshot::error_message(
            nombre, "El nombre del clan no puede ser vacio"));
        return;
    }
    if (j->estaEnClan()) {
        snapshots.push_back(
            Snapshot::error_message(nombre, "Ya perteneces a un clan"));
        return;
    }
    if (clanes.count(clanNom)) {
        snapshots.push_back(Snapshot::error_message(
            nombre, "Ya existe un clan con ese nombre"));
        return;
    }
    if (j->getNivel() < 6) {
        snapshots.push_back(Snapshot::error_message(
            nombre, "Debes ser nivel 6 o superior para fundar un clan"));
        return;
    }

    clanes.emplace(clanNom, Clan(clanNom, nombre));
    j->setClanNombre(clanNom);
    snapshots.push_back(Snapshot::chat_message(
        "Sistema", nombre,
        "Clan '" + clanNom + "' creado. Eres el fundador."));
}

// ----------------- CLAN JOIN -----------------

void Game::handleClanJoin(const std::string& nombre, const Command& cmd,
                          std::vector<Snapshot>& snapshots) {
    std::string clanNom = cmd.get_clan_name();
    Jugador* j = getJugador(nombre);
    if (!j) return;

    if (j->estaEnClan()) {
        snapshots.push_back(
            Snapshot::error_message(nombre, "Ya perteneces a un clan"));
        return;
    }
    auto it = clanes.find(clanNom);
    if (it == clanes.end()) {
        snapshots.push_back(
            Snapshot::error_message(nombre, "El clan no existe"));
        return;
    }
    Clan& clan = it->second;
    if (clan.esBaneado(nombre)) {
        snapshots.push_back(Snapshot::error_message(
            nombre, "Fuiste expulsado de ese clan y no puedes reingresar"));
        return;
    }
    if (clan.esMiembro(nombre)) {
        snapshots.push_back(Snapshot::error_message(
            nombre, "Ya eres miembro de ese clan"));
        return;
    }
    if (clan.hayPendiente(nombre)) {
        snapshots.push_back(Snapshot::error_message(
            nombre, "Ya tienes una solicitud pendiente en ese clan"));
        return;
    }
    if (static_cast<int>(clan.getMiembros().size()) >= Clan::MAX_MIEMBROS) {
        snapshots.push_back(Snapshot::error_message(
            nombre, "El clan ya tiene el maximo de miembros"));
        return;
    }

    clan.agregarSolicitud(nombre);
    snapshots.push_back(Snapshot::chat_message(
        "Sistema", nombre, "Solicitud enviada al clan '" + clanNom + "'"));

    const std::string& fundador = clan.getFundador();
    if (jugadores.count(fundador)) {
        snapshots.push_back(Snapshot::chat_message(
            "Sistema", fundador, nombre + " quiere unirse a tu clan."));
    }
}

// ----------------- CLAN REVIEW -----------------

void Game::handleClanReview(const std::string& nombre,
                            std::vector<Snapshot>& snapshots) {
    Jugador* j = getJugador(nombre);
    if (!j || !j->estaEnClan()) {
        snapshots.push_back(
            Snapshot::error_message(nombre, "No perteneces a un clan"));
        return;
    }
    auto it = clanes.find(j->getClanNombre());
    if (it == clanes.end()) return;

    Clan& clan = it->second;
    if (clan.getFundador() != nombre) {
        snapshots.push_back(Snapshot::error_message(
            nombre, "Solo el fundador puede ver las solicitudes"));
        return;
    }

    const auto& sols = clan.getSolicitudes();
    if (sols.empty()) {
        snapshots.push_back(Snapshot::chat_message(
            "Sistema", nombre, "No hay solicitudes pendientes."));
    } else {
        std::string lista = "Solicitudes pendientes: ";
        for (size_t i = 0; i < sols.size(); ++i) {
            if (i > 0) lista += ", ";
            lista += sols[i];
        }
        snapshots.push_back(Snapshot::chat_message("Sistema", nombre, lista));
    }
}

// ----------------- CLAN ACCEPT -----------------

void Game::handleClanAccept(const std::string& nombre, const Command& cmd,
                            std::vector<Snapshot>& snapshots) {
    std::string nickSol = cmd.get_nick();
    Jugador* j = getJugador(nombre);
    if (!j || !j->estaEnClan()) {
        snapshots.push_back(
            Snapshot::error_message(nombre, "No perteneces a un clan"));
        return;
    }
    auto it = clanes.find(j->getClanNombre());
    if (it == clanes.end()) return;

    Clan& clan = it->second;
    if (clan.getFundador() != nombre) {
        snapshots.push_back(Snapshot::error_message(
            nombre, "Solo el fundador puede aceptar solicitudes"));
        return;
    }
    if (!clan.hayPendiente(nickSol)) {
        snapshots.push_back(Snapshot::error_message(
            nombre, "No hay solicitud pendiente de " + nickSol));
        return;
    }
    if (static_cast<int>(clan.getMiembros().size()) >= Clan::MAX_MIEMBROS) {
        snapshots.push_back(Snapshot::error_message(
            nombre, "El clan ya tiene el maximo de miembros"));
        return;
    }

    clan.aprobarSolicitud(nickSol);
    Jugador* ingresante = getJugador(nickSol);
    if (ingresante) {
        ingresante->setClanNombre(j->getClanNombre());
        snapshots.push_back(Snapshot::chat_message(
            "Sistema", nickSol,
            "Tu solicitud al clan '" + j->getClanNombre() + "' fue aceptada!"));
    }
    snapshots.push_back(Snapshot::chat_message(
        "Sistema", nombre, nickSol + " ahora es miembro del clan."));
}

// ----------------- CLAN REJECT -----------------

void Game::handleClanReject(const std::string& nombre, const Command& cmd,
                            std::vector<Snapshot>& snapshots) {
    std::string nickSol = cmd.get_nick();
    Jugador* j = getJugador(nombre);
    if (!j || !j->estaEnClan()) {
        snapshots.push_back(
            Snapshot::error_message(nombre, "No perteneces a un clan"));
        return;
    }
    auto it = clanes.find(j->getClanNombre());
    if (it == clanes.end()) return;

    Clan& clan = it->second;
    if (clan.getFundador() != nombre) {
        snapshots.push_back(Snapshot::error_message(
            nombre, "Solo el fundador puede rechazar solicitudes"));
        return;
    }
    if (!clan.rechazarSolicitud(nickSol)) {
        snapshots.push_back(Snapshot::error_message(
            nombre, "No hay solicitud pendiente de " + nickSol));
        return;
    }

    Jugador* solicitante = getJugador(nickSol);
    if (solicitante) {
        snapshots.push_back(Snapshot::chat_message(
            "Sistema", nickSol,
            "Tu solicitud al clan '" + j->getClanNombre() + "' fue rechazada."));
    }
    snapshots.push_back(Snapshot::chat_message(
        "Sistema", nombre, "Solicitud de " + nickSol + " rechazada."));
}

// ----------------- CLAN BAN / KICK -----------------

void Game::handleClanBanKick(const std::string& nombre, const Command& cmd,
                             std::vector<Snapshot>& snapshots) {
    std::string nickTarget = cmd.get_nick();
    Jugador* j = getJugador(nombre);
    if (!j || !j->estaEnClan()) {
        snapshots.push_back(
            Snapshot::error_message(nombre, "No perteneces a un clan"));
        return;
    }
    auto it = clanes.find(j->getClanNombre());
    if (it == clanes.end()) return;

    Clan& clan = it->second;
    if (clan.getFundador() != nombre) {
        snapshots.push_back(Snapshot::error_message(
            nombre, "Solo el fundador puede expulsar miembros"));
        return;
    }
    if (nickTarget == nombre) {
        snapshots.push_back(Snapshot::error_message(
            nombre, "No puedes expulsarte a ti mismo"));
        return;
    }
    if (!clan.removerMiembro(nickTarget)) {
        snapshots.push_back(Snapshot::error_message(
            nombre, nickTarget + " no es miembro del clan"));
        return;
    }

    const bool esBan = (cmd.get_type() == protocol::ClientOpcode::CLAN_BAN);
    if (esBan) {
        clan.banear(nickTarget);
    }

    Jugador* expulsado = getJugador(nickTarget);
    if (expulsado) {
        expulsado->setClanNombre("");
        const std::string razon = esBan ? "expulsado con ban" : "expulsado";
        snapshots.push_back(Snapshot::chat_message(
            "Sistema", nickTarget,
            "Fuiste " + razon + " del clan '" + j->getClanNombre() + "'."));
    }
    const std::string accion = esBan ? "baneado" : "expulsado";
    snapshots.push_back(Snapshot::chat_message(
        "Sistema", nombre, nickTarget + " fue " + accion + " del clan."));
}

// ----------------- CLAN LEAVE -----------------

void Game::handleClanLeave(const std::string& nombre,
                           std::vector<Snapshot>& snapshots) {
    Jugador* j = getJugador(nombre);
    if (!j || !j->estaEnClan()) {
        snapshots.push_back(
            Snapshot::error_message(nombre, "No perteneces a un clan"));
        return;
    }
    auto it = clanes.find(j->getClanNombre());
    if (it == clanes.end()) {
        j->setClanNombre("");
        return;
    }

    Clan& clan = it->second;
    if (clan.getFundador() == nombre) {
        snapshots.push_back(Snapshot::error_message(
            nombre, "El fundador no puede abandonar el clan"));
        return;
    }

    clan.removerMiembro(nombre);
    std::string clanNom = j->getClanNombre();
    j->setClanNombre("");
    snapshots.push_back(Snapshot::chat_message(
        "Sistema", nombre, "Abandonaste el clan '" + clanNom + "'."));
}

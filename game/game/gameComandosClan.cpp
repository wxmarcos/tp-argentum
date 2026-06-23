#include "common/protocol_defs.h"
#include "game/clan.h"
#include "game/game.h"

// ----------------- CLAN CREATE -----------------

void Game::handleClanCreate(const std::string& nombre, const Command& cmd,
                            std::vector<OutgoingSnapshot>& snapshots,
                            uint16_t playerId) {
    std::string clanNom = cmd.get_clan_name();
    Jugador* j = getJugador(nombre);
    if (!j) return;

    if (clanNom.empty()) {
        push_unicast(snapshots,
                     Snapshot::error_message(
                         nombre, "El nombre del clan no puede ser vacio"),
                     playerId);
        return;
    }
    if (j->estaEnClan()) {
        push_unicast(snapshots,
                     Snapshot::error_message(nombre, "Ya perteneces a un clan"),
                     playerId);
        return;
    }
    if (clanes.count(clanNom)) {
        push_unicast(
            snapshots,
            Snapshot::error_message(nombre, "Ya existe un clan con ese nombre"),
            playerId);
        return;
    }
    if (j->getNivel() < 6) {
        push_unicast(
            snapshots,
            Snapshot::error_message(
                nombre, "Debes ser nivel 6 o superior para fundar un clan"),
            playerId);
        return;
    }

    for (auto& [nom, c] : clanes) {
        c.rechazarSolicitud(nombre);
    }

    clanes.emplace(clanNom, Clan(clanNom, nombre));
    j->setClanNombre(clanNom);
    push_unicast(snapshots,
                 Snapshot::chat_message(
                     "Clanes", nombre,
                     "Clan '" + clanNom + "' creado. Eres el fundador."),
                 playerId);
}

// ----------------- CLAN JOIN -----------------

void Game::handleClanJoin(const std::string& nombre, const Command& cmd,
                          std::vector<OutgoingSnapshot>& snapshots,
                          uint16_t playerId) {
    std::string clanNom = cmd.get_clan_name();
    Jugador* j = getJugador(nombre);
    if (!j) return;

    if (j->estaEnClan()) {
        push_unicast(snapshots,
                     Snapshot::error_message(nombre, "Ya perteneces a un clan"),
                     playerId);
        return;
    }
    auto it = clanes.find(clanNom);
    if (it == clanes.end()) {
        push_unicast(snapshots,
                     Snapshot::error_message(nombre, "El clan no existe"),
                     playerId);
        return;
    }
    Clan& clan = it->second;
    if (clan.esBaneado(nombre)) {
        push_unicast(
            snapshots,
            Snapshot::error_message(
                nombre, "Fuiste expulsado de ese clan y no puedes reingresar"),
            playerId);
        return;
    }
    if (clan.esMiembro(nombre)) {
        push_unicast(
            snapshots,
            Snapshot::error_message(nombre, "Ya eres miembro de ese clan"),
            playerId);
        return;
    }
    if (clan.hayPendiente(nombre)) {
        push_unicast(
            snapshots,
            Snapshot::error_message(
                nombre, "Ya tienes una solicitud pendiente en ese clan"),
            playerId);
        return;
    }
    if (static_cast<int>(clan.getMiembros().size()) >=
        config.getClanMaxMiembros()) {
        push_unicast(snapshots,
                     Snapshot::error_message(
                         nombre, "El clan ya tiene el maximo de miembros"),
                     playerId);
        return;
    }

    clan.agregarSolicitud(nombre);
    push_unicast(
        snapshots,
        Snapshot::chat_message("Clanes", nombre,
                               "Solicitud enviada al clan '" + clanNom + "'"),
        playerId);

    const std::string& fundador = clan.getFundador();
    auto itFundadorId = nick_to_player_id.find(fundador);
    if (itFundadorId != nick_to_player_id.end()) {
        push_unicast(
            snapshots,
            Snapshot::chat_message("Clanes", fundador,
                                   nombre + " quiere unirse a tu clan."),
            itFundadorId->second);
    }
}

// ----------------- CLAN REVIEW -----------------

void Game::handleClanReview(const std::string& nombre,
                            std::vector<OutgoingSnapshot>& snapshots,
                            uint16_t playerId) {
    Jugador* j = getJugador(nombre);
    if (!j || !j->estaEnClan()) {
        push_unicast(snapshots,
                     Snapshot::error_message(nombre, "No perteneces a un clan"),
                     playerId);
        return;
    }
    auto it = clanes.find(j->getClanNombre());
    if (it == clanes.end()) return;

    Clan& clan = it->second;
    if (clan.getFundador() != nombre) {
        push_unicast(snapshots,
                     Snapshot::error_message(
                         nombre, "Solo el fundador puede ver las solicitudes"),
                     playerId);
        return;
    }

    const auto& sols = clan.getSolicitudes();
    const auto& miembros = clan.getMiembros();

    std::string listaMiembros = "Miembros: ";
    for (size_t i = 0; i < miembros.size(); ++i) {
        if (i > 0) listaMiembros += ", ";
        listaMiembros += miembros[i];
    }

    push_unicast(snapshots,
                 Snapshot::chat_message("Clanes", nombre, listaMiembros),
                 playerId);
    if (sols.empty()) {
        push_unicast(snapshots,
                     Snapshot::chat_message("Clanes", nombre,
                                            "No hay solicitudes pendientes."),
                     playerId);
    } else {
        std::string lista = "Solicitudes pendientes: ";
        for (size_t i = 0; i < sols.size(); ++i) {
            if (i > 0) lista += ", ";
            lista += sols[i];
        }

        push_unicast(snapshots, Snapshot::chat_message("Clanes", nombre, lista),
                     playerId);
    }
}

// ----------------- CLAN ACCEPT -----------------

void Game::handleClanAccept(const std::string& nombre, const Command& cmd,
                            std::vector<OutgoingSnapshot>& snapshots,
                            uint16_t playerId) {
    std::string nickSol = cmd.get_nick();
    Jugador* j = getJugador(nombre);
    if (!j || !j->estaEnClan()) {
        push_unicast(snapshots,
                     Snapshot::error_message(nombre, "No perteneces a un clan"),
                     playerId);
        return;
    }
    auto it = clanes.find(j->getClanNombre());
    if (it == clanes.end()) return;

    Clan& clan = it->second;
    if (clan.getFundador() != nombre) {
        push_unicast(snapshots,
                     Snapshot::error_message(
                         nombre, "Solo el fundador puede aceptar solicitudes"),
                     playerId);
        return;
    }
    if (!clan.hayPendiente(nickSol)) {
        push_unicast(snapshots,
                     Snapshot::error_message(
                         nombre, "No hay solicitud pendiente de " + nickSol),
                     playerId);
        return;
    }
    if (static_cast<int>(clan.getMiembros().size()) >=
        config.getClanMaxMiembros()) {
        push_unicast(snapshots,
                     Snapshot::error_message(
                         nombre, "El clan ya tiene el maximo de miembros"),
                     playerId);
        return;
    }

    Jugador* ingresante = getJugador(nickSol);
    if (ingresante && ingresante->estaEnClan()) {
        clan.rechazarSolicitud(nickSol);
        push_unicast(snapshots,
                     Snapshot::error_message(
                         nombre, nickSol + " ya pertenece a otro clan"),
                     playerId);
        return;
    }

    clan.aprobarSolicitud(nickSol);

    for (auto& [nom, c] : clanes) {
        if (nom != j->getClanNombre()) {
            c.rechazarSolicitud(nickSol);
        }
    }

    if (ingresante) {
        ingresante->setClanNombre(j->getClanNombre());
        auto itId = nick_to_player_id.find(nickSol);
        if (itId != nick_to_player_id.end()) {
            push_unicast(snapshots,
                         Snapshot::chat_message("Clanes", nickSol,
                                                "Tu solicitud al clan '" +
                                                    j->getClanNombre() +
                                                    "' fue aceptada!"),
                         itId->second);
        }
    }
    push_unicast(snapshots,
                 Snapshot::chat_message("Clanes", nombre,
                                        nickSol + " ahora es miembro del clan"),
                 playerId);
}

// ----------------- CLAN REJECT -----------------

void Game::handleClanReject(const std::string& nombre, const Command& cmd,
                            std::vector<OutgoingSnapshot>& snapshots,
                            uint16_t playerId) {
    std::string nickSol = cmd.get_nick();
    Jugador* j = getJugador(nombre);
    if (!j || !j->estaEnClan()) {
        push_unicast(snapshots,
                     Snapshot::error_message(nombre, "No perteneces a un clan"),
                     playerId);
        return;
    }
    auto it = clanes.find(j->getClanNombre());
    if (it == clanes.end()) return;

    Clan& clan = it->second;
    if (clan.getFundador() != nombre) {
        push_unicast(snapshots,
                     Snapshot::error_message(
                         nombre, "Solo el fundador puede rechazar solicitudes"),
                     playerId);
        return;
    }
    if (!clan.rechazarSolicitud(nickSol)) {
        push_unicast(snapshots,
                     Snapshot::error_message(
                         nombre, "No hay solicitud pendiente de " + nickSol),
                     playerId);
        return;
    }

    Jugador* solicitante = getJugador(nickSol);
    if (solicitante) {
        auto itId = nick_to_player_id.find(nickSol);
        if (itId != nick_to_player_id.end()) {
            push_unicast(snapshots,
                         Snapshot::chat_message("Clanes", nickSol,
                                                "Tu solicitud al clan '" +
                                                    j->getClanNombre() +
                                                    "' fue rechazada"),
                         itId->second);
        }
    }
    push_unicast(
        snapshots,
        Snapshot::chat_message("Clanes", nombre,
                               "Solicitud de " + nickSol + " rechazada"),
        playerId);
}

// ----------------- CLAN BAN / KICK -----------------

void Game::handleClanBanKick(const std::string& nombre, const Command& cmd,
                             std::vector<OutgoingSnapshot>& snapshots,
                             uint16_t playerId) {
    std::string nickTarget = cmd.get_nick();
    Jugador* j = getJugador(nombre);
    if (!j || !j->estaEnClan()) {
        push_unicast(snapshots,
                     Snapshot::error_message(nombre, "No perteneces a un clan"),
                     playerId);
        return;
    }
    auto it = clanes.find(j->getClanNombre());
    if (it == clanes.end()) return;

    Clan& clan = it->second;
    if (clan.getFundador() != nombre) {
        push_unicast(snapshots,
                     Snapshot::error_message(
                         nombre, "Solo el fundador puede expulsar miembros"),
                     playerId);
        return;
    }
    if (nickTarget == nombre) {
        push_unicast(
            snapshots,
            Snapshot::error_message(nombre, "No puedes expulsarte a ti mismo"),
            playerId);
        return;
    }
    if (!clan.removerMiembro(nickTarget)) {
        push_unicast(snapshots,
                     Snapshot::error_message(
                         nombre, nickTarget + " no es miembro del clan"),
                     playerId);
        return;
    }

    const bool esBan = (cmd.get_type() == protocol::ClientOpcode::CLAN_BAN);
    if (esBan) {
        clan.banear(nickTarget);
    }

    Jugador* expulsado = getJugador(nickTarget);
    if (expulsado) {
        expulsado->setClanNombre("");
        const std::string razon =
            esBan ? "baneado del clan '" + j->getClanNombre() +
                        "'. No podras reingresar"
                  : "expulsado del clan '" + j->getClanNombre() + "'";
        auto itExpulsadoId = nick_to_player_id.find(nickTarget);
        if (itExpulsadoId != nick_to_player_id.end()) {
            push_unicast(
                snapshots,
                Snapshot::chat_message("Clanes", nickTarget, "Fuiste " + razon),
                itExpulsadoId->second);
        }
    }
    const std::string accion = esBan ? "baneado" : "expulsado";
    push_unicast(
        snapshots,
        Snapshot::chat_message("Clanes", nombre,
                               nickTarget + " fue " + accion + " del clan"),
        playerId);
}

// ----------------- CLAN LEAVE -----------------

void Game::handleClanLeave(const std::string& nombre,
                           std::vector<OutgoingSnapshot>& snapshots,
                           uint16_t playerId) {
    Jugador* j = getJugador(nombre);
    if (!j || !j->estaEnClan()) {
        push_unicast(snapshots,
                     Snapshot::error_message(nombre, "No perteneces a un clan"),
                     playerId);
        return;
    }
    auto it = clanes.find(j->getClanNombre());
    if (it == clanes.end()) {
        j->setClanNombre("");

        return;
    }

    Clan& clan = it->second;
    if (clan.getFundador() == nombre) {
        push_unicast(snapshots,
                     Snapshot::error_message(
                         nombre, "El fundador no puede abandonar el clan"),
                     playerId);
        return;
    }

    clan.removerMiembro(nombre);
    std::string clanNom = j->getClanNombre();
    j->setClanNombre("");
    push_unicast(snapshots,
                 Snapshot::chat_message(
                     "Clanes", nombre, "Abandonaste el clan '" + clanNom + "'"),
                 playerId);
    const std::string fundador = clan.getFundador();
    auto itFundadorId = nick_to_player_id.find(fundador);
    if (itFundadorId != nick_to_player_id.end()) {
        push_unicast(snapshots,
                     Snapshot::chat_message(
                         "Clanes", fundador,
                         nombre + " abandono el clan '" + clanNom + "'"),
                     itFundadorId->second);
    }
}

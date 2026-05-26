#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include <string>

#include "protocol/client_command.h"
#include "protocol/game_update.h"

class Protocol {
    public:
    // Convierte un comando del cliente a su representacion en el wire.
    static std::string serialize(const ClientCommand& cmd);

    // Parsea un mensaje recibido del servidor a un GameUpdate.
    static GameUpdate parse(const std::string& message);

    // Helpers de conversion de direccion - texto del wire.
    static std::string direction_to_wire(protocol::Direction dir);
    static protocol::Direction direction_from_wire(const std::string& token);
};

#endif

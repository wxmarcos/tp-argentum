#include "game/items/oro.h"
#include "game/items/item_defs.h"

Oro::Oro(int cantidad) : cantidad(cantidad) {}

std::string Oro::getNombre() const { return item_defs::ORO; }
TipoItem Oro::getTipo() const { return TipoItem::ORO; }
int Oro::getCantidad() const { return cantidad; }

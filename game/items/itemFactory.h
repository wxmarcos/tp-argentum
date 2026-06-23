#pragma once
#include <memory>
#include <string>

#include "game/config.h"
#include "game/items/item.h"
#include "game/items/oro.h"

class ItemFactory {
public:
    ItemFactory() = delete;

    static std::unique_ptr<Item> crear(const std::string& nombre,
                                       const Config& config);
    static std::unique_ptr<Oro> crearOro(int cantidad);
};

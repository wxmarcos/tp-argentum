target_sources(taller_game PRIVATE
    # Razas
    game/razas/raza.cpp
    game/razas/humano.cpp
    game/razas/elfo.cpp
    game/razas/enano.cpp
    game/razas/gnomo.cpp

    # Clases
    game/clases/charClases.cpp
    game/clases/mago.cpp
    game/clases/clerigo.cpp
    game/clases/paladin.cpp
    game/clases/guerrero.cpp

    # Characters
    game/characters/character.cpp
    game/characters/jugador.cpp

    # NPCs (stubs por ahora)
    game/characters/npcs/npc.cpp
    game/characters/npcs/sacerdote.cpp
    game/characters/npcs/comerciante.cpp
    game/characters/npcs/banquero.cpp

    # Criaturas (stubs por ahora)
    game/criaturas/creature.cpp
    game/criaturas/goblin.cpp
    game/criaturas/esqueleto.cpp
    game/criaturas/zombie.cpp
    game/criaturas/arana.cpp
    game/criaturas/orco.cpp
    game/criaturas/golem.cpp

    # Items (stubs por ahora)
    game/items/item.cpp
    game/items/arma.cpp
    game/items/armadura.cpp
    game/items/casco.cpp
    game/items/escudo.cpp
    game/items/pocion.cpp
    game/items/inventario.cpp

    # Banco
    game/banco/cuentaBanco.cpp

    # Mapa y juego
    game/map.cpp
    game/clan.cpp
    game/game.cpp
)
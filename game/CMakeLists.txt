target_sources(taller_game 
    PRIVATE
    
    # Razas

    # game/razas/humano.cpp
    # game/razas/elfo.cpp
    # game/razas/enano.cpp
    # game/razas/gnomo.cpp

    # Clases

    clases/charClase.cpp
    clases/mago.cpp
    clases/clerigo.cpp
    clases/paladin.cpp
    clases/guerrero.cpp

    # Characters

    characters/character.cpp
    characters/jugador.cpp

    # NPCs

    # game/characters/npcs/npc.cpp
    # game/characters/npcs/sacerdote.cpp
    # game/characters/npcs/comerciante.cpp
    # game/characters/npcs/banquero.cpp

    # Criaturas

    # game/criaturas/creature.cpp
    # game/criaturas/goblin.cpp
    # game/criaturas/esqueleto.cpp
    # game/criaturas/zombie.cpp
    # game/criaturas/arana.cpp
    # game/criaturas/orco.cpp
    # game/criaturas/golem.cpp

    # Items

    # game/items/item.cpp
    # game/items/arma.cpp
    # game/items/armadura.cpp
    # game/items/casco.cpp
    # game/items/escudo.cpp
    # game/items/pocion.cpp
    # game/items/inventario.cpp

    # Banco

    # game/banco/cuentaBanco.cpp

    # Mapa y juego

    map.cpp
    #clan.cpp
    game.cpp
    PUBLIC
    map.h
    game.h
)
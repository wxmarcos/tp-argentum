target_sources(taller_game PRIVATE
    # Config
    config.cpp

    # Formulas
    formulas.cpp

    # Razas
    razas/raza.cpp
    razas/humano.cpp
    razas/elfo.cpp
    razas/enano.cpp
    razas/gnomo.cpp

    # Clases
    clases/charClase.cpp
    clases/mago.cpp
    clases/clerigo.cpp
    clases/paladin.cpp
    clases/guerrero.cpp

    # Characters
    characters/character.cpp
    characters/jugador.cpp

    # NPCs (stubs por ahora)
    characters/npcs/npc.cpp
    characters/npcs/sacerdote.cpp
    characters/npcs/comerciante.cpp
    characters/npcs/banquero.cpp

    # Criaturas (stubs por ahora)
    criaturas/criatura.cpp
    criaturas/goblin.cpp
    criaturas/esqueleto.cpp
    criaturas/zombie.cpp
    criaturas/arana.cpp
    criaturas/orco.cpp
    criaturas/golem.cpp

    # Items
    items/item.cpp
    items/arma.cpp
    items/baculo.cpp
    items/armadura.cpp
    items/casco.cpp
    items/escudo.cpp
    items/pocion.cpp
    items/inventario.cpp
    items/itemFactory.cpp

    # Banco
    banco/cuentaBanco.cpp
    #Snapshot
    snapshot_factory.cpp
    # Mapa y juego
    mapa.cpp
    mundo.cpp
    clan.cpp
    game.cpp
)
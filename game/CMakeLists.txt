target_sources(taller_game PRIVATE
    # Config
    config.cpp

    # Formulas
    formulas.cpp

    # Razas
    razas/raza.cpp

    # Clases
    clases/charClase.cpp

    # Characters
    characters/character.cpp
    characters/jugador.cpp

    # Criaturas
    characters/criaturas/criatura.cpp
    characters/criaturas/goblin.cpp
    characters/criaturas/goblinJorobado.cpp
    characters/criaturas/esqueleto.cpp
    characters/criaturas/esqueletoHacha.cpp
    characters/criaturas/zombie.cpp
    characters/criaturas/arana.cpp
    characters/criaturas/aranaBlanca.cpp
    characters/criaturas/orco.cpp
    characters/criaturas/golem.cpp
    characters/criaturas/golemDemoniaco.cpp

    # Items
    items/item.cpp
    items/arma.cpp
    items/baculo.cpp
    items/armadura.cpp
    items/casco.cpp
    items/escudo.cpp
    items/pocion.cpp
    items/oro.cpp
    items/inventario.cpp
    items/itemFactory.cpp

    # Banco
    banco/cuentaBanco.cpp

    #Snapshot
    snapshot_factory.cpp

    # Game
    game/game.cpp
    game/gameComandos.cpp
    game/gameComandosClan.cpp
    game/gameComandosComercio.cpp
    game/gameComandosItems.cpp
    game/gameCombate.cpp
    game/gameMundo.cpp

    # Mapa y juego
    mapa.cpp
    mundo.cpp
    clan.cpp
    tmx_loader.cpp
)

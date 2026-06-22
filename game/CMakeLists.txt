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

    # NPCs
    characters/npcs/npc.cpp
    characters/npcs/sacerdote.cpp
    characters/npcs/comerciante.cpp
    characters/npcs/banquero.cpp

    # Criaturas
    characters/criaturas/criatura.cpp
    characters/criaturas/goblin.cpp
    characters/criaturas/goblin_jorobado.cpp
    characters/criaturas/esqueleto.cpp
    characters/criaturas/esqueleto_hacha.cpp
    characters/criaturas/zombie.cpp
    characters/criaturas/arana.cpp
    characters/criaturas/arana_blanca.cpp
    characters/criaturas/orco.cpp
    characters/criaturas/golem.cpp
    characters/criaturas/golem_demoniaco.cpp

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

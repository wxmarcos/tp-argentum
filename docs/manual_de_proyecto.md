# Manual de Proyecto

**Trabajo Práctico — Argentum Online**  
Taller de Programación | Facultad de Ingeniería — UBA | Junio 2026

**Integrantes:**
- Pedro Miguel
- Leandro Rodrigo Pesa
- Marcos Tomas Weng Xu

---

## 1. División del trabajo

| Integrante | Rol | Sección |
|---|---|---|
| Pedro Miguel | Lógica del servidor | `game/` |
| Leandro Rodrigo Pesa | Cliente | `client/` |
| Marcos Tomas Weng Xu | Servidor, protocolo de comunicación y persistencia | `server/`, `common/`, `server/persistence/` |

### Pedro Miguel — Lógica del servidor

Responsable de toda la capa de lógica del juego:

- **Clase Game**: encargada de procesar los comandos de los jugadores, actualizar el estado del juego y enviar la información a los clientes.
- **Jerarquía de personajes**: `Character`, `Jugador` y `Criatura`.
- **Razas y clases**.
- **Sistema de inventario**.
- **ItemFactory**: utilizada para generar los distintos ítems del juego.
- **Sistema de configuración mediante `config.toml`**: permite modificar los parámetros del juego sin recompilar.
- **Fórmulas** de combate, experiencia y obtención de oro.
- **Sistema de combate PvE y PvP**.
- **Sistema de drops** de criaturas.
- **IA de criaturas**: detección de jugadores, persecución y ataque.
- **Sistema de spawn** configurable por mapa con población máxima.
- **Sistema de clanes**: creación, ingreso, aceptación, rechazo, expulsión y salida.
- **Comercio con NPCs**: comprar, vender, depositar y retirar en banco.

### Leandro Rodrigo Pesa — Cliente

Responsable de todo el cliente gráfico del juego:

* **Clase `ClientApp`**: coordina el ciclo principal del cliente, el flujo de login y la sesión de juego.
* **Clase `ServerConnection`**: gestiona la conexión TCP con el servidor mediante dos threads dedicados (`ClientSender` y `ClientReceiver`) y colas thread-safe.
* **Clase `ClientGameState`**: mantiene el estado local del mundo aplicando los snapshots recibidos del servidor.
* **Sistema de renderizado (`WorldRenderer`, `HudRenderer`)**: dibuja el mapa TMX por capas, los personajes con animaciones, criaturas, ítems en el suelo, efectos visuales y textos flotantes de daño.
* **Sistema de sprites**: registro de texturas para cuerpos, cabezas, cascos, armas y criaturas con ajustes finos de posición por raza, clase y dirección.
* **Carga de mapas TMX**: parser propio del formato de Tiled con soporte de tilesets externos e inline, capas de colisión y flags de flip.
* **Sistema de animaciones**: animaciones de caminar basadas en tiempo real con transición automática al frame de reposo.
* **Sistema de audio**: música de fondo y efectos de sonido modulados por distancia usando SDL2_mixer.
* **Sistema de input**: manejo de teclado, mouse y consola de comandos con parser de comandos del juego (`/meditar`, `/comprar`, etc.).
* **Flujo de login y creación de personaje**: pantalla de menú, autenticación y creación de personaje nuevo.
* **Cheats de desarrollo**: combinaciones de teclas para facilitar las pruebas (god mode, morir, resucitar, mana infinito).
* **Configuración mediante `client.toml`**: permite modificar parámetros del cliente sin recompilar.

### Marcos Tomas Weng Xu — Servidor de red, protocolo de comunicación y persistencia

El desarrollo del servidor estuvo orientado a construir una arquitectura concurrente y autoritativa, desacoplando la recepción de comandos, la lógica del juego y la persistencia mediante colas thread-safe. Además, se implementó un protocolo binario propio para la comunicación cliente-servidor y un sistema de persistencia binaria asíncrona para evitar bloqueos durante la ejecución del juego.

#### Servidor

Responsable de la comunicación entre los clientes y la lógica del juego:

* **Clase `Server`**: inicializa el servidor y coordina el funcionamiento de todos sus componentes.
* **Clase `Acceptor`**: escucha nuevas conexiones TCP y crea un `ClientHandler` para cada cliente conectado.
* **Clase `ClientHandler`**: administra la comunicación de un cliente mediante un `Receiver` y un `Sender`.
* **Clase `Receiver`**: recibe comandos desde el socket, los decodifica y los encola para su procesamiento.
* **Clase `Sender`**: envía a cada cliente las actualizaciones generadas por el servidor.
* **Clase `GameLoop`**: ejecuta el ciclo principal del servidor a una frecuencia configurable, procesa los comandos recibidos, actualiza el estado del juego y distribuye los `Snapshot` correspondientes.
* **Clase `MonitorClients`**: administra los clientes conectados y permite realizar envíos **broadcast**, **unicast** y **multicast** según el destinatario de cada evento.
* **Arquitectura concurrente**: utiliza colas thread-safe para desacoplar la recepción de comandos, la lógica del juego y la persistencia, evitando bloqueos entre hilos.
* **Servidor autoritativo**: todas las acciones solicitadas por los clientes son validadas por el servidor antes de modificar el estado del mundo.


#### Protocolo de comunicación

Responsable del intercambio de información entre clientes y servidor:

* **Protocolo binario propio**: minimiza el tamaño de los mensajes transmitidos y reduce el ancho de banda utilizado.
* **Clase `Command`**: representa los comandos enviados por los clientes al servidor.
* **Clase `Snapshot`**: representa las actualizaciones del estado del juego enviadas por el servidor a los clientes.
* **`CommandCodec` y `SnapshotCodec`**: serializan y deserializan los mensajes utilizando un formato binario.
* **Sistema de opcodes**: identifica el tipo de comando o actualización transmitida.
* **Comunicación orientada a eventos**: el servidor únicamente envía información cuando cambia el estado del juego.
* **Tipos de envío**: soporta mensajes **broadcast**, **unicast** y **multicast**, optimizando el tráfico de red y evitando enviar información innecesaria a los clientes.
* **Sincronización del estado**: los `Snapshot` permiten mantener sincronizados los clientes mediante eventos de creación, movimiento, eliminación, combate, inventario, estadísticas y cambios de mapa.

#### Persistencia

Responsable del almacenamiento y recuperación del estado permanente del juego:

* **Clase `PersistenceWorker`**: ejecuta las operaciones de persistencia en un hilo independiente para no bloquear el `GameLoop`.
* **Clase `PersistenceLoader`**: restaura personajes persistidos durante el proceso de login.
* **Cola de persistencia**: almacena los trabajos generados por el servidor y sincroniza la comunicación con el `PersistenceWorker`.
* **Archivos binarios**: almacenan personajes, índice de acceso y clanes utilizando registros de tamaño fijo.
* **Índice de jugadores**: permite localizar un personaje por su nick sin recorrer el archivo completo.
* **Clase `PersistenceRecordMapper`**: convierte entre los objetos del juego y su representación binaria.
* **Persistencia incremental**: cada acción relevante del jugador genera un trabajo de persistencia, reduciendo la pérdida de información ante una desconexión inesperada.
* **Persistencia asíncrona**: todas las escrituras en disco se realizan fuera del hilo principal del servidor, manteniendo constante la frecuencia de actualización del `GameLoop`.

---

## 2. Organización semanal y plan inicial

El plan inicial contemplaba ir desarrollando el juego de forma incremental semana a semana, priorizando primero la comunicación cliente-servidor y luego ir sumando features de juego.

| Semana | Objetivo planificado | Lo que se hizo |
|---|---|---|
| 12/05 | Definición de roles, repo y diagrama de clases | Se definieron los roles, se armó el repo con ramas `main` y `dev` y se hizo el diagrama de clases inicial |
| 19/05 | Flujo de movimiento completo cliente-servidor, protocolo base, concurrencia base | Se planteó la arquitectura del cliente con los threads de red, el protocolo binario base y el movimiento validado por el servidor |
| 26/05 | Sprites cargados, animaciones, cámara, concurrencia semi terminada | Se implementó el sistema de sprites y texturas, la cámara centrada en el jugador y las animaciones de caminar |
| 02/06 | HUD, cámara terminada, gameloop , cheats, installer | Se terminó el HUD con inventario y barras de vida/maná, el mini-chat, los cheats de desarrollo y el gameloop |
| 09/06 | ENTREGA INTERMEDIA | Se entregó el cliente funcional con login, movimiento, renderizado y HUD |
| 16/06 | Mini-chat, persistencia, clanes | Se integró el mini-chat completo con scroll y comandos del juego, implementacion base de clanes y persistencia completa |
| 23/06 | ENTREGA FINAL — todas las features terminadas | Se completaron todas las features del cliente y se pulieron detalles visuales y de audio |

El plan inicial se respetó en líneas generales. La parte que más se extendió respecto a lo planificado fue el sistema de sprites y ajustes visuales de los personajes, que requirió más trabajo fino del esperado para que las cabezas, cascos y armas quedaran correctamente alineados sobre cada combinación de raza y clase.

---

## 3. Herramientas utilizadas

### IDE
- Pedro: Visual Studio Code con extensiones de C++ (clangd, CMake Tools).
- Leandro: Visual Studio Code con extensiones de C++ (clangd, CMake Tools).
<!-- TODO: Marcos: IDE utilizado -->

### Linter y análisis estático
- `cpplint` para verificar estilo de código C++ (Google style guide).
<!-- TODO: Otras herramientas utilizadas -->

### Control de versiones
- Git con GitHub para control de versiones y revisión de código.

### Build system
- CMake para la configuración y compilación del proyecto.

---

## 4. Recursos y documentación de aprendizaje
- Enunciado y material de la materia Taller de Programación (FIUBA): principal referencia para el diseño de la arquitectura, los requisitos funcionales y las reglas de negocio del juego.
- **cppreference.com**: referencia principal de la biblioteca estándar de C++20.
- **toml++** (https://marzer.github.io/tomlplusplus/): documentación oficial de la librería de configuración TOML.
- **SDL2 Wiki** (https://wiki.libsdl.org/): referencia para el cliente gráfico (renderizado, manejo de eventos y audio).
- **SDL2_mixer docs**: para el sistema de audio y mezcla de efectos.
- **SDL2_ttf docs**: para el renderizado de texto con fuentes TTF.
- **Tiled Map Editor docs** (https://doc.mapeditor.org/): para entender el formato TMX y los tilesets.
<!-- TODO: Otros recursos utilizados por Marcos -->

---

## 5. Puntos más problemáticos

### Alineación de sprites por raza, clase y dirección

El mayor desafío visual fue lograr que las cabezas, cascos y armas quedaran correctamente posicionados sobre el cuerpo de cada personaje. Cada combinación de raza y clase tiene proporciones ligeramente distintas en su spritesheet, y además la posición varía según la dirección en la que mira el personaje. Esto derivó en una tabla de ajustes finos (offsets en píxeles) para cada combinación posible, que requirió mucha prueba y error hasta lograr un resultado visualmente correcto.

### Parser de mapas TMX

El formato TMX de Tiled tiene variantes (tilesets externos en archivos `.tsx`, tilesets inline, capas de tiles, capas de colisión como objetos rectangulares, flags de flip en los GIDs) que requirieron un parser propio capaz de manejar todos esos casos. Además, el sistema de GIDs globales y la resolución de qué textura y qué rect corresponde a cada tile sumaron complejidad.

---

## 6. Errores conocidos

- No se encontraron bugs a la hora de probar el juego. Tal vez existe algun bug por fuera de lo probado en el mismo.

---

## 7. ¿Qué cambiarían?

### A nivel código

- **Representar las entidades mediante identificadores únicos estables**. En varias partes del sistema se usa el nick como identificador de jugadores y también strings para criaturas o NPCs. Un diseño más robusto habría usado un `EntityId` único para toda entidad del mundo, dejando el nick solamente como atributo visible del jugador.
- **Los ajustes de sprites hardcodeados**. La tabla de offsets de cabezas y cascos por raza, clase y dirección está hardcodeada en el código. Sería mejor tenerla en un archivo de configuración para poder ajustarla sin recompilar.

### A nivel organizacional

- Hacer code reviews más frecuentes entre los integrantes.
- Mejorar la comunicación entre los integrantes. 

---

## 8. ¿Qué debería darse en Taller?

<!-- TODO: Temas o herramientas que creen que deberían enseñarse en la materia y no se dieron -->

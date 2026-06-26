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

<!-- TODO: Describir trabajo realizado en el cliente (SDL2, renderizado, protocolo cliente, etc.) -->

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
<!-- TODO: Describir trabajo realizado en el servidor (aceptación de conexiones, threading, protocolo, etc.) -->

---

## 2. Organización semanal y plan inicial

<!-- TODO: Describir el plan de desarrollo semana a semana y en qué difirió del plan inicial -->

---

## 3. Herramientas utilizadas

### IDE
- Pedro: Visual Studio Code con extensiones de C++ (clangd, CMake Tools).
- <!-- TODO: Leandro: IDE utilizado -->
- <!-- TODO: Marcos: IDE utilizado -->

### Linter y análisis estático
- `cpplint` para verificar estilo de código C++ (Google style guide).
- <!-- TODO: Otras herramientas utilizadas -->

### Control de versiones
- Git con GitHub para control de versiones y revisión de código.

### Build system
- CMake para la configuración y compilación del proyecto.

---

## 4. Recursos y documentación de aprendizaje
- Enunciado y material de la materia Taller de Programación (FIUBA): principal referencia para el diseño de la arquitectura, los requisitos funcionales y las reglas de negocio del juego.
- **cppreference.com**: referencia principal de la biblioteca estándar de C++20.
- **toml++** (https://marzer.github.io/tomlplusplus/): documentación oficial de la librería de configuración TOML.
- **SDL2 Wiki** (https://wiki.libsdl.org/): referencia para el cliente gráfico.
- <!-- TODO: Otros recursos utilizados por Leandro -->
- <!-- TODO: Otros recursos utilizados por Marcos -->

---

## 5. Puntos más problemáticos

<!-- TODO: Describir los desafíos técnicos más importantes que encontraron durante el desarrollo -->

---

## 6. Errores conocidos

<!-- TODO: Listar bugs conocidos que quedaron sin resolver al momento de la entrega -->

---

## 7. ¿Qué cambiarían?

### A nivel código

- **Representar las entidades mediante identificadores únicos estables**. En varias partes del sistema se usa el nick como identificador de jugadores y también strings para criaturas o NPCs. Un diseño más robusto habría usado un `EntityId` único para toda entidad del mundo, dejando el nick solamente como atributo visible del jugador.
<!-- TODO: Qué refactors o decisiones de diseño cambiarían si volvieran a hacer el proyecto -->

### A nivel organizacional

<!-- TODO: Qué cambiarían en la dinámica del equipo, división de tareas, comunicación, etc. -->

---

## 8. ¿Qué debería darse en Taller?

<!-- TODO: Temas o herramientas que creen que deberían enseñarse en la materia y no se dieron -->

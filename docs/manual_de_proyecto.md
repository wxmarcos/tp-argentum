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
| Marcos Tomas Weng Xu | Servidor de red | `server/` |

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

### Marcos Tomas Weng Xu — Servidor de red

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

<!-- TODO: Qué refactors o decisiones de diseño cambiarían si volvieran a hacer el proyecto -->

### A nivel organizacional

<!-- TODO: Qué cambiarían en la dinámica del equipo, división de tareas, comunicación, etc. -->

---

## 8. ¿Qué debería darse en Taller?

<!-- TODO: Temas o herramientas que creen que deberían enseñarse en la materia y no se dieron -->

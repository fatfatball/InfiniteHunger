# Infinite Hunger

A 2D Gauntlet-like game written in C++20 for the **Programming and Fundamental Algorithms** coursework.

The project is built directly on the provided `GamesEngineeringBase` framework. It does not use an external game engine or a third-party gameplay framework. The game uses a software rasterizer, a custom object/update model, custom dynamic arrays, and a deterministic room-generation scheme.

---

## 1. Overview

`Infinite Hunger` is a top-down, room-based action game with:

- Three playable classes: **Swordsman**, **Archer**, and **Wizard**
- Three enemy types plus a static monster spawner
- Local two-player play
- Melee, projectile, and area attacks
- Character skills paid for with health
- Keys, locked doors, hazards, and pressure-plate traps
- Procedurally streamed rooms
- A tutorial room
- Menu, player selection, tally, and save/continue flow
- A small bitmap text renderer used for the menus, tally, and FPS display

The player's body is a chain of nodes. The chain is both the character's physical body and its health bar: shorter chain means lower health.

---

## 2. Build and Run

### Requirements

- Visual Studio with the C++ desktop workload
- The Visual Studio solution: `Assignment.slnx`
- The provided `GamesEngineeringBase` files
- Windows (the framework uses Direct3D 11 and the Win32 window API)

### Build

1. Open `Assignment.slnx`.
2. Select `Debug | x64` or `Release | x64`.
3. Build the `Assignment` project.
4. Run from Visual Studio.

The project uses the **v145** toolset and **C++20**.

### Runtime paths

The following are resolved relative to the working directory:

```text
Resources/
save.sav
```

Keep the Visual Studio debugging working directory set to:

```text
$(ProjectDir)
```

Otherwise sprites, maps, and the save file will not be found.

---

## 3. Controls

### Player 1

| Input | Action |
|---|---|
| `W`, `A`, `S`, `D` | Move / turn |
| `C` | Primary attack |
| `V` | Skill |

### Player 2

| Input | Action |
|---|---|
| Arrow keys | Move / turn |
| `,` | Primary attack |
| `.` | Skill |

### Menus

- Mouse hover and left click
- Menu buttons are `ClickableText` objects with callbacks

### Quit / save

- `Esc` requests a quit
- If the current scene is gameplay, the run is saved to `save.sav` before closing

---

## 4. High-Level Architecture

```text
main.cpp
  └── SceneManager
        ├── Window
        ├── Camera
        ├── SaveManager
        ├── currentScene / nextScene
        │
        ├── SceneMenu
        ├── SceneTutorial
        ├── SceneGameplay
        └── SceneTally

Scene
  ├── objects          : List<shared_ptr<BaseGameObject>>
  ├── uiObjects        : List<shared_ptr<ClickableText>>
  ├── players          : List<shared_ptr<BasePlayer>>
  ├── roomManager      : shared_ptr<RoomManager>
  └── camera reference
```

`SceneManager` owns the window and camera and controls scene transitions. A scene owns its world objects and UI objects. Rooms are streamed by `RoomManager`.

### Scene responsibilities

| Class | Responsibility |
|---|---|
| `SceneManager` | Main loop, current/next scene, camera, save manager, quit flow |
| `Scene` | Object lifecycle, fixed/visual updates, collisions, rendering |
| `SceneMenu` | Title screen and player selection |
| `SceneTutorial` | Fixed tutorial room |
| `SceneGameplay` | Normal single/multiplayer gameplay and run statistics |
| `SceneTally` | End-of-run statistics |
| `RoomManager` | Room creation, streaming, camera boundary |
| `SaveManager` | Persistent room slots and save-file I/O |

Scene transitions are deferred:

```text
ChangeScene()
  -> builds nextScene
  -> actual swap at the end of the rendered frame
```

This avoids destroying a scene while its own UI callback or update is still executing.

---

## 5. Object Model

```text
BaseGameObject
├── BasePhysicalObject
│   ├── BaseCharacter
│   │   ├── BasePlayer
│   │   │   ├── Player0   (Swordsman)
│   │   │   ├── Player1   (Archer)
│   │   │   └── Player2   (Wizard)
│   │   ├── BaseEnemy
│   │   │   ├── EnemyFast
│   │   │   ├── EnemyMedium
│   │   │   ├── EnemySlow
│   │   │   └── EnemyStatic
│   │   └── BaseBodyNode
│   │       ├── PlayerTailNode
│   │       └── EnemyBodyNode
│   ├── BaseAttack
│   │   ├── PlayerAttack0
│   │   ├── PlayerAttack1
│   │   └── PlayerAttack2
│   ├── Projectile
│   └── BaseItem
│       ├── ItemKey
│       ├── ItemLockedDoor
│       ├── ItemTrap
│       └── ItemTrapTrigger
├── Tilemap
│   └── Room
│       ├── Room0
│       ├── Room1
│       ├── Room2
│       └── RoomTutorial
└── ObjectTimer
```

`ClickableText` is intentionally **not** a `BaseGameObject`. It is a screen-space UI widget with an AABB and a callback, not a world entity.

`BaseSkill` is also not a `BaseGameObject`; it is a small strategy object owned by a player.

### BaseGameObject

`BaseGameObject` provides:

- `prevState`, `curState`, and `renderState`
- position, scale, rotation
- `isActive` / `isPendingDestroy`
- `canCollide` and `isTrigger`
- rotated AABB calculation
- interpolated rendering state
- depth-test and texture-sampling interfaces
- `OnStart`, `FixedUpdate`, `VisualUpdate`, `OnDestroy`

A rotated object's AABB is computed from its rotated half-extents:

```cpp
resX = |cos(theta) * hx| + |sin(theta) * hy|
resY = |sin(theta) * hx| + |cos(theta) * hy|
```

### BasePhysicalObject

Adds:

- Verlet-style position integration
- `velocity`, `force`, `mass`
- damping and maximum velocity
- `collisionRadius`
- `isFixed` (constraint participation)
- `isStatic` (collision response participation)

### BaseCharacter

Adds:

- health and invulnerability
- a chain of body nodes
- distance, angle, and direction-angle constraints
- body-node rotation definitions
- PolyLine rendering data

### Ownership

Scene objects are owned by `shared_ptr` in `Scene::objects`. Rooms use `weak_ptr` to remember the static objects they spawned, so the scene remains the actual owner and a room cannot keep a picked-up key alive forever.

---

## 6. Main Loop

The game uses a fixed simulation step:

```text
FIXED_DELTATIME = 1 / 50 second
```

Pseudocode:

```text
while (running)
{
    frameTime = timer.dt()
    frameTime = min(frameTime, 0.1)

    accumulator += frameTime

    while (accumulator >= FIXED_DELTATIME)
    {
        sceneManager.FixedUpdate(FIXED_DELTATIME)
        accumulator -= FIXED_DELTATIME
    }

    sceneManager.VisualUpdate(frameTime)
    sceneManager.Render(accumulator / FIXED_DELTATIME)
}
```

`SceneManager::FixedUpdate` calls:

```text
Scene::FixedUpdateObjects(dt)
  -> all object FixedUpdate()
  -> entity/entity collisions
  -> entity/tilemap collisions
  -> SolveConstraints()
  -> UpdateVelocity()
  -> UpdateStates()
Scene::RecycleObjects()
```

The order matters:

- Objects are updated before collisions.
- Collisions and constraints run before states are committed.
- Objects are marked with `isPendingDestroy` during simulation and only removed after the fixed step.
- Visual interpolation and rendering only see surviving objects.

`Scene::VisualUpdateObjects` updates body chains, rotations, and UI. The camera then follows its target with smoothing.

Rendering is performed after visual updates. Scene transitions are applied after rendering.

---

## 7. Rendering

The renderer is a CPU software rasterizer that writes into the back buffer provided by `GamesEngineeringBase`.

### Screen tiling

The screen is divided into `64 x 64` render tiles. Each tile is processed independently.

`CO_RENDER_THREADS` worker threads plus the main thread consume tiles from an atomic counter:

```text
nextTile.fetch_add(1)
```

Each worker renders a different tile, so workers do not write to the same screen pixels.

### Two-pass rendering

For each tile:

1. **Depth prepass**
   - Iterate objects in render order.
   - Run `DepthTest` for each object.
   - Build a per-tile list of objects with assigned depth values.
   - Opaque pixels update the shared depth graph.

2. **Color pass**
   - Render the collected objects in reverse depth order.
   - `RenderSelf` writes only pixels that are not blocked by the depth graph.

`RenderOrder` is a coarse layer value:

```text
First  (floor / tilemap)
Second (body nodes / PolyLine)
Third  (characters / attacks)
```

### Texture sampling and rotation

For each screen pixel, the renderer:

1. Converts the screen position to world space through the camera.
2. Translates relative to the object.
3. Rotates by the negative object rotation.
4. Checks the rotated local extent.
5. Converts to texture `(u, v)` coordinates.
6. Samples `GetColorAt` and `GetAlphaAt`.

This allows rotated sprites without a separate rotated sprite asset.

### Interpolation

Simulation runs at 50 Hz, while rendering can run at a different frame rate. `UpdateRenderState` interpolates:

- position
- scale
- rotation (using angle-aware interpolation)

This avoids visible stepping.

### PolyLine

Body chains are rendered as thick polylines:

- Liang-Barsky clipping against a tile
- Bresenham line stepping
- Midpoint circle stamping
- Cross stamping for thickness

The combination of an unfilled circle and a cross was chosen as a compromise between visual quality and CPU cost. A filled circle at every Bresenham point produces too many redundant writes.

### Text

UI text uses a 5x7 bitmap font:

```text
FONT5x7[96][5]
```

Each character is five columns of seven bits. `TextRenderer` draws text directly to the back buffer after the world render, so text is screen-space and unaffected by the camera or depth buffer.

The text renderer supports:

- scale
- centered text
- simple newlines
- AABB calculation for clickable text

---

## 8. Physics and Constraints

### Verlet-style integration

`BasePhysicalObject::UpdatePhysicPosition` uses the previous physics state as the velocity source:

```cpp
velocity = curPhysicState.velocityS + force / mass * dt;
velocity = clamp(velocity, maxVelocity);
velocity *= pow(dampValue, dt);
globalPosition = curState.globalPositionS + velocity * dt;
```

This gives simple, stable motion with damping and no explicit acceleration integration state.

### Position Based Dynamics

`BaseCharacter` uses Position Based Dynamics for body chains.

Supported constraints:

- **Distance constraint**  
  Keeps adjacent body nodes at a target distance.
- **Angle constraint**  
  Keeps the angle between two consecutive edges within a maximum bend.
- **Direction-angle constraint**  
  Keeps the neck aligned with the character's facing direction.

The solver is Gauss-Seidel style: constraints are solved one after another, in order, for a single iteration per fixed step. More iterations would make the chain stiffer but cost more.

Distance constraints distribute correction using inverse mass:

```text
w = 1 / mass
```

Fixed bodies receive zero weight. This keeps the head fixed while the tail follows.

The player's body-node mass decreases toward the tail, producing a heavier torso and a lighter tail.

### Collision model

Entity collision is circle-based:

```text
distance(a, b) < radius(a) + radius(b)
```

The collision response separates objects using inverse mass. Static objects do not move. Triggers receive collision callbacks but do not perform separation.

Tilemap collision is AABB-based. `Tilemap::ResolveCollision` checks the object's AABB against solid tiles and resolves the overlap.

Collision flags:

| Flag | Meaning |
|---|---|
| `canCollide` | Object participates in collision queries |
| `isTrigger` | Callback only; no positional separation |
| `isStatic` | Not moved by collision response |
| `isFixed` | Not moved by constraints |

This distinction is important for keys, doors, traps, projectiles, and swords.

---

## 9. Procedural Rooms

### Room geometry

Each room is a `19 x 19` tilemap.

```text
TILE_LENGTH = 64
ROOM_WIDTH  = 19
ROOM_HEIGHT = 19
```

A room is centered at:

```text
(coord.x * ROOM_WIDTH  * TILE_LENGTH,
 coord.y * ROOM_HEIGHT * TILE_LENGTH)
```

Each room loads:

- tile definitions from `Resources/Tiles`
- a map file from `Resources/Maps/<room>`

### Room streaming

`RoomManager` maintains the current room and its four direct neighbors. When the camera target enters a new room:

1. Add the four neighboring rooms.
2. Remove rooms that are more than one room away from the previous room.
3. Update the camera boundary to the current room.

The tutorial sets:

```cpp
roomManager->isInfinite = false;
```

so it uses one fixed `RoomTutorial` instead of streaming.

### Deterministic room type

Room types are selected from a run seed and the room coordinate.

The coordinate is converted to a non-negative pair, then passed through a Cantor pairing function:

```text
x' = x + 65535
y' = y + 65535
pair = (x' + y') * (x' + y' + 1) / 2 + y'
offset = pair * 325
```

A `std::mt19937` is seeded with:

```text
runSeed + offset
```

and the first output selects:

```text
roomNumber = rng() % 3
```

The result selects `Room0`, `Room1`, or `Room2`.

This means:

- The same run seed and coordinate produce the same room type.
- The result does not depend on room loading order.
- Different rooms have different seeds.

### Room ownership

`Room` stores weak references to the static objects it spawned:

```cpp
List<std::weak_ptr<BaseGameObject>> roomObjects;
```

When a room is removed:

```text
RoomManager::RemoveRoom
  -> Room::DestroyRoomObjects
  -> mark spawned items/traps as pending destroy
```

Enemies are deliberately not owned by the room. They can be tracked by their own save callbacks and despawn rules.

### Room types

| Room | Purpose |
|---|---|
| `Room0` | Combat room with hazards and keys |
| `Room1` | Locked-door room with enemies |
| `Room2` | Mixed enemies, spawner, and key |
| `RoomTutorial` | Fixed tutorial with keys, doors, and one of each enemy type |

---

## 10. Characters, Combat, and Items

### Player health

A player's health is represented by the number of tail nodes.

- Starting health: `3`
- Maximum health: `13`
- Taking damage removes tail nodes.
- Healing adds tail nodes.
- The player dies when the tail count reaches zero.

The player also suffers periodic self-damage. The interval between self-damage ticks decreases over time, creating pressure to finish the run.

### Attacks

Primary attacks are cooldown-based. The default cooldown is `0.75s`; the Archer uses a shorter `0.4s` attack cooldown.

| Attack | Behaviour |
|---|---|
| `PlayerAttack0` | Physical sword object sweeping in front of the player |
| `PlayerAttack1` | Physical arrow with constant speed and finite lifetime |
| `PlayerAttack2` | Static area attack in front of the player |

Attacks are physical trigger objects. They use the existing collision pipeline and damage enemies through `TakeDamage`.

### Skills

Skills cost one point of health. They are limited by health rather than a separate cooldown.

| Skill | Effect |
|---|---|
| Swordsman | Invulnerable for `7.5s` |
| Archer | Fires 16 arrows in all directions; `+25%` speed for `10s` |
| Wizard | Large area explosion around the player |

### Enemies

| Enemy | Role |
|---|---|
| `EnemyFast` | Fast melee chaser |
| `EnemyMedium` | Heavier melee chaser with healing body nodes |
| `EnemySlow` | Slow chaser with a ranged projectile attack |
| `EnemyStatic` | Static spawner that creates `EnemyFast` minions |

Enemies awaken when they enter the camera view and chase the closest living player.

Enemy body chains use the same PBD model as the player. Their body nodes are hit proxies that forward damage to the owner.

When an enemy dies, its head is removed and its body nodes remain briefly. `BodyNodeType::Heal` nodes can restore player health after the owner is dead.

`EnemyStatic` limits itself to a small number of live minions and respawns replacements over time.

### Items and hazards

| Object | Behaviour |
|---|---|
| `ItemKey` | Trigger pickup; increments player key count |
| `ItemLockedDoor` | Blocks the player; consumes one key when opened |
| `ItemTrap` | Contact hazard that damages players and enemies |
| `ItemTrapTrigger` | Pressure plate that triggers a trap projectile |

Room static objects are cleaned up when their room unloads.

### Projectiles

`Projectile` is a lightweight physical trigger object used by enemy ranged attacks and traps. It has:

- direction
- speed
- damage
- lifetime
- owner faction
- collision handling

---

## 11. UI and Scene Flow

### Scene flow

```text
Main Menu
  ├── Tutorial
  ├── Single Player -> Player Selection -> Gameplay
  ├── Multiplayer   -> Player Selection -> Gameplay
  ├── Continue      -> SaveManager::LoadFile -> Gameplay
  └── Quit

Gameplay
  -> player death -> Tally
  -> Esc          -> save and quit

Tally
  -> Main Menu
```

### Clickable text

`ClickableText` is a screen-space UI object with:

- text
- position
- scale
- screen AABB
- mouse-hover colour
- edge-triggered click callback

It uses the mouse edge state (`previous mouse down` vs current mouse down) to avoid firing every frame while the button is held.

Menus and player selection are built from these widgets.

### Text rendering

The menus, tally, and FPS display use `TextRenderer`:

- 5x7 fixed bitmap glyphs
- integer scaling
- direct back-buffer writes
- no camera transform
- no depth interaction

The FPS counter is drawn in the top-left corner by `SceneManager`.

---

## 12. Save and Continue

The save system is intentionally small and text-based.

### Runtime state

`SaveManager` stores a list of `RoomSave` records:

```cpp
struct RoomSave
{
    RoomCoord coord;
    bool completed[4];
};
```

Each room has four persistent progress slots, used for things such as:

```text
key picked up
door unlocked
enemy defeated
trap/hazard handled
```

### Save data

`SaveContext` stores:

- random room seed
- starting room coordinate
- `RunStat` (player count, kills, time)
- up to two player snapshots
- `PlayerSaveData`: alive flag, class, position, health, self-damage timer, self-damage countdown, key count

The save file is:

```text
save.sav
```

It is written as plain text. `SaveManager::SaveFile` writes the `SaveContext`, followed by the room records.

### Save triggers

A gameplay quit (`Esc`) requests a deferred quit. At the end of the visual update:

1. Recycle the current objects so natural enemy despawns can update their room flags.
2. Build a `SaveContext` from the live scene.
3. Write `save.sav`.
4. Close the application.

### Continue

The menu's `CONTINUE` option calls `SceneManager::ContinueGame`.

```text
LoadFile
  -> SaveContext
  -> room state list

ContinueGame
  -> create SceneGameplay with saved seed and start room
  -> recreate player classes
  -> restore position, health, self-damage timers, and keys
  -> restore camera target
```

Rooms are regenerated from the saved seed and then filtered by their saved completion flags.

The save system is a single-slot runtime save. It is not intended as a versioned, migration-safe save format.

---

## 13. Data Structures and Algorithms

### Custom List

The project uses a custom dynamic array:

```cpp
template<typename T>
class List;
```

Properties:

- contiguous storage
- capacity growth by doubling
- `PushBack`: amortized `O(1)`
- indexing: `O(1)`
- `Erase` / `Insert` in the middle: `O(n)`
- `Reserve` / `Resize`: `O(n)`

The game intentionally uses `List` instead of `std::vector`, `std::list`, or `std::map` for gameplay containers.

The only standard containers encountered are inside the provided `GamesEngineeringBase` framework, not in the game's own data model.

### AABB

`AABB` supports:

- containment
- intersection
- intersection AABB
- union
- screen-space clipping

The rotated object AABB is computed from the object's half-extents and rotation, not from a world-space sprite rectangle.

### Collision complexity

Let:

- `N` = number of scene objects
- `R` = number of loaded rooms
- `C` = number of constraints
- `T` = number of render tiles

The current costs are:

| Step | Complexity |
|---|---|
| Object fixed updates | `O(N)` |
| Entity/entity collision | `O(N^2)` |
| Entity/tilemap collision | `O(R * N)` |
| Constraint solving | `O(C)` per iteration |
| Rendering | roughly `O(T * N + visible pixels)` |
| Room type generation | `O(1)` |

`N` is kept small by room streaming and object lifetimes, so the simple `O(N^2)` collision loop is acceptable for this project.

### Rendering algorithms

- Screen-space tiling: `64 x 64`
- Parallel tile consumption with `std::atomic<int>`
- Depth prepass and colour pass
- Rotated UV sampling
- Bresenham line stepping
- Midpoint circle generation
- Liang-Barsky line clipping
- Alpha blending in `Camera::DrawOnCanvas`

### Room generation algorithms

- Cantor pairing for 2D coordinate to 1D offset
- `std::mt19937` for deterministic room-type selection
- Local room streaming with a five-room working set
- AABB room/camera intersection

### Text algorithm

- 5x7 bitmap font
- bit test per glyph column
- rectangle fill per set pixel
- cursor advance of `6 * scale`
- line advance of `8 * scale`

---

## 14. Design Decisions and Trade-offs

### Reuse the physical object system for attacks

Swords, arrows, and explosions are physical trigger objects. This reuses:

- collision detection
- lifetime handling
- scene ownership
- render-state interpolation

instead of introducing a second attack-only collision system.

### Separate triggers from static geometry

- Keys and pressure plates use `isTrigger`.
- Doors and traps use `isStatic`.
- Attacks use both trigger and physical collision.

This keeps collision response and gameplay callbacks separate.

### Deferred destruction

Objects are never erased in the middle of an update or a UI callback.

```text
mark isPendingDestroy
  -> finish the fixed step
  -> RecycleObjects
```

This avoids iterator invalidation and use-after-free bugs.

### Deferred scene changes

`SceneManager` builds `nextScene` and swaps at the end of rendering.

This allows a button callback to request a scene change safely.

### Weak room ownership

Rooms remember spawned objects with `weak_ptr`. The scene remains the owner. This avoids a room keeping a collected key alive after the key has been removed from the scene.

### Text as screen-space UI

Text is rendered after the world and bypasses the camera and depth buffer. This is considerably simpler than forcing text through the world-object render pipeline.

---

## 15. Known Limitations

- Entity collision uses a simple `O(N^2)` pairwise loop; there is no spatial hash or broadphase.
- Tilemap collision checks every loaded room against every physical object.
- Rendering is CPU-based. Threads reduce wall-clock time, but the renderer still writes every visible pixel in software.
- Text uses a fixed 5x7 bitmap font; there is no font atlas or anti-aliasing.
- AI uses steering/chase behaviour only. There is no pathfinding or navigation graph.
- Room generation chooses a room type deterministically, but does not build a global dungeon connectivity graph.
- The save file is a single-slot, plain-text runtime save with no schema version or migration support.
- Audio is not part of the current build.
- The tutorial is a fixed room rather than a scripted sequence of objectives.

---

## 16. Project Layout

```text
Assignment/
├── Assignment.slnx
├── Assignment.vcxproj
├── Assignment.vcxproj.filters
├── README.md
├── Resources/
│   ├── Characters/
│   ├── Items/
│   ├── Maps/
│   └── Tiles/
├── save.sav
└── src/
    ├── main.cpp
    ├── Constants.h
    ├── MyMath.h
    ├── core/
    │   ├── GamesEngineeringBase.h
    │   ├── BaseGameObject.h
    │   ├── BasePhysicalObject.h
    │   ├── BaseCharacter.h/.cpp
    │   ├── BaseBodyNode.h
    │   ├── BasePlayer.h/.cpp
    │   ├── BaseEnemy.h
    │   ├── BaseAttack.h/.cpp
    │   ├── BaseSkill.h
    │   ├── BaseItem.h/.cpp
    │   └── ObjectTimer.h
    ├── entity/
    │   ├── Player0/1/2
    │   ├── PlayerAttack0/1/2
    │   ├── PlayerSkill0/1/2
    │   ├── PlayerTailNode
    │   ├── EnemyFast/Medium/Slow/Static
    │   ├── EnemyBodyNode
    │   ├── Projectile
    │   └── PolyLine2D
    ├── world/
    │   ├── Tilemap
    │   ├── Room.h
    │   ├── Room0/1/2
    │   ├── RoomTutorial
    │   ├── RoomManager
    │   ├── Camera
    │   ├── SaveManager
    │   ├── scene/
    │   │   ├── Scene
    │   │   ├── SceneManager
    │   │   ├── SceneMenu
    │   │   ├── SceneTutorial
    │   │   ├── SceneGameplay
    │   │   └── SceneTally
    │   └── tile_item/
    │       ├── ItemKey
    │       ├── ItemLockedDoor
    │       ├── ItemTrap
    │       └── ItemTrapTrigger
    ├── data_structs/
    │   ├── List.h
    │   ├── Vector2.h
    │   ├── AABB.h
    │   └── SmallStructs.h
    ├── ui/
    │   ├── ClickableText
    │   └── Fonts.h
    └── test/
        ├── TextRenderer
        └── TestGameObject / TestPhysicalObject / TestPhysicalTail
```
## 17. Assets and Acknowledgements

Character, item, and tile sprites are based on **Kenney** assets.

The window, image loading, input, timing, and rendering back buffer are provided by the module's `GamesEngineeringBase` framework.

Special thanks to Kenney for the assets.

# ECS

A simple, header-only Entity Component System for C++, built from scratch. Designed around a dense packed component storage model with bitmask-based entity signatures for fast archetype queries.

## Architecture

The ECS is split into three managers coordinated by a single `Registry` facade, all implemented as singletons.

```
Registry
├── EntityManager    — entity lifecycle + bitmask ID tracking
├── ComponentManager — typed dense arrays per component
└── SystemManager    — views and entity membership updates
```

### Entity

An entity is a plain integer ID. Each entity has an associated `ID` — a 64-bit bitmask where each bit represents whether a component type is attached. Up to **64 component types** are supported.

### ComponentArray\<T\>

Each component type gets its own `ComponentArray<T>`, a densely packed `std::vector<T>` with bidirectional `entity ↔ index` maps. Removal uses the **swap-with-last** technique for O(1) deletion without holes.

```
Add    → push to back, update both maps
Remove → swap with last element, pop back, fix maps
Get    → direct index lookup
```

### EntityManager

Manages entity creation and recycling. Destroyed entities go into a recycle pool and are reused before allocating new IDs.

### SystemManager

Maintains registered views. When any entity's component bitmask changes (`EntityIDChanged`), all views are updated — entities are inserted or removed from each view's `std::set<Entity>` based on bitmask matching.

### Registry

The public API. Combines all three managers behind a clean interface.

---

## Usage

### Setup

```cpp
Registry *registry = Registry::SharedInstance();

// Register every component type before use (max 64)
registry->Register<Position>();
registry->Register<Velocity>();
```

### Creating Entities

```cpp
// Empty entity
Entity e0 = registry->CreateEntity();

// Entity with components pre-assigned (bitmask set at creation)
Entity e1 = registry->CreateEntity<Position, Velocity>();
Entity e2 = registry->CreateEntity<Velocity>();
```

### Adding and Reading Components

```cpp
registry->AddComponent(e0, Velocity{100, 0});
registry->AddComponent(e0, Position{});

int vx = registry->GetComponent<Velocity>(e0).x;
```

### Querying Entities (Views)

```cpp
// Returns all entities that have both Position and Velocity
auto view = registry->View<Position, Velocity>();

for (const auto& entity : view->entities) {
    auto& vel = registry->GetComponent<Velocity>(entity);
    auto& pos = registry->GetComponent<Position>(entity);
    // ...
}
```

### Filtering

```cpp
// Exclude entities that also have a specific component
view->Filter<Collider>();
```

### Removing Components and Entities

```cpp
registry->RemoveComponent<Velocity>(entity);
registry->DestroyEntity(entity);  // entity ID is recycled for reuse
```

---

## Known Limitations

- **Max 64 component types** — the entity signature is a 64-bit integer bitmask. Registering more than 64 types triggers an assert.
- **`Except<T>()`** — stubbed out, not yet implemented.
- **Variadic `RemoveComponent`** — commented out; only single-type removal currently works.
- **View entity set is `std::set`** — not cache-friendly for large worlds; iteration is O(n log n).
- **Entity recycling pool is `std::vector`** — noted in source as a candidate for replacement with `std::deque` for correct FIFO semantics.
- **`RemoveComponent` bitmask bug** — uses `!` (logical NOT) instead of `~` (bitwise NOT), so component bits are not correctly cleared on removal.

---

## Roadmap

- [ ] Fix `RemoveComponent` bitmask clear (`~` instead of `!`)
- [ ] Implement `Except<T>()` filter on views
- [ ] Replace entity recycle pool `std::vector` with `std::deque`
- [ ] Variadic `RemoveComponent<T...>(entity)`
- [ ] Replace `std::set<Entity>` in views with a flat packed array for faster iteration
- [ ] Support beyond 64 components via `std::bitset` or a sparse bitmask
- [ ] Archetype-based storage for improved cache locality across component types
- [ ] System update loop integration (tick / update interface per system)

---

## Building

```bash
git clone https://github.com/laperex/ECS.git
cd ECS
mkdir build && cd build
cmake ..
make
```

Requires a C++ compiler with C++17 support or later.

---

## License

See [LICENSE](LICENSE) for details.

# Cache_Simulator
A c++ based cache simulator

A configurable, trace-driven cache simulator built from scratch in C++. Simulates direct-mapped, set-associative, and fully-associative caches with configurable replacement policies, write strategies, and multi-level hierarchy support. Designed as both a learning tool and a performance analysis engine for exploring how cache design decisions affect real-world hit rates.

## Features

### Mapping techniques
- **Direct-mapped** — each memory block maps to exactly one cache line
- **N-way set-associative** — configurable associativity (2-way, 4-way, 8-way, etc.)
- **Fully-associative** — any block can occupy any cache line

All three are implemented using a unified set-based architecture. Direct-mapped is treated as 1-way set-associative, and fully-associative as a single set containing all lines.

### Replacement policies
- **LRU** — evicts the least recently used line
- **FIFO** — evicts the oldest resident line
- **LIFO** — evicts the most recently loaded line
- **MRU** — evicts the most recently used line
- **LFU** — evicts the least frequently accessed line
- **Random** — evicts a randomly selected line
- **Belady's optimal** — evicts the line whose next access is furthest in the future (possible in simulation since the full trace is known in advance)

### Write policies
- **Write-through** — writes propagate to main memory immediately
- **Write-back** — writes update only the cache; main memory is updated on eviction (uses dirty bit)
- **Write-allocate** — on a write miss, the block is loaded into cache before writing
- **No-write-allocate** — on a write miss, the write goes directly to main memory

### Multi-level cache hierarchy
- Configurable L1, L2, and L3 caches
- Each level independently configurable (size, associativity, block size, replacement policy)
- Hierarchical lookup with proper miss propagation between levels

### Cache miss classification
Misses are categorized using the three Cs model:
- **Compulsory** — first access to a block
- **Conflict** — caused by limited associativity (would not occur in a fully-associative cache of the same size)
- **Capacity** — caused by limited cache size (would occur even in a fully-associative cache)

### Performance analysis
- Hit/miss counts and rates per cache level
- Average memory access time (Tavg) calculation from user-provided latencies
- Side-by-side comparison of different configurations on the same trace
- Miss breakdown by type (compulsory, conflict, capacity)

## Motivation
This project was built from the ground up to deeply understand cache architecture and explore how mapping techniques, replacement policies, and write strategies interact to affect performance.

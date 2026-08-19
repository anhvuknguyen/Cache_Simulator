# Cache_Simulator

A configurable, trace-driven cache simulator built from scratch in C++. Simulates direct-mapped, set-associative, and fully-associative caches with configurable replacement policies, write strategies, and multi-level hierarchy support. Designed as both a learning tool and a performance analysis engine for exploring how cache design decisions affect hit rates and memory traffic.

## Scope

This simulator models cache **behavior**, not cache **contents**. No data is stored and main memory is not modeled — the simulator tracks which blocks are resident, which are dirty, and what traffic each access generates. Every metric it reports (hit rates, miss classification, memory traffic, Tavg) is derivable from that, which is why write policies are observable even without modeling the bytes being written.

## Features

### Mapping techniques
- **Direct-mapped** — each memory block maps to exactly one cache line
- **N-way set-associative** — configurable associativity (2-way, 4-way, 8-way, etc.)
- **Fully-associative** — any block can occupy any cache line

All three use a unified set-based architecture: direct-mapped is N sets of 1 line, fully-associative is 1 set of N lines, and N-way set-associative sits between them. A single `Cache` implementation handles all three; only the set/line geometry changes.

### Replacement policies

Each policy is implemented as a `Cache_set` subclass whose underlying data structure encodes the eviction rule directly, rather than by tagging cache lines with policy-specific metadata. `Cache_line` stays uniform across all policies.

- **LRU** — evicts the least recently used line (`std::list` + `unordered_map` of iterators for O(1) lookup and O(1) reordering)
- **FIFO** — evicts the oldest resident line
- **LIFO** — evicts the most recently loaded line
- **MRU** — evicts the most recently used line
- **LFU** — evicts the least frequently accessed line, breaking ties by least-recently-used
- **Random** — evicts a randomly selected line
- **Belady's optimal** — evicts the line whose next access is furthest in the future

Belady's is offline: it requires the complete future access stream, so the trace is pre-scanned and each set receives the filtered list of accesses that map to it. Because lower cache levels see a stream that depends on the runtime behavior of the levels above them, that future is not knowable in advance — **Belady's is therefore restricted to single-level configurations.**

### Write policies

Write behavior is two orthogonal choices — what happens on a write hit, and what happens on a write miss. This simulator supports the two standard pairings:

- **Write-back + write-allocate** — write hits set a dirty bit and update the cache only; write misses load the block first. Dirty lines generate a write to the next level when evicted, which may itself allocate and cascade downward.
- **Write-through + no-write-allocate** — write hits update the cache and propagate immediately to the next level; write misses bypass the cache entirely. This combination never produces dirty lines, so no writebacks occur.

Writebacks are counted as memory traffic but are not treated as demand accesses: they do not update hit/miss rates and are not classified into the three Cs, since they are not program references.

### Multi-level cache hierarchy
- Configurable number of levels, each independently parameterized (number of sets, lines per set, block size, mapping technique, replacement policy, write policy)
- A single-level cache is just a hierarchy of length one — there is no separate code path
- **Non-inclusive non-exclusive (NINE)** — levels do not enforce containment relationships. A block in L1 may or may not be present in L2, and no back-invalidation occurs when a lower level evicts. This matches a common real-hardware design point and avoids the invalidation machinery an inclusive hierarchy would require.
- Lower levels are only accessed on a miss above them, so each level sees a filtered access stream

### Cache miss classification

Misses are categorized using the three Cs model:
- **Compulsory** — first access to this block at this level
- **Conflict** — would have hit in a fully-associative cache of the same capacity; caused by limited associativity
- **Capacity** — would have missed even in a fully-associative cache of the same capacity

Conflict and capacity are distinguished by running a **shadow fully-associative LRU cache** alongside each level, sized to that level's total capacity and fed that level's access stream. A miss that hits in the shadow is a conflict miss; a miss that also misses in the shadow is a capacity miss.

Two consequences of this method are worth stating:
- A fully-associative cache cannot have conflict misses by definition, so the shadow is skipped entirely for such levels and all non-compulsory misses are classified as capacity.
- Each level is classified independently against its own shadow. "Conflict misses at L2" means conflicts arising from L2's geometry given the post-L1-filtering stream it actually saw — not conflicts as a fraction of all program references.

### Performance analysis
- Per-level hit and miss counts, reported as both local hit rate (hits ÷ accesses to that level) and global hit rate (hits ÷ total program references)
- Miss breakdown by type (compulsory, conflict, capacity) per level
- Memory traffic, including writebacks generated by dirty evictions
- Average memory access time (Tavg) from user-provided per-level hit latencies and a memory penalty

## Input format

Trace files are plain text, one access per line:

```
R 0x7fff5a3c
W 0x7fff5a3c
```

`R` denotes a read, `W` a write. Addresses are 32-bit hexadecimal.

## Configuration constraints

- Block size and number of sets must be powers of two
- `log2(block_size) + log2(num_sets)` must be less than 32
- Belady's optimal is available only for single-level configurations

Invalid configurations throw during construction and are reported to the user rather than silently corrected.

## Project structure

```
include/     types.h (enums, Level_config), class headers, CacheUtils
src/         Cache_line, Cache_set and its subclasses, Cache, Cache_hierarchy, main
traces/      test trace files
```

Built with `g++ -std=c++17`. Run `make` to build.

## Design notes

**Why one `Cache_line` class?** Every replacement policy can be expressed through the choice of data structure holding the lines, rather than through per-line metadata plus a policy switch. LRU needs ordering, LFU needs frequency, FIFO needs insertion order — but all of these live in the set, not the line. This keeps `Cache_line` to what a real cache line actually stores: a valid bit, a dirty bit, and a tag.

**Why a shadow cache instead of a heuristic?** The common shortcut — treating every miss after the cache has seen more unique blocks than it has lines as a capacity miss — is wrong, because conflict misses continue to occur past that point. Simulating the fully-associative reference directly is the standard academic method and gives an exact answer.

**Why NINE rather than inclusive?** Inclusion requires a back-invalidation path: when a lower level evicts a block, it must be removed from every level above it, including handling the case where the copy above is dirty. That machinery buys nothing pedagogically here and complicates every replacement policy's bookkeeping, since lines would disappear for reasons the policy did not choose.

## Motivation

This project was built from the ground up to deeply understand cache architecture and explore how mapping techniques, replacement policies, and write strategies interact to affect performance.

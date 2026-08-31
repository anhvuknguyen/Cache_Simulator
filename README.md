# Cache_Simulator

A configurable, trace-driven cache simulator built from scratch in C++. Simulates direct-mapped, set-associative, and fully-associative caches with configurable replacement policies, write strategies, and multi-level hierarchy support. The goal is an analysis tool: run the same trace against different cache structures and compare what changes.

## Scope

This simulator models cache **behavior**, not cache **contents**. No data is stored and main memory is not modeled — the simulator tracks which blocks are resident, which are dirty, and what traffic each access generates. Every metric it reports is derivable from that, which is why write policies are observable even without modeling the bytes being written.

Two consequences of not modeling data movement:

- Block sizes may differ per level. In real hardware a mismatch would require multi-block fills (one L1 line spanning several L2 blocks); here a writeback carries a block-aligned address and the level below decomposes it with its own geometry. Configurations where a lower level has a *smaller* block than the level above are permitted but not faithfully modeled.
- A 1-set × 1-line cache is degenerately both direct-mapped and fully-associative. Its miss classification depends on which `Mapping_Technique` was selected, since the fully-associative path skips the shadow cache. Avoid this geometry when comparing configurations.

## Quickstart

```bash
git clone https://github.com/<your-username>/Cache_Simulator.git
cd Cache_Simulator
make
./bin/cacheSim
```

The simulator prompts for the cache configuration — number of levels, and for each level the mapping technique, associativity, number of sets, block size, replacement policy, and write strategy — then asks for a trace file. Sample traces are in `traces/`.

Requires `g++` with C++17 support and `make`.

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
- **Belady's optimal** — evicts the line whose next access is furthest in the future, preferring lines never used again

Belady's is offline: it requires the complete future access stream, so the trace is pre-scanned and each set receives the filtered list of accesses that map to it. Because lower cache levels see a stream that depends on the runtime behavior of the levels above them, that future is not knowable in advance — **Belady's is therefore restricted to single-level configurations.**

Both reads and writes are memory accesses, so a write hit updates replacement metadata exactly as a read hit does — refreshing recency under LRU, incrementing frequency under LFU, and advancing the future stream under Belady's.

### Write policies

Write behavior is two orthogonal choices — what happens on a write hit, and what happens on a write miss. This simulator supports the two standard pairings, **configurable independently per level**:

- **Write-back + write-allocate** — write hits set a dirty bit and update the cache only; write misses fetch the block from the level below first. Dirty lines generate a write to the next level when evicted, which may itself allocate and cascade further down.
- **Write-through + no-write-allocate** — write hits update the cache and propagate immediately to the next level; write misses do not allocate and pass straight through. This combination never produces dirty lines, so such a level never originates a writeback.

Mixed configurations are supported and are the interesting case: an L1 write-through over an L2 write-back models a common real design, where L1 stays simple and L2 absorbs the resulting write traffic. Each level applies its own policy to whatever request type it receives, so a request may change kind as it descends — a write-allocate fetch travels downward as a *read*, while a write-through forward travels as a *write*.

### Accounting model

A level's counters increment exactly once per program reference that reaches it, and record the operation **that level was asked to perform**, not the original instruction type. A write miss at a write-back L1 therefore appears as a write miss at L1 and a *read* at L2, because L2 was asked to supply the block.

Two categories of traffic are deliberately excluded from demand accounting:

- **Fills** — installing a fetched block into a level that already recorded its miss
- **Writebacks** — a dirty victim pushed down by the level above

Writebacks are real data movement and are tracked separately (`Writebacks Received` per level, plus memory writes), but they do not touch hit/miss counters, are not classified into the three Cs, and do not update shadow caches. The reason is that a writeback is caused by the *replacement policy above*, not by the program: changing L1 from LRU to FIFO changes which blocks are written back without the program issuing a single different instruction. A writeback does, however, update the receiving line's replacement metadata, since it is a genuine write to that line.

### Multi-level cache hierarchy
- Configurable number of levels, each independently parameterized (number of sets, lines per set, block size, mapping technique, replacement policy, write policy)
- A single-level cache is just a hierarchy of length one — there is no separate code path
- **Non-inclusive non-exclusive (NINE)** — levels do not enforce containment relationships. A block in L1 may or may not be present in L2, and no back-invalidation occurs when a lower level evicts. This matches a common real-hardware design point and avoids the invalidation machinery an inclusive hierarchy would require.
- Lower levels are only accessed on a miss above them, so each level sees a filtered access stream

Reads, writes, and writebacks are each implemented as a recursive descent over levels, bottoming out at main memory. A single program write can therefore trigger a fetch descending one way and a chain of writebacks descending the other, each routed by the receiving level's own policy.

### Cache miss classification

Misses are categorized using the three Cs model:
- **Compulsory** — first access to this block at this level
- **Conflict** — would have hit in a fully-associative cache of the same capacity; caused by limited associativity
- **Capacity** — would have missed even in a fully-associative cache of the same capacity

Conflict and capacity are distinguished by running a **shadow fully-associative LRU cache** alongside each level, sized to that level's total capacity and fed that level's access stream. A miss that hits in the shadow is a conflict miss; a miss that also misses in the shadow is a capacity miss.

Two consequences of this method are worth stating:
- A fully-associative cache cannot have conflict misses by definition, so the shadow is skipped entirely for such levels and all non-compulsory misses are classified as capacity.
- Each level is classified independently against its own shadow. "Conflict misses at L2" means conflicts arising from L2's geometry given the post-L1-filtering stream it actually saw — not conflicts as a fraction of all program references.

### Reported statistics

Per level: total reads, read hits, read misses, total writes, write hits, write misses, writebacks received, evictions, and the three-Cs breakdown. Hierarchy-wide: memory reads and memory writes.

Average memory access time is derived from these counts together with the per-level hit latencies and memory penalty of whatever machine is being modeled. Latencies are properties of the hardware rather than of the access pattern, so the simulator reports the counts and leaves that arithmetic to the user.

## Validation

Correctness is checked two ways.

**Hand-computed trace keys.** Small traces with hand-derived expected output covering each replacement policy, each write policy, multi-level routing, and the interactions between them — including cases built specifically to distinguish policies that behave identically on simple traces.

**An automated invariant checker** asserting relationships that must hold regardless of configuration:

- L1's read and write counts equal the number of `R` and `W` lines in the trace
- Demand accesses at level *N+1* equal what level *N* sent down: its read misses, plus its write misses under write-back or its total writes under write-through
- Per level, `compulsory + conflict + capacity` equals total demand misses
- In an all-write-back hierarchy, total hits plus memory reads equals total references
- In an all-write-through hierarchy, memory writes equals the number of `W` lines, and memory reads is zero

These catch the failure modes that produce plausible-looking but wrong output: double-counted accesses, misses that never get classified, and writeback traffic leaking into demand statistics.

## Input format

Trace files are plain text, one access per line:

```
R 0x7fff5a3c
W 0x7fff5a3c
```

`R` denotes a read, `W` a write. Addresses are 32-bit hexadecimal.

## Output Format

**Sample of a single trace output:**
```
R 16
Tag: 2 Index: 0
Read Miss in L[1]
Compulsory Miss
Read Miss in L[2]
Compulsory Miss
Read Miss in L[3]
Compulsory Miss
Main Memory Read
Installment in L[3]; Block Address: 16
Eviction in L[2]; Block Address: 0; Dirty Bit: 0
Installment in L[2]; Block Address: 16
Eviction in L[1]; Block Address: 8; Dirty Bit: 1
Writeback Hit in L[2]
Wrote in L[2]; Block Address: 8
Installment in L[1]; Block Address: 16
```

**Sample output of Cache Representation**
```
L[1]:
---Set 0---  Evictions: 9
|Valid: 1 |Dirty: 1 |Tag: 4 |Address: 32
---Set 1---  Evictions: 0
|Valid: 0 |Dirty: 0 |Tag: 0 |Address: 0

L[2]:
---Set 0---  Evictions: 8
|Valid: 1 |Dirty: 0 |Tag: 4 |Address: 32
|Valid: 1 |Dirty: 1 |Tag: 2 |Address: 16
---Set 1---  Evictions: 0

L[3]:
---Set 0---  Evictions: 0
|Valid: 1 |Dirty: 0 |Tag: 8 |Address: 32
|Valid: 1 |Dirty: 1 |Tag: 0 |Address: 0
|Valid: 1 |Dirty: 1 |Tag: 6 |Address: 24
|Valid: 1 |Dirty: 0 |Tag: 4 |Address: 16
|Valid: 1 |Dirty: 1 |Tag: 2 |Address: 8
```

**Sample Output of Cache Stats**
```
Cache Stats:
                    L[1]    L[2]    L[3]    
        Total Reads: 5       10      10      
          Read Hits: 0       0       5       
        Read Misses: 5       10      5       
       Total Writes: 5       0       0       
         Write Hits: 0       0       0       
       Write Misses: 5       0       0       
Writebacks Received: 0       4       3       
          Evictions: 9       8       0       
  Compulsory Misses: 5       5       5       
    Conflict Misses: 0       0       0       
    Capacity Misses: 5       5       0       

      Memory  Reads: 5
      Memory Writes: 0
```

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

**Why one `Cache_line` class?** Every replacement policy can be expressed through the choice of data structure holding the lines, rather than through per-line metadata plus a policy switch. LRU needs ordering, LFU needs frequency, FIFO needs insertion order — but all of these live in the set, not the line. This keeps `Cache_line` to what a real cache line actually stores: a valid bit, a dirty bit, and a tag (Of course, without the actual data).

**Why a shadow cache instead of a heuristic?** Initially, every miss after the maximum capacity of the cache is reached was treated as a capacity miss. However, this property is incorrect, because conflict misses continue to occur past that point. Simulating the fully-associative reference directly is the standard academic method and gives an exact answer. Essentially, we decide what type of misses we've experienced by comparing it to a fully-associative cache of the same size; this is our capacity limit.

**Why is a re-fetched block clean?** Dirty means "differs from the level below," not "has ever been written." When L1 evicts a dirty block, the modified data moves to L2 and the obligation to write it to memory moves with it. A later fetch of that block into L1 produces a copy identical to L2's, so L1's copy is clean while L2's remains dirty. Marking the fetched copy dirty would write the same data to memory twice.

## Motivation

Built from the ground up to deeply understand cache architecture and explore how mapping techniques, replacement policies, and write strategies interact to affect performance.
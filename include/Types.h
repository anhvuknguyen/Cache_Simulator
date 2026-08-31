#ifndef TYPES_H
#define TYPES_H

namespace Cache_types{
    //Mapping Techniques
    enum class Mapping_Technique:char{
        Direct,
        Fully_Associative,
        Set_Associative
    };

    //Replacement Policy
    enum class Replacement_Policy:char{
        Direct,
        Random,
        FIFO,
        LIFO,
        MRU,
        LRU,
        LFU,
        Belady,
        QLRU,
        BRRIP,
        SRRIP,
        DRRIP
    };

    //Write Strategy
    enum class Write_Strategy:char{
        Write_Back_Write_Allocate,
        Write_Through_No_Write_Allocate
    };

    //Cache Miss Types
    enum class Miss_Type:char{
        Hit,
        Miss,
        null
    };

    //Operation
    enum class Operation:char{
        Read,
        Write
    };

    //Fetch Origin
    enum class Fetch_Origin:char{
        Processor,
        Write_Routine
    };

    //Level Configurations
    struct Level_config{
        int set_size;
        int num_sets;
        int num_blocks;
        Mapping_Technique mapping_Tech;
        Replacement_Policy replacement_Pol;
        Write_Strategy write_Strat;
    };

    //Eviction Return Type
    struct Evict_Return_T{
        int tag;
        int dirtyBit;
    };

    struct SRRIP_Pair{
        Cache_line line;
        int rrpv;
    };

    //Cache Level Stats
    struct Cache_level_stats{
        int reads;
        int read_hit_Count;
        int read_miss_Count;
        int writes;
        int write_hit_Count;
        int write_miss_Count;
        int writebacks_received;
        int eviction_Count;
        int compulsory_Miss_Count;
        int capacity_Miss_Count;
        int conflict_Miss_Count;
    };

    //Cache Level Details
    struct Cache_level_details{
        Mapping_Technique mapping_tech;
        Replacement_Policy replacement_pol;
        Write_Strategy write_strat;
        int cache_Size;
        int num_Sets;
        int lines_Per_Set;
        int total_lines;
        int block_Size;
        int num_TagBits;
        int num_IndexBits;
        int num_OffsetBits;
    };
}
#endif
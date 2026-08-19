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
        Belady
    };

    //Write Strategy
    enum class Write_Strategy:char{
        Write_Back_Write_Allocate,
        Write_Through_No_Write_Allocate
    };

    //Cache Miss Types
    enum class Miss_Type:char{
        Hit,
        Miss
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
}
#endif
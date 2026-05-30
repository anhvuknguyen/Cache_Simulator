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
        Write_Back,
        Write_Through
    };

    //Cache Miss Types
    enum class Miss_Types:char{
        Hit,
        Compulsory_Miss,
        Conflict_Miss,
        Capacity_Miss
    };
}
#endif
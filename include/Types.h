#ifndef TYPES
#define TYPES


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
        Pseudo_LRU,
        LFU
    };
}
#endif
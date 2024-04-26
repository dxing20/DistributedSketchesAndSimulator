#ifndef HASH_SEEDS_H
#define HASH_SEEDS_H

// Dataplane
#define IP_TO_EDGESWITCH_IDX_HASH_SEED 42
#define ECMP_NEXT_HOP_HASH_SEED 1

// Bloom Filter
// used to generate the hash functions on each sketch
#define BLOOM_FILTER_HASH_CHAIN_HASH_SEED 3

// KMV
#define KMV_HASH_SEED 1

// FiveTupleGen
#define RANDOM_VALUE_TO_FLOW_1_HASH_SEED 1
#define RANDOM_VALUE_TO_FLOW_2_HASH_SEED 2

// Evaluation
#define RANDOM_FLOW_FOR_BLOOM_FILTER_EVALUATION_HASH_SEED time(NULL)

//BLBF
#define BLBF_HASH_SEED 3

#endif // HASH_SEEDS_H
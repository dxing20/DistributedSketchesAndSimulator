#include "KMVSuperSpreader.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
#include "../utils/Seeds.hpp"
#include "../utils/SpookyV2.hpp"
#include <unordered_set>
#include <climits>
#include <algorithm>
#include "../utils/Frequency.hpp"
#include "../utils/debug_util.hpp"

#include <fstream>
#include <iostream>

// map
// key: group
// value: map of route len to memory size percentage

std::unordered_map<int, std::unordered_map<size_t, double>> KMVSuperSpreaderMemoryDistribution = {
    {0, {{5, 1.0}}},
    {1, {{5, 1.0}}},
    {2, {{1, 0.5}, {3, 0.5}}},
};

// std::unordered_map<int, std::unordered_map<size_t, double>> KMVSuperSpreaderMemoryDistribution = {
//     {0, {{5, 1.0}}},
//     {1, {{3, 1.0}}},
//     {2, {{1, 1.0}}},
// };

size_t getSeed(size_t group, size_t route_len)
{
    return (group << 16) | route_len + KMV_HASH_SEED;
}

KMVSuperSpreader::KMVSuperSpreader(void *_args)
{
    KMVSuperSpreaderArgs *args = reinterpret_cast<KMVSuperSpreaderArgs *>(_args);
    this->mem = args->mem;
    this->memsize = args->memsize;
    this->group = args->group;

    std::memset(this->mem, 0, this->memsize);

    auto group = KMVSuperSpreaderMemoryDistribution.find(this->group);

    assert(group != KMVSuperSpreaderMemoryDistribution.end());

    size_t remaining = group->second.size();
    size_t used_mem = 0;
    for (auto &route : group->second)
    {
        size_t required_mem = (int)(route.second * this->memsize) / sizeof(MaxHeapNode) * sizeof(MaxHeapNode);

        // fprintf(stdout, "group %u, route %u, required_mem %u\n", this->group, route.first, required_mem);

        assert(required_mem > sizeof(MaxHeapNode) && required_mem + used_mem <= this->memsize);

        // if this is the last route, use the rest of the memory
        if (remaining == 1)
        {
            this->heaps.insert(
                {route.first,
                 HeapData{
                     .memsize = this->memsize - used_mem,
                     .mem = (void *)((char *)this->mem + used_mem),
                     .heap = new MaxHeap((void *)((char *)this->mem + used_mem), this->memsize - used_mem)}});
            break;
        }
        else
        {
            // else, use the percentage of memory truncated
            // to the nearest size of MaxHeapNode
            this->heaps.insert(
                {route.first,
                 HeapData{
                     .memsize = required_mem,
                     .mem = (void *)((char *)this->mem + used_mem),
                     .heap = new MaxHeap((void *)((char *)this->mem + used_mem), required_mem)}});
            used_mem += required_mem;
        }
    }

    assert(this->heaps.size() == group->second.size());
}

KMVSuperSpreader::~KMVSuperSpreader()
{
    free(this->mem);
}

void *KMVSuperSpreader::initialize(void *args)
{
    return NULL;
}

void *KMVSuperSpreader::update(const void *packet)
{
    Packet *p = (Packet *)packet;
    auto mem_spread = KMVSuperSpreaderMemoryDistribution.find(this->group);

    if(mem_spread == KMVSuperSpreaderMemoryDistribution.end()){
        fprintf(stderr, "group %d not found in memory distribution\n", this->group);
        exit(1);
    }

    auto route = mem_spread->second.find(p->route_len);

    if (route == mem_spread->second.end())
    {
        return NULL;
    }

    if (p->cur > p->route_len/2)
    {
        return NULL;
    }

    auto heap = this->heaps.find(p->route_len)->second.heap;

    const void *flow = p->packet;
    unsigned int seed = getSeed(this->group, p->route_len);
    // fprintf(stdout, "seed: %lu, group %lu, routelen: %lu\n", seed, this->group, p->route_len);
    unsigned int flowHash = SpookyHash::Hash32(flow, 13, seed);
    // fprintf(stdout, "flowHash: %lu\n", flowHash);
    char src[6];
    memcpy(src, flow, 4);
    memcpy(src + 4, (char *)flow + 8, 2);
    heap->replaceMin(flowHash, src);
    return NULL;
}

void *KMVSuperSpreader::query(void *_args)
{
    KMVSuperSpreaderQuery *args = reinterpret_cast<KMVSuperSpreaderQuery *>(_args);
    return NULL;
}

void *KMVSuperSpreader::merge(void *args)
{
    // TODO
    return NULL;
}

void *KMVSuperSpreader::construct(void *args)
{
    // TODO
    return NULL;
}

void *KMVSuperSpreader::compress(void *args)
{
    // TODO
    return NULL;
}

KMVSuperSpreaderController::KMVSuperSpreaderController()
{
    this->sketch = std::vector<SketchBase *>();
}

KMVSuperSpreaderController::~KMVSuperSpreaderController()
{
}

KMVSuperSpreader *KMVSuperSpreaderController::newSketch(unsigned int memsize, void *mem, int group)
{
    // TODO
    KMVSuperSpreaderArgs args = {
        .mem = mem,
        .memsize = memsize,
        .group = (size_t)group
    };
    KMVSuperSpreader *kvm = new KMVSuperSpreader((void *)&args);
    this->sketch.push_back(kvm);
    this->group.push_back(group);
    return kvm;
}

void *KMVSuperSpreaderController::query(void *args, std::vector<int> *route)
{
    KMVSuperSpreaderQuery *query = reinterpret_cast<KMVSuperSpreaderQuery *>(args);
    std::string filePath = query->fileout;

    std::ofstream file(filePath);

    // MaxHeapNode *nodes;
    // map[route_len] -> map[group] -> map[src] -> set[hash]
    // std::unordered_map<size_t, std::unordered_map<size_t, std::unordered_map<std::string, std::unordered_set<unsigned int>>>> superspreadersByRoute;
    // superspreadersByRoute = std::unordered_map<size_t, std::unordered_map<size_t, std::unordered_map<std::string, std::unordered_set<unsigned int>>>>();

    size_t hash;
    unsigned char src[6];
    size_t k;

    std::unordered_map<size_t, std::unordered_map<size_t, size_t>> k_size;


    if (this->sketch.size() == 0){
        fprintf(stderr, "No sketches found\n");
        exit(1);
    }

    file << "actual_layer,measure_layer,switch_idx,k,sampled_hash,hash,src\n";

    for (int i=0; i<this->sketch.size(); i++){
        KMVSuperSpreader *s = reinterpret_cast<KMVSuperSpreader *>(this->sketch.at(i));
        size_t actual_layer = s->group;

        for (auto heap: s->heaps){
            size_t measure_layer = heap.first;
            MaxHeap *maxheap = heap.second.heap;
            size_t k = maxheap->capacity;
            size_t sample_size = maxheap->heapSize;

            

            for (size_t j=0; j<sample_size; j++){
                MaxHeapNode n = maxheap->heapArray[j];
                memcpy(src, n.value, 6);
                hash = n.key;

                

                file << actual_layer << "," << measure_layer << "," << i << "," << k << "," << sample_size << "," << hash << "," << convertToIPAddress(src) << ":" << *(unsigned short*)(src+4) << "\n";

            }

        }
    }
    file.close();

    return NULL;
}

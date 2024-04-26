#ifndef KMV_SUPERSPREADER_H
#define KMV_SUPERSPREADER_H


#include "SketchBase.hpp"
#include "../utils/MaxHeap.hpp"
#include <stdint.h>
#include <vector>
#include <cmath>
#include <cassert>
#include <unordered_map>
#include <string>




typedef struct HeapData{
    size_t memsize;
    void* mem;
    MaxHeap* heap;
}HeapData;

typedef struct KMVSuperSpreaderArgs{
    void* mem;
    size_t memsize;
    size_t group;
}KMVSuperSpreaderArgs;

typedef struct KMVSuperSpreaderQuery{
    char* fileout;
    void* srcs;
    size_t src_len;
}KMVSuperSpreaderQuery;

class KMVSuperSpreader : public SketchBase {
public:
    void* mem;
    size_t memsize;
    size_t group;
    std::unordered_map<size_t, HeapData> heaps;

    KMVSuperSpreader(void* args);
    ~KMVSuperSpreader();
    void* initialize(void* args);
    void* update(const void* args);
    void* query(void* args);
    void* merge(void* args);
    void* construct(void* args);
    void* compress(void* args);
};

class KMVSuperSpreaderController : public ControllerBase {
  public:
    std::vector<int> group;
    KMVSuperSpreaderController();
    ~KMVSuperSpreaderController();
    KMVSuperSpreader* newSketch(unsigned int memsize, void* mem, int group);
    void* query(void* args, std::vector<int>* route);
};

#endif // KMV_SUPERSPREADER_H
#ifndef MIN_HEAP_HPP
#define MIN_HEAP_HPP

#include <cstddef>
#include <unordered_map>
#include <string>

typedef struct MaxHeapNode {
    size_t key; 
    char value[6];
} MaxHeapNode;


class MaxHeap {
private:

    size_t parent(size_t i);

    size_t leftChild(size_t i);

    size_t rightChild(size_t i);

    void heapifyUp(size_t index);

    void heapifyDown(size_t index);

public:
    MaxHeapNode* heapArray;
    size_t heapSize;
    size_t capacity;
    std::unordered_map<std::string, size_t> valueToIndex;

    MaxHeap(void* mem, size_t mem_cap);

    void insert(const size_t key, const char value[6]);

    bool extractMax(size_t* key, char value[6]);

    void getMax(size_t* key, char value[6]);

    void replaceMin(size_t key, char value[6]);
};


#endif  // MIN_HEAP_HPP
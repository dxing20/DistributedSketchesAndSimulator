#include "MaxHeap.hpp"

#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <cassert>
#include <cstring>

size_t MaxHeap::parent(size_t i)
{
    return (i - 1) / 2;
}

size_t MaxHeap::leftChild(size_t i)
{
    return 2 * i + 1;
}

size_t MaxHeap::rightChild(size_t i)
{
    return 2 * i + 2;
}

void MaxHeap::heapifyUp(size_t index)
{
    while (index != 0 && heapArray[parent(index)].key < heapArray[index].key)
    {
        valueToIndex[std::string(heapArray[parent(index)].value, 6)] = index;
        valueToIndex[std::string(heapArray[index].value, 6)] = parent(index);

        std::swap(heapArray[parent(index)], heapArray[index]);
        index = parent(index);
    }
}

void MaxHeap::heapifyDown(size_t index)
{
    size_t maxIndex = index;
    size_t left = leftChild(index);
    size_t right = rightChild(index);

    if (left < heapSize && heapArray[left].key > heapArray[maxIndex].key)
        maxIndex = left;

    if (right < heapSize && heapArray[right].key > heapArray[maxIndex].key)
        maxIndex = right;

    if (index != maxIndex)
    {
        valueToIndex[std::string(heapArray[maxIndex].value, 6)] = index;
        valueToIndex[std::string(heapArray[index].value, 6)] = maxIndex;

        std::swap(heapArray[index], heapArray[maxIndex]);
        heapifyDown(maxIndex);
    }
}

MaxHeap::MaxHeap(void *mem, size_t mem_cap)
{
    heapArray = (MaxHeapNode *)mem;
    capacity = mem_cap / sizeof(MaxHeapNode);
    heapSize = 0;
    valueToIndex = std::unordered_map<std::string, size_t>();
}

void MaxHeap::insert(const size_t key, const char value[6])
{
    if (heapSize == capacity)
    {
        std::cout << "Heap overflow, cannot insert more elements." << std::endl;
        return;
    }

    heapSize++;
    size_t index = heapSize - 1;
    heapArray[index].key = key;
    std::memcpy(heapArray[index].value, value, 6);

    valueToIndex[std::string(value, 6)] = index;

    heapifyUp(index);
}

bool MaxHeap::extractMax(size_t *key, char value[6])
{
    if (heapSize <= 0)
        return false;

    if (heapSize == 1)
    {
        heapSize--;
        *key = heapArray[0].key;
        std::memcpy(value, heapArray[0].value, 6);

        valueToIndex.erase(std::string(value, 6));
        return true;
    }

    *key = heapArray[0].key;
    std::memcpy(value, heapArray[0].value, 6);
    valueToIndex.erase(std::string(value, 6));
    heapArray[0] = heapArray[heapSize - 1];
    valueToIndex[std::string(heapArray[0].value, 6)] = 0;
    heapSize--;
    heapifyDown(0);

    

    return true;
}

void MaxHeap::getMax(size_t *key, char value[6])
{
    if (heapSize <= 0)
    {
        *key = UINT_MAX;
        return;
    }

    *key = heapArray[0].key;
    std::memcpy(value, heapArray[0].value, 6);
}

void MaxHeap::replaceMin(size_t key, char value[6])
{
    // replaces a value in heap if
    // the key smaller than existing values

    if (heapSize < capacity)
    {
        insert(key, value);
        return;
    }

    size_t maxKey;
    char maxValue[6];
    getMax(&maxKey, maxValue);
    // all keys in heap is smaller
    // fprintf(stdout, "replaceMin: %u\n", key);
    if (key > maxKey)
    {
        return;
    }

    // try to find the value in heap
    // bool match;
    // for (size_t i = 0; i < heapSize; i++)
    // {
    //     match = true;
    //     for (size_t j = 0; j < 6; j++)
    //     {
    //         if (value[j] != heapArray[i].value[j])
    //         {
    //             match = false;
    //             break;
    //         }
    //     }
    //     if (match)
    //     {
    //         if (key < heapArray[i].key)
    //         {
    //             heapArray[i].key = key;
    //             heapifyDown(i);
    //         }
    //         return;
    //     }
    // }

    if (valueToIndex.find(std::string(value, 6)) != valueToIndex.end())
    {
        size_t index = valueToIndex.at(std::string(value, 6));
        if (key < heapArray[index].key)
        {
            heapArray[index].key = key;
            heapifyDown(index);
        }
        return;
    }


    // value not in heap, evict the largest value
    if (heapSize == capacity)
    {
        bool status = extractMax(&maxKey, maxValue);
        assert(status);
    }
    insert(key, value);


}

// int main()
// {
//     int mem_cap = 1024;
//     void *mem = malloc(mem_cap);
//     MaxHeap heap(mem, mem_cap);
//     size_t key;
//     char value[6];

//     heap.insert(3, "a");
//     heap.insert(2, "am");
//     heap.insert(1, "I");
//     heap.insert(5, "heap");
//     heap.insert(4, "good");
//     heap.insert(6, "wow");

//     heap.replaceMin(2, "wow");
//     heap.replaceMin(6, "heap");

//     while (heap.extractMax(&key, value))
//     {
//         std::cout << key << " " << value << std::endl;
//     }

//     free(mem);
// }
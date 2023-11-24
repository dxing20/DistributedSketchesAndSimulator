#include "BloomFilter.hpp"
#include <cstdio>

BloomFilter::BloomFilter(void* args) {
    printf("BloomFilter::BloomFilter()\n");
}

BloomFilter::~BloomFilter() {
    // TODO
}

void* BloomFilter::initialize(void* args) {
    // TODO

    return NULL;
}

void* BloomFilter::update(void* args) {
    // TODO
    return NULL;
}

void* BloomFilter::query(void* args) {
    // TODO
    return NULL;
}

void* BloomFilter::merge(void* args) {
    // TODO
    return NULL;
}

void* BloomFilter::construct(void* args) {
    // TODO
    return NULL;
}

void* BloomFilter::compress(void* args) {
    // TODO
    return NULL;
}

BloomFilterController::BloomFilterController() {
    // TODO
}

BloomFilterController::~BloomFilterController() {
    // TODO
}

BloomFilter* BloomFilterController::newSketch(unsigned int memsize, void* mem) {
    // TODO
    return NULL;
}

void* BloomFilterController::query(void* args) {
    // TODO
    return NULL;
}

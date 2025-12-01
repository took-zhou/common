#include <iostream>

struct AllocationMertics {
  uint32_t total_allocated = 0;
  uint32_t total_freed = 0;
};

static AllocationMertics s_allocation_metrics;

void *operator new(size_t size) {
  s_allocation_metrics.total_allocated += size;
  return malloc(size);
}

void operator delete(void *memory, size_t size) {
  s_allocation_metrics.total_freed += size;
  free(memory);
}

void PrintMemoryUsage() {
  std::cout << "Memory usage: " << s_allocation_metrics.total_allocated - s_allocation_metrics.total_freed << " bytes\n";
}

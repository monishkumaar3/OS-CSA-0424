#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define PAGE_SIZE 4096  // 4KB pages
#define NUM_PAGES 16    // Total number of pages in physical memory
#define NUM_VPAGES 64   // Total number of virtual pages
#define MEMORY_SIZE (NUM_PAGES * PAGE_SIZE)
#define VIRTUAL_MEMORY_SIZE (NUM_VPAGES * PAGE_SIZE)

// Simulated physical memory
uint8_t physical_memory[MEMORY_SIZE];

// Simulated backing store for pages not in physical memory
uint8_t backing_store[VIRTUAL_MEMORY_SIZE];

// Page table entry structure
typedef struct {
    bool valid;          // Valid bit
    uint32_t frame_num;  // Physical frame number
} page_table_entry;

// Page table structure
page_table_entry page_table[NUM_VPAGES];

// Initialize the simulated backing store
void initialize_backing_store() {
    for (int i = 0; i < VIRTUAL_MEMORY_SIZE; i++) {
        backing_store[i] = i % 256; // Fill with a repeating pattern for simplicity
    }
}
void initialize_page_table() {
    for (int i = 0; i < NUM_VPAGES; i++) {
        page_table[i].valid = false;
        page_table[i].frame_num = -1;
    }
}
uint32_t translate_address(uint32_t virtual_address) {
    uint32_t page_num = virtual_address / PAGE_SIZE;
    uint32_t offset = virtual_address % PAGE_SIZE;

    if (page_num >= NUM_VPAGES) {
        fprintf(stderr, "Invalid virtual address: 0x%x\n", virtual_address);
        exit(1);
    }

    if (!page_table[page_num].valid) {
        // Page fault handling
        fprintf(stderr, "Page fault at virtual address: 0x%x\n", virtual_address);

        // Find a free frame in physical memory
        uint32_t frame_num = page_num % NUM_PAGES; // Simplified frame allocation for this example

        // Load the page from the backing store into the physical memory
        memcpy(&physical_memory[frame_num * PAGE_SIZE],
               &backing_store[page_num * PAGE_SIZE],
               PAGE_SIZE);

        // Update the page table entry
        page_table[page_num].valid = true;
        page_table[page_num].frame_num = frame_num;
    }

    uint32_t physical_address = page_table[page_num].frame_num * PAGE_SIZE + offset;
    return physical_address;
}
void test_virtual_memory_manager() {
    // Initialize the backing store with some data
    initialize_backing_store();

    // Initialize the page table
    initialize_page_table();

    // Test translating some virtual addresses
    uint32_t virtual_addresses[] = {0x0000, 0x1000, 0x2000, 0x3000, 0x8000, 0x9000, 0xF000};
    int num_addresses = sizeof(virtual_addresses) / sizeof(virtual_addresses[0]);

    for (int i = 0; i < num_addresses; i++) {
        uint32_t va = virtual_addresses[i];
        uint32_t pa = translate_address(va);
        printf("Virtual address: 0x%x -> Physical address: 0x%x\n", va, pa);
    }

    // Access memory using translated addresses
    for (int i = 0; i < num_addresses; i++) {
        uint32_t va = virtual_addresses[i];
        uint32_t pa = translate_address(va);
        printf("Data at virtual address 0x%x: %d\n", va, physical_memory[pa]);
    }
}

int main() {
    test_virtual_memory_manager();
    return 0;
}


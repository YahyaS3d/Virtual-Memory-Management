# Simulation Memory Management

This repository contains a C++ program that simulates memory management for a computer system. The program is designed to help you understand memory management concepts, including paging and page fault handling.

## Overview

The `sim_mem` program simulates the memory management of a computer system. It manages a simplified memory hierarchy consisting of main memory and a swap file. The main features of this simulation include:

- **Memory Segments**: The program manages different memory segments, including text, data, BSS, heap, and stack.

- **Paging**: Memory is divided into fixed-size pages, and the program simulates page-level memory management.

- **Page Table**: A page table data structure is used to keep track of the mapping between virtual and physical addresses.

- **Page Fault Handling**: When a page is accessed and not present in memory, a page fault occurs. The program simulates page fault handling by loading pages from an executable file into main memory.

## Usage

To use the `sim_mem` program, follow these steps:

1. Clone the repository to your local machine.

2. Build the program using a C++ compiler. You can use the following command to build the program:

   ```bash
   g++ sim_mem.cpp -o sim_mem
   ```

3. Run the program, providing the necessary command-line arguments. For example:

   ```bash
   ./sim_mem executable.bin swapfile.bin 1024 512 256 256 64
   ```

   - `executable.bin`: The name of the executable file to load into memory.
   - `swapfile.bin`: The name of the swap file.
   - `1024`: Text segment size (in bytes).
   - `512`: Data segment size (in bytes).
   - `256`: BSS segment size (in bytes).
   - `256`: Heap and stack segment size (in bytes).
   - `64`: Page size (in bytes).

4. The program will simulate memory management operations, including loading pages, storing values, and handling page faults.

## Program Output

The program provides the following output:

- **Memory State**: It displays the contents of main memory, showing the value stored at each address.

- **Swap Memory**: It displays the contents of the swap file, showing the value of each page.

- **Page Table**: It shows the current state of the page table, indicating which pages are valid, dirty, and their corresponding frame and swap index.

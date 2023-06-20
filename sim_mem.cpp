// sim_mem.cpp
#include "sim_mem.h"
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>

char main_memory[MEMORY_SIZE];

sim_mem::sim_mem(char exe_file_name[], char swap_file_name[], int text_size, int data_size, int bss_size, int heap_stack_size, int page_size)
        : text_size(text_size), data_size(data_size), bss_size(bss_size), heap_stack_size(heap_stack_size), page_size(page_size)
{
    // Check if the file names are not null
    if (exe_file_name == nullptr || swap_file_name == nullptr) {
        std::cerr << "Invalid file name." << std::endl;
        exit(1);
    }

    // Open executable file
    program_fd = open(exe_file_name, O_RDONLY);
    if (program_fd == -1) {
        std::cerr << "Error opening executable file." << std::endl;
        exit(1);
    }

    // Open or create swap file
    swapfile_fd = open(swap_file_name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (swapfile_fd == -1) {
        std::cerr << "Error opening or creating swap file." << std::endl;
        exit(1);
    }

    // Start the main memory with zeros
    memset(main_memory, 0, MEMORY_SIZE);

    // Calculate the number of pages for each segment
    int num_of_txt_pages = (text_size + page_size - 1) / page_size;
    int num_of_data_pages = (data_size + page_size - 1) / page_size;
    int num_of_bss_pages = (bss_size + page_size - 1) / page_size;
    int num_of_heap_stack_pages = (heap_stack_size + page_size - 1) / page_size;

    // Calculate the total number of pages
    num_of_pages = num_of_txt_pages + num_of_data_pages + num_of_bss_pages + num_of_heap_stack_pages;

    // Allocate memory for the page table
    page_table = new page_descriptor*[num_of_pages];

    // Initialize page table entries
    int frame_index = 0;

    // Text segment
    for (int i = 0; i < num_of_txt_pages; i++) {
        page_table[frame_index] = new page_descriptor();
        page_table[frame_index]->valid = true;
        page_table[frame_index]->frame = frame_index;
        page_table[frame_index]->dirty = false;
        page_table[frame_index]->swap_index = -1;
        frame_index++;
    }

    // Data segment
    for (int i = 0; i < num_of_data_pages; i++) {
        page_table[frame_index] = new page_descriptor();
        page_table[frame_index]->valid = true;
        page_table[frame_index]->frame = frame_index;
        page_table[frame_index]->dirty = false;
        page_table[frame_index]->swap_index = -1;
        frame_index++;
    }

    // BSS segment
    for (int i = 0; i < num_of_bss_pages; i++) {
        page_table[frame_index] = new page_descriptor();
        page_table[frame_index]->valid = true;
        page_table[frame_index]->frame = frame_index;
        page_table[frame_index]->dirty = false;
        page_table[frame_index]->swap_index = -1;
        frame_index++;
    }

    // Heap and Stack segments
    for (int i = 0; i < num_of_heap_stack_pages; i++) {
        page_table[frame_index] = new page_descriptor();
        page_table[frame_index]->valid = true;
        page_table[frame_index]->frame = frame_index;
        page_table[frame_index]->dirty = false;
        page_table[frame_index]->swap_index = -1;
        frame_index++;
    }
}

sim_mem::~sim_mem()
{
    // Close open file descriptors
    close(program_fd);
    close(swapfile_fd);

    // Free memory allocated for the page table
    for (int i = 0; i < 4; i++) {
        delete[] page_table[i];
    }
    delete[] page_table;
}

char sim_mem::load(int address)
{
    // Check if the address is within the valid range
    if (address < 0 || address >= MEMORY_SIZE) {
        std::cerr << "Invalid memory address." << std::endl;
        return '\0';
    }

    // Calculate the page number and offset
    int page_number = address / page_size;
    int offset = address % page_size;

    // Check if the page is valid
    if (!page_table[page_number / num_of_pages][page_number % num_of_pages].valid) {
        // Handle page fault - Load the page from the executable file

        // Calculate the offset in the executable file
        int exe_offset = (page_number * page_size) % MEMORY_SIZE;

        // Read the page from the executable file into main memory
        lseek(program_fd, exe_offset, SEEK_SET);
        int bytes_read = read(program_fd, &main_memory[page_number * page_size], page_size);

        if (bytes_read != page_size) {
            std::cerr << "Error reading from executable file." << std::endl;
            return '\0';
        }

        // Update the page table entry
        page_table[page_number / num_of_pages][page_number % num_of_pages].valid = true;
        page_table[page_number / num_of_pages][page_number % num_of_pages].dirty = false;
        page_table[page_number / num_of_pages][page_number % num_of_pages].frame = page_number;
        page_table[page_number / num_of_pages][page_number % num_of_pages].swap_index = -1;
    }

    // Calculate the physical address
    int physical_address = page_table[page_number / num_of_pages][page_number % num_of_pages].frame * page_size + offset;

    // Access the physical address and return the value
    return main_memory[physical_address];
}

void sim_mem::store(int address, char value)
{
    // Check if the address is within the valid range
    if (address < 0 || address >= MEMORY_SIZE) {
        std::cerr << "Invalid memory address." << std::endl;
        return;
    }

    // Calculate the page number and offset
    int page_number = address / page_size;
    int offset = address % page_size;

    // Check if the page is valid
    if (!page_table[page_number / num_of_pages][page_number % num_of_pages].valid) {
        // Handle page fault - Load the page from the executable file

        // Calculate the offset in the executable file
        int exe_offset = (page_number * page_size) % MEMORY_SIZE;

        // Read the page from the executable file into main memory
        lseek(program_fd, exe_offset, SEEK_SET);
        int bytes_read = read(program_fd, &main_memory[page_number * page_size], page_size);

        if (bytes_read != page_size) {
            std::cerr << "Error reading from executable file." << std::endl;
            return;
        }

        // Update the page table entry
        page_table[page_number / num_of_pages][page_number % num_of_pages].valid = true;
        page_table[page_number / num_of_pages][page_number % num_of_pages].dirty = false;
        page_table[page_number / num_of_pages][page_number % num_of_pages].frame = page_number;
        page_table[page_number / num_of_pages][page_number % num_of_pages].swap_index = -1;
    }

    // Calculate the physical address
    int physical_address = page_table[page_number / num_of_pages][page_number % num_of_pages].frame * page_size + offset;

    // Store the value at the physical address
    main_memory[physical_address] = value;
}

void sim_mem::print_memory()
{
    std::cout << "\nPhysical memory" << std::endl;
    for (char value : main_memory) {
        std::cout << "[" << value << "]" << std::endl;
    }
}

void sim_mem::print_swap() const
{
    char* buffer = new char[page_size];
    std::cout << "\nSwap memory" << std::endl;

    lseek(swapfile_fd, 0, SEEK_SET); // Go to the start of the swap file

    // Read and print each page in the swap file
    int pageNumber = 0;
    while (read(swapfile_fd, buffer, page_size) == page_size) {
        std::cout << "Page " << pageNumber << " - ";
        for (int i = 0; i < page_size; i++) {
            std::cout << "[" << buffer[i] << "]\t";
        }
        std::cout << std::endl;
        pageNumber++;
    }

    delete[] buffer;
}
void sim_mem::print_page_table()
{
    std::cout << "Valid\tDirty\tFrame\tSwap index" << std::endl;

    // Iterate over the page table and print the values of each page descriptor
    for (int i = 0; i < num_of_pages; i++) {
        for (int j = 0; j < num_of_proc; j++) {
            std::cout << "[" << page_table[j][i].valid << "]\t"
                      << "[" << page_table[j][i].dirty << "]\t"
                      << "[" << page_table[j][i].frame << "]\t"
                      << "[" << page_table[j][i].swap_index << "]" << std::endl;
        }
    }
}
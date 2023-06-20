#include "sim_mem.h"

int main()
{
    char val;
    sim_mem mem_sm("exec_file", "swap_file", 16, 16, 32, 32, 16);
    mem_sm.store(98, 'X');

    val = mem_sm.load(8);
    if (val != '\0') {
        std::cout << "Load successful. Value: " << val << std::endl;
    } else {
        std::cout << "Load failed." << std::endl;
    }

    mem_sm.print_memory();
    mem_sm.print_swap();
    mem_sm.print_page_table();
    return 0;
}
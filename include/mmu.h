#ifndef __MMU_H_
#define __MMU_H_

#include <iostream>
#include <string>
#include <vector>
#include <typeinfo>

typedef struct Variable {
    std::string name;
    int virtual_address;
    int size;
    int number_elements;
} Variable;

typedef struct Process {
    uint32_t pid;
    std::vector<Variable*> variables;
} Process;

class Mmu {
private:
    uint32_t _next_pid;
    int _max_size;
    std::vector<Process*> _processes;

public:
    Mmu(int memory_size);
    ~Mmu();

    int getNumVariables(int pid, std::string name);
    int getVirtualAddress(int pid, std::string name);
    uint32_t createProcess();
    void printMmu();
    void printProcesses();
    void createAllocate(int pid, int text_size, int data_size);
    uint32_t allocate(int pid, std::string var_name, std::string data_type, int number_of_elements);
};

#endif // __MMU_H_

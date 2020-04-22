#include "mmu.h"

Mmu::Mmu(int memory_size)
{
    _next_pid = 1024;
    _max_size = memory_size;
}

Mmu::~Mmu()
{
}

uint32_t Mmu::createProcess()
{
    Process *proc = new Process();
    proc->pid = _next_pid;

    Variable *var = new Variable();
    var->name = "<FREE_SPACE>";
    var->virtual_address = 0;
    var->size = _max_size;
    proc->variables.push_back(var);

    _processes.push_back(proc);

    _next_pid++;
    return proc->pid;
}

void Mmu::createAllocate(int pid, int text_size, int data_size){
    Process *proc;
    for (int i = 0; i < _processes.size(); i++){
        if (_processes[i]->pid == pid){
            proc = _processes[i];
        }
    }
    Variable *var = new Variable();
    var->name = "<TEXT>";
    var->virtual_address = 0;
    var->size = text_size;
    proc->variables.push_back(var);

    Variable *var1 = new Variable();
    var1->name = "<GLOBALS>";
    var1->virtual_address = text_size;
    var1->size = data_size;
    proc->variables.push_back(var1);

    Variable *var2 = new Variable();
    var2->name = "<STACK>";
    var2->virtual_address = text_size + data_size;
    var2->size = 65536;
    proc->variables.push_back(var2);
}

uint32_t Mmu::allocate(int pid, std::string var_name, std::string data_type, int number_of_elements){
    return 0;
}

void Mmu::printMmu()
{
    int i, j;

    std::cout << " PID  | Variable Name | Virtual Addr | Size" << std::endl;
    std::cout << "------+---------------+--------------+------------" << std::endl;
    for (i = 0; i < _processes.size(); i++)
    {
        for (j = 0; j < _processes[i]->variables.size(); j++)
        {
            // TODO: print all variables (excluding <FREE_SPACE> entries)
            if (_processes[i]->variables[j]->name != "<FREE_SPACE>"){
                printf("%5d | %13s |   0x%08X | %10d\n", _processes[i]->pid, _processes[i]->variables[j]->name.c_str(), _processes[i]->variables[j]->virtual_address, _processes[i]->variables[j]->size);
            }
        }
    }
}

void Mmu::printProcesses(){
    for (int i = 0; i < _processes.size(); i++){
        std::cout << _processes[i]->pid << std::endl;
    }
}
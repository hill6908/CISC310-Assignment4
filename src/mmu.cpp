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
    var->number_elements = 0;
    proc->variables.push_back(var);

    _processes.push_back(proc);

    _next_pid++;
    return proc->pid;
}

int Mmu::getNumVariables(int pid, std::string name)
{
    /*  use typeid to get the type and divide by the size of 
        the type to get the number of values    */
    Process *proc;
    for (int i = 0; i < _processes.size(); i++){
        if (_processes[i]->pid == pid){
            proc = _processes[i];
        }
    }

    for(int j = 0; j < proc->variables.size();j++)
    {
        if(proc->variables[j]->name == name)
        {
            return proc->variables[j]->number_elements;
        }
    }

}

//returns the virutal address for a variable in a process with pid and name 
int Mmu::getVirtualAddress(int pid, std::string name)
{
     Process *proc;
    for (int i = 0; i < _processes.size(); i++){
        if (_processes[i]->pid == pid){
            proc = _processes[i];
        }
    }

    for(int j = 0; j < proc->variables.size(); j++)
    {
        if(proc->variables[j]->name == name)
        {
            return proc->variables[j]->virtual_address;
        }
    }

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
    var->size = text_size;;
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

    //find the process with the matching pid
    Process *proc;
    for (int i = 0; i < _processes.size(); i++){
        if (_processes[i]->pid == pid){
            proc = _processes[i];
        }
    }

    Variable *newVar = new Variable();
    newVar->name = var_name;

    //set the size of the variables on the stack
    if(data_type == "char")
    {
        newVar->size = number_of_elements;
    }
    else if(data_type == "short")
    {
        newVar->size = (2*number_of_elements);
    }
    else if(data_type == "int" || data_type == "float")
    {
        newVar->size = (4*number_of_elements);
    }
    else //its a long or a double
    {
        newVar->size = (8*number_of_elements);
    }

    /* need to retrieve the last variable on the stack, get that virtual address 
        and add the size to it to make the new virtual address
    */
    Variable *backVar = new Variable();
    backVar = proc->variables.back();

    std::cout << "previous var" << backVar->name <<std::endl;

    newVar->number_elements = number_of_elements;
    newVar->virtual_address = backVar->virtual_address + backVar->size;
    std::cout << backVar->virtual_address << " + " <<backVar->size <<std::endl;
    proc->variables.push_back(newVar);

    std::cout << newVar->virtual_address <<std::endl;
    return newVar->virtual_address;
}

int Mmu::setValues(int pid, std::string name, int offset)
{
    //return the virtual address to be passed to the page table for conversion
     Process *proc;
    for (int i = 0; i < _processes.size(); i++){
        if (_processes[i]->pid == pid){
            proc = _processes[i];
        }
    }

    for(int j = 0; j < proc->variables.size(); j++)
    {
        if(proc->variables[j]->name == name)
        {
            int size_of_elements = proc->variables[j]->size / proc->variables[j]->number_elements;
            return proc->variables[j]->virtual_address + (offset * size_of_elements);
        }
    }

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
            // print all variables (excluding <FREE_SPACE> entries)
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

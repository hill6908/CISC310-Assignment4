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
    var->type = "free";
    var->places = 0;
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

std::string Mmu::getType(int pid, std::string name)
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
            return proc->variables[j]->type;
        }
    }
}

void Mmu::setPlaces(int pid, std::string name, int numPlaces){
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
            proc->variables[j]->places = numPlaces;
        }
    }
}

int Mmu::getPlaces(int pid, std::string name){
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
            return proc->variables[j]->places;
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

    //move the free space to after the text, globals, stack 

    Variable *free = new Variable();
    free = proc->variables[0];
    proc->variables.erase(proc->variables.begin());

    Variable *var = new Variable();
    var->name = "<TEXT>";
    var->virtual_address = 0;
    var->size = text_size;
    var->type = "text";
    var->places = 0;
    proc->variables.push_back(var);

    Variable *var1 = new Variable();
    var1->name = "<GLOBALS>";
    var1->virtual_address = text_size;
    var1->size = data_size;
    var1->type = "globals";
    var->places = 0;
    proc->variables.push_back(var1);

    Variable *var2 = new Variable();
    var2->name = "<STACK>";
    var2->virtual_address = text_size + data_size;
    var2->size = 65536;
    var2->type = "stack";
    var->places = 0;
    proc->variables.push_back(var2);

    free->virtual_address = text_size + data_size + 65536;

    proc->variables.push_back(free);
    for(int i = 0; i < proc->variables.size(); i++)
    {
        if(proc->variables[i]->name == "<FREE_SPACE>")
        {
            std::cout<< "Free space at: " << i << std::endl;
        }
    }
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
        newVar->type = "char";
        newVar->places = 0;
    }
    else if(data_type == "short")
    {
        newVar->size = (2*number_of_elements);
        newVar->type = "short";
        newVar->places = 0;
    }
    else if(data_type == "int" || data_type == "float")
    {
        newVar->size = (4*number_of_elements);
        if (data_type == "int"){
            newVar->type = "int";
        }
        else{
            newVar->type = "float";
        }
        newVar->places = 0;
    }
    else //its a long or a double
    {
        newVar->size = (8*number_of_elements);
        if (data_type == "long"){
            newVar->type = "long";
        }
        else if (data_type == "double"){
            newVar->type = "double";
        }
        newVar->places = 0;
    }

    newVar->number_elements = number_of_elements;
   
    /*  add the variable at the end of the free variable, make the free variable size smaller
        the virtual address is going to be the virtual address of the free variable + the new 
        size of the virtual address */
    
    Variable *space = new Variable(); 
    for(int i = 0; i < proc->variables.size(); i ++)
    {
        if(proc->variables[i]->name == "<FREE_SPACE>" && proc->variables[i]->size >= newVar->size)
        {
            std::cout << "free space" << std::endl;
            //found the free space 
            space = proc->variables[i];

            //pop the free space off? 
            //proc->variables.erase(proc->variables[i]);
            if(proc->variables[i] != proc->variables.back())
            {
                //next variable isn't a free space 
                //virtual address and size stays the same 
                space->name = newVar->name;
                space->number_elements = newVar->number_elements;
                std::cout << space->virtual_address <<std::endl;
                return space->virtual_address;
            }

            else
            {
                //the free variable the only thing left the vector, need to update the free space 
                proc->variables.erase(proc->variables.begin() + i);
                newVar->virtual_address = space->virtual_address;
                space->virtual_address = newVar->virtual_address + newVar->size;
                proc->variables.push_back(newVar);
                proc->variables.push_back(space);
                std::cout << newVar->virtual_address <<std::endl;
                return newVar->virtual_address;

            }
        }
    }
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

void Mmu::free(int pid, std::string name)
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
            proc->variables[j]->name = "<FREE_SPACE>";
            proc->variables[j]->number_elements = 0;
            proc->variables[j]->type = "free";
            proc->variables[j]->places = 0;
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
                printf("%5d | %-13s |   0x%08X | %10d\n", _processes[i]->pid, _processes[i]->variables[j]->name.c_str(), _processes[i]->variables[j]->virtual_address, _processes[i]->variables[j]->size);
            }
        }
    }
}


void Mmu::printProcesses(){
    for (int i = 0; i < _processes.size(); i++){
        std::cout << _processes[i]->pid << std::endl;
    }
}

void Mmu::terminate(int pid){
    for (int i = 0; i < _processes.size(); i++){
        if (_processes[i]->pid == pid){
            _processes.erase(_processes.begin() + i);
            break;
        }
    }
}

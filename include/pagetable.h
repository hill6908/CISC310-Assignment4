#ifndef __PAGETABLE_H_
#define __PAGETABLE_H_

#include <iostream>
#include <string>
#include <map>
#include <array>

class PageTable {
private:
    int _page_size;
    int current_page;
    std::map<std::string, int> _table;

public:
    PageTable(int page_size);
    ~PageTable();

    int getPageNumber(int virtual_address);
    void addEntry(uint32_t pid, int page_number);
    int getPhysicalAddress(uint32_t pid, int virtual_address);
    //void create(int pid, int text_size, int data_size);
    void print();
};

#endif // __PAGETABLE_H_

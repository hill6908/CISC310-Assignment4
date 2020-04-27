#include "pagetable.h"

PageTable::PageTable(int page_size)
{
    _page_size = page_size;
    _current_page_num = 0;
    _current_frame = 0;
}

PageTable::~PageTable()
{

}

int PageTable::getPageNumber()
{
    return _current_page_num;
}

void PageTable::addEntry(uint32_t pid, int page_number)
{
    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);

    // Find free frame
    // TODO: implement this!

    //memory / page size = max number of pages 
    if(_current_page_num > 67108864 / _page_size)
    {
        std::cout<< "Allocation would exceed system memory. ";
    }

    else
    {
        int frame = _current_frame;
        _table[entry] = frame;
        
        _current_frame += 1;
        _current_page_num +=1;
    }
}

/*int PageTable::allocate(int pid, std::string data_type, int number_of_elements)
{


}*/

int PageTable::getPhysicalAddress(uint32_t pid, int virtual_address)
{
    // Convert virtual address to page_number and page_offset
     /* mod virtual address by pagesize to get offset
        divide virtual address by pagesize to get page number    */ 
    int page_number = virtual_address/_page_size;
    int page_offset = virtual_address % _page_size;


    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);
    
    // If entry exists, look up frame number and convert virtual to physical address
    int address = -1;
    if (_table.count(entry) > 0)
    {
        /*  Physcial address =  frame * page_size + offset*/
       int frame = _table[entry];
       address = (frame*_page_size) + page_offset;
    }

    return address;
}

void PageTable::print()
{
    std::map<std::string, int>::iterator it = _table.begin();

    std::cout << " PID  | Page Number | Frame Number" << std::endl;
    std::cout << "------+-------------+--------------" << std::endl;

    for(std::pair<std::string,int> element : _table )
    {
        std::string pid_page = element.first;
        int frame = element.second;

        std::string delim = "|";
        std::string pid = pid_page.substr(0,pid_page.find(delim));
        int page = std::stoi(pid_page.substr(pid_page.find(delim) + 1, pid_page.length()));

        if(page < 10)
        {
            std::cout << pid <<" |           " << page << " |           " << frame <<std::endl;
        }
        else if (page > 10 && page < 100)
        {
            std::cout << pid <<" |          " << page << " |           " << frame << std::endl;
        }
        else
        {
            std::cout<< pid << " |         " << page << " |           " << frame <<std::endl;
        }
    }
}

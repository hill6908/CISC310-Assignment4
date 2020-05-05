#include "pagetable.h"

PageTable::PageTable(int page_size)
{
    _page_size = page_size;
}
PageTable::~PageTable()
{

}

int PageTable::getPageNumber(int virtual_address)
{
    int page_number = virtual_address / _page_size;

    return page_number;
}

void PageTable::terminatePID(uint32_t pid){
    int i = 0;
    std::string entry = std::to_string(pid) + "|" + std::to_string(i);

    while (_table.find(entry) != _table.end()){
        _table.erase(entry);
        i++;
        entry = std::to_string(pid) + "|" + std::to_string(i);
    }
}

void PageTable::deleteEntry(uint32_t pid, int page_number)
{
    // get rid of the page from the map 
    _table.erase(std::to_string(pid) + "|" + std::to_string(page_number));
}

void PageTable::addEntry(uint32_t pid, int page_number, int size)
{
    int numLoops = (size / _page_size) + 1;

    for (int i = 0; i < numLoops; i++){
        // Combination of pid and page number act as the key to look up frame number
        std::string entry = std::to_string(pid) + "|" + std::to_string(page_number+i);

        //If entry is not already in the table
        //memory / page size = max number of pages 
        if((page_number + 1 * _page_size) + size > 67108864)
        {
            std::cout<< "Allocation would exceed system memory. ";
        }
        else{
            if (_table.find(entry) == _table.end()){
                int frame = 0;
                //iterate through the map and find the last value of a frame & add 1?
                for (auto it : _table)
                {
                    if(it.second >= frame)
                    {
                        frame = it.second + 1;
                    }
                }

                _table[entry] = frame;
            }
        }
    }
}

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
            std::cout << pid <<"  |           " << page << " |           " << frame <<std::endl;
        }
        else if (page > 10 && page < 100)
        {
            std::cout << pid <<"  |          " << page << " |           " << frame << std::endl;
        }
        else
        {
            std::cout<< pid << " |         " << page << " |           " << frame <<std::endl;
        }
    }
}

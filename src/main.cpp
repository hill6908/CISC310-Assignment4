#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <typeinfo>
#include "mmu.h"
#include "pagetable.h"

void printStartMessage(int page_size);
void parseCommandLineInput(std::vector<std::string> input, uint8_t *memory, PageTable *page_table, Mmu *mmu);

int main(int argc, char **argv)
{
    // Ensure user specified page size as a command line parameter
    if (argc < 2)
    {
        fprintf(stderr, "Error: you must specify the page size\n");
        return 1;
    }

    // Print opening instuction message
    int page_size = std::stoi(argv[1]);
    printStartMessage(page_size);

    // Create physical 'memory'
    uint8_t *memory = new uint8_t[67108864]; // 64 MB (64 * 1024 * 1024)

    //Creates the page table
    PageTable *page_table = new PageTable(page_size);
    Mmu *mmu = new Mmu(67108864);

    // Prompt loop
    std::string command;
    std::cout << "> ";
    std::getline (std::cin, command);
    while (command != "exit") {
        // Handle command
        // use a vector to hold all the inputs to the command line
    	std::istringstream iss(command);

    	//found at: https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/
    	std::vector<std::string> input (std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());
    
    	/* For bug-fixing purposes:
    	for(std::size_t i = 0; i < input.size(); i ++)
    	{
    		//print the command at input[i]
    		std::cout << input[i];
    	}
		*/ 

    	parseCommandLineInput(input,memory,page_table,mmu);
        // Get next command
        std::cout << "> ";
        std::getline (std::cin, command);
    }


    return 0;
}

void parseCommandLineInput(std::vector<std::string> input, uint8_t *memory, PageTable *page_table, Mmu *mmu)
{
	//takes in the vector of inputs, parses them and does the appropriate action
	//a little clunky, can be changed! 
	if(input[0] == "create")
	{
        int text_size = std::stoi(input[1]);
        int data_size = std::stoi(input[2]);
        int pid = mmu->createProcess();
        mmu->createAllocate(pid, text_size, data_size);

        int text_va = mmu->getVirtualAddress(pid,"<TEXT>");
        int data_va = mmu->getVirtualAddress(pid,"<GLOBALS>");
        int stack_va = mmu->getVirtualAddress(pid,"<STACK>");

        int text_pg = page_table->getPageNumber(text_va);
        int data_pg = page_table->getPageNumber(data_va);
        int stack_pg = page_table->getPageNumber(stack_va);

        page_table->addEntry(pid,text_pg);
        page_table->addEntry(pid,data_pg);
        page_table->addEntry(pid,stack_pg);

        std::cout << pid << std::endl;
	}
	else if(input[0] == "allocate")
	{ 
		//<PID><var_name><data_type><number_of_elements>
		/* allocate memory on the heap, print the virtual memory address	*/ 
		int virutal_mem = mmu->allocate(std::stoi(input[1]),input[2],input[3],std::stoi(input[4]));
		std::cout << "Between mmu allocate and page_num" << std::endl;
		int page_num = page_table->getPageNumber(virutal_mem);

		std::cout << "page num = " << page_num <<std::endl;
		page_table->addEntry(std::stoi(input[1]),page_num);

	}
	else if(input[0] == "set")
	{
		//set <PID><var_name><offset><value_0><value_1><value2>...<valueN>
		/* set the value for variable <var_name> starting at <offset> 
			multiple contiguous vales can be set with one command		*/

		std::cout << "in set" <<std::endl;
		int pid = std::stoi(input[1]);
		int virt_add = 0;
		int phys_add = 0;
		//loop through through the values in input 
		for(int i = 4; i < input.size(); i ++)
		{
			//need to determine the type of the input 
			std::cout << "before virt and phys find "<<std::endl;
			virt_add = mmu->setValues(pid,input[2],std::stoi(input[3]));
			phys_add = page_table->getPhysicalAddress(pid,virt_add);
			
			//need to be able to store all types of values here: 
			std::cout << "Input to set is: " << input[i] <<std::endl;
			memory[phys_add] = std::stoi(input[i]);
		}
	}
	else if(input[0] == "print")
	{
		/*print options: 
			print processes
			print page
			print mmu
			print PID var_name (e.g.)
				*/ 
		if(input[1] == "page")
		{
			page_table->print();
		}

		else if(input[1] == "mmu")
		{
			//print mmu table
            mmu->printMmu();
		}
        else if (input[1] == "processes"){
            mmu->printProcesses();
        }

        /*	check if the input[1] contains digits for a process print*/
        else if (isdigit(input[1][0]))
        {
        	int pid = std::stoi(input[1]);
            //pass pid and name, print value of the variable for that process 
        	int virtual_add = mmu->getVirtualAddress(pid,input[2]);
        	int number_elements = mmu->getNumVariables(pid,input[2]);
        	int physcial_add = page_table->getPhysicalAddress(pid,virtual_add);
            std::string type = mmu->getType(pid, input[2]);

        	int i = 0;
        	while(i < number_elements || i <= 4)
        	{
        		//for comma formatting:
        		if(i == number_elements-1)
        		{
        			if (type == "char"){
                        std::cout << memory[physcial_add+i];
                        break;
                    }
                    else if (type == "short"){
                        std::cout << (short)memory[physcial_add+i];
                        break;
                    }
                    else if (type == "int"){
                        std::cout << (int)memory[physcial_add+i];
                        break;
                    }
                    else if (type == "float"){
                        std::cout << (float)memory[physcial_add+i];
                        break;
                    }
                    else if (type == "long"){
                        std::cout << (long)memory[physcial_add+i];
                        break;
                    }
                    else if (type == "double"){
                        std::cout << (double)memory[physcial_add+i];
                        break;
                    }
        		}
        		else
        		{
        			std::cout << memory[physcial_add+i] << ", ";
                    if (type == "char"){
                        std::cout << memory[physcial_add+i] << ", ";
                    }
                    else if (type == "short"){
                        std::cout << (short)memory[physcial_add+i] << ", ";
                    }
                    else if (type == "int"){
                        std::cout << (int)memory[physcial_add+i] << ", ";
                    }
                    else if (type == "float"){
                        std::cout << (float)memory[physcial_add+i] << ", ";
                    }
                    else if (type == "long"){
                        std::cout << (long)memory[physcial_add+i] << ", ";
                    }
                    else if (type == "double"){
                        std::cout << (double)memory[physcial_add+i] << ", ";
                    }
        		}
        		if(i == 4)
        		{
        			int remainder = number_elements - 4;
        			std::cout << "... [" << remainder << "]";
        		}
                i++;
        	}
        	std::cout << "\n";
        }
	}
	else if(input[0] == "free")
	{
		//free <PID> <var_name>
		/* deallocate memory on the heap that is associated with <var_name>		*/
        int pid = std::stoi(input[1]);
		mmu->free(pid,input[2]);
	}

	else if(input[0] == "terminate")
	{
        int pid = stoi(input[1]);

        mmu->terminate(pid);
	}
}

void printStartMessage(int page_size)
{
    std::cout << "Welcome to the Memory Allocation Simulator! Using a page size of " << page_size << " bytes." << std:: endl;
    std::cout << "Commands:" << std:: endl;
    std::cout << "  * create <text_size> <data_size> (initializes a new process)" << std:: endl;
    std::cout << "  * allocate <PID> <var_name> <data_type> <number_of_elements> (allocated memory on the heap)" << std:: endl;
    std::cout << "  * set <PID> <var_name> <offset> <value_0> <value_1> <value_2> ... <value_N> (set the value for a variable)" << std:: endl;
    std::cout << "  * free <PID> <var_name> (deallocate memory on the heap that is associated with <var_name>)" << std:: endl;
    std::cout << "  * terminate <PID> (kill the specified process)" << std:: endl;
    std::cout << "  * print <object> (prints data)" << std:: endl;
    std::cout << "    * If <object> is \"mmu\", print the MMU memory table" << std:: endl;
    std::cout << "    * if <object> is \"page\", print the page table" << std:: endl;
    std::cout << "    * if <object> is \"processes\", print a list of PIDs for processes that are still running" << std:: endl;
    std::cout << "    * if <object> is a \"<PID>:<var_name>\", print the value of the variable for that process" << std:: endl;
    std::cout << std::endl;
}
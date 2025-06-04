#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <videodriver.h>
#include <idtLoader.h>
#include <sound.h>
#include <colors.h>
#include <homero.h>
#include <mm_manager.h>

// Elegí un lugar libre de la memoria del kernel, por ejemplo:
#define MANAGER_STRUCT_SIZE  sizeof(struct MemoryManagerCDT)
#define MANAGED_MEMORY_SIZE  0x100000  // 1 MiB, ajustá si necesitás más



extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

extern void save_original_regs();

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

typedef int (*EntryPoint)();

void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	clearBSS(&bss, &endOfKernel - &bss);
	return getStackBase();
}


int main()
{	
	load_idt(); //Setup idt before terminal runs

	globalMemoryManager = createMemoryManager(
		(void*)0x800000, // Memory for the memory manager structure
		(void*)0x801000, // Memory for the managed memory
		MANAGED_MEMORY_SIZE
	);




	((EntryPoint)sampleCodeModuleAddress)(); //Calling sampleCodeModule's main address
	
	schedule();


	while (1) {
		printChar('H', 100, 100, (Color){255, 255, 255});  // blanco
	}
	// beep();
	return 0;
}

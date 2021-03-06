#include "pal.h"
#include "pal_regression.h"

int main(int argc, char** argv, char** envp) {
    /* check if the program is loaded */
    pal_printf("User Program Started\n");

    /* check control block */
    /* check arguments */
    pal_printf("# of Arguments: %d\n", argc);
    for (int i = 0; i < argc; i++) {
        pal_printf("argv[%d] = %s\n", i, argv[i]);
    }

    /* unique process ID */
    pal_printf("Process ID: %016lx\n", pal_control.process_id);

    /* parent process */
    pal_printf("Parent Process: %p\n", pal_control.parent_process);

    /* test debug stream */
    char* msg = "Written to Debug Stream\n";
    int ret = DkDebugLog(msg, strlen(msg));
    if (ret < 0) {
        pal_printf("Failed to write the debug message.\n");
        return 1;
    }

    /* Allocation Alignment */
    pal_printf("Allocation Alignment: %ld\n", pal_control.alloc_align);

    /* user address range */
    pal_printf("User Address Range: %p - %p\n", pal_control.user_address.start,
               pal_control.user_address.end);

    if (pal_control.user_address.start && pal_control.user_address.end &&
        pal_control.user_address.start < pal_control.user_address.end)
        pal_printf("User Address Range OK\n");

    pal_printf("CPU num: %ld\n", pal_control.cpu_info.online_logical_cores);
#if defined(__i386__) || defined(__x86_64__)
    pal_printf("CPU vendor: %s\n", pal_control.cpu_info.cpu_vendor);
    pal_printf("CPU brand: %s\n", pal_control.cpu_info.cpu_brand);
    pal_printf("CPU family: %ld\n", pal_control.cpu_info.cpu_family);
    pal_printf("CPU model: %ld\n", pal_control.cpu_info.cpu_model);
    pal_printf("CPU stepping: %ld\n", pal_control.cpu_info.cpu_stepping);
    pal_printf("CPU flags: %s\n", pal_control.cpu_info.cpu_flags);
#elif defined(__powerpc64__)
    pal_printf("CPU cpu: %s\n", pal_control.cpu_info.cpu);
    pal_printf("CPU clock: %s\n", pal_control.cpu_info.clock);
    pal_printf("CPU revision: %s\n", pal_control.cpu_info.revision);
    pal_printf("CPU timebase: %s\n", pal_control.cpu_info.timebase);
    pal_printf("CPU platform: %s\n", pal_control.cpu_info.platform);
    pal_printf("CPU model: %s\n", pal_control.cpu_info.model);
    pal_printf("CPU machine: %s\n", pal_control.cpu_info.machine);
    pal_printf("CPU firmware: %s\n", pal_control.cpu_info.firmware);
    pal_printf("CPU MMU: %s\n", pal_control.cpu_info.mmu);
#else
#error Unsupported architecture
#endif

    return 0;
}

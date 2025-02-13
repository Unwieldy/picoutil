#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// Check if a file exists using stat()
int file_exists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

int flash(char* filename) {
    char command[512];
    // Construct the command
    snprintf(command, sizeof(command), "sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c \"adapter speed 5000\" -c \"program %s verify reset exit\"", filename);

    // Execute the command.
    return system(command);
}

int reset() {
    const char command[] = "openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c \"init; reset; exit\"";
    return system(command);
}

void usage() {
    printf("Usage: picoutil [-r] [-f firmware.uf2]\n");
}

int main(int argc, char *argv[]) {
    int opt;
    char *firmware_file = NULL;
    int reset_flag = 0;
    
    if (argc < 2) {
        usage();
        return 0;
    }

    // Parse command-line options.
    while ((opt = getopt(argc, argv, "f:rh")) != -1) {
        switch (opt) {
            case 'f':
                firmware_file = optarg;
                break;
            case 'r':
                reset_flag = 1;
                break;
            case 'h':
                usage();
                break;
            default:
                usage();
                return 1;
        }
    }

    // Determine what command to execute.
    if (firmware_file) {
        // If the firmware file is provided, verify that it exists.
        if (!file_exists(firmware_file)) {
            fprintf(stderr, "Error: File '%s' not found.\n", firmware_file);
            return 1;
        }
        if (flash(firmware_file)) {
            fprintf(stderr, "Error: Failed to execute flash command.\n");
            return 1;
        }
    }

    if (reset_flag) {
        if(reset()) {
            fprintf(stderr, "Error: Failed to execute reboot command.\n");
            return 1;
        }
    }

    return 0;
}

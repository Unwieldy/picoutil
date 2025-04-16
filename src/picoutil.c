#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int file_exists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

int flash(char* interface_file, char *filename) {
    char command[512];
    // Construct the command
    snprintf(command, sizeof(command),
             "sudo openocd -f %s -f target/rp2040.cfg -c \"adapter speed 5000\" -c \"program %s verify reset exit\"",
             interface_file, filename);

    return system(command);
}

int reset(char* interface_file) {
    char command[512];
    // Construct the command
    snprintf(command, sizeof(command), "openocd -f %s -f target/rp2040.cfg -c \"init; reset; exit\"", interface_file);
    return system(command);
}

void usage() {
    printf("Usage: picoutil [-r] [-f firmware.uf2] [-i interface.cfg]\n");
}

int main(int argc, char *argv[]) {
    int opt;
    char *firmware_file = NULL;
    // Default interface configuration file.
    char *interface_file = "interface/cmsis-dap.cfg";
    int reset_flag = 0;
    
    // Check if there is at least one argument.
    if (argc < 2) {
        usage();
        return 0;
    }

    // Parse command-line options.
    while ((opt = getopt(argc, argv, "f:rhi:")) != -1) {
        switch (opt) {
            case 'f':
                firmware_file = optarg;
                break;
            case 'r':
                reset_flag = 1;
                break;
            case 'i':
                interface_file = optarg;
                break;
            case 'h':
                usage();
                return 0;
            default:
                usage();
                return 1;
        }
    }

    // Determine what command to execute.
    if (firmware_file) {
        // Check that the firmware file exists.
        if (!file_exists(firmware_file)) {
            fprintf(stderr, "Error: File '%s' not found.\n", firmware_file);
            return 1;
        }
        // Check that the interface configuration file exists.
        if (!file_exists(interface_file)) {
            fprintf(stderr, "Error: Interface config file '%s' not found.\n", interface_file);
            return 1;
        }
        if (flash(interface_file, firmware_file)) {
            fprintf(stderr, "Error: Failed to execute flash command.\n");
            return 1;
        }
    }

    if (reset_flag) {
        // Check that the interface configuration file exists.
        if (!file_exists(interface_file)) {
            fprintf(stderr, "Error: Interface config file '%s' not found.\n", interface_file);
            return 1;
        }
        if (reset(interface_file)) {
            fprintf(stderr, "Error: Failed to execute reboot command.\n");
            return 1;
        }
    }

    return 0;
}


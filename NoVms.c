#include <stdio.h>
#include <string.h>






#define LIST_SIZE 4
#define BUFFER_SIZE 1024
int main() {
    // Create a list of strings
    char* list[LIST_SIZE] = { "VMware", "VirtualBox", "QXL", "Virtio" }; //add or take away names to this list. This is not a comprehensive list whatsoever 

    //Run the wmic command with good old popen
    FILE* pipe = _popen("wmic path win32_VideoController get name", "r");
    if (!pipe) {
        printf("Failed to run command\n");
        return 1;
    }
    
    //Make buffer for look
    char buffer[BUFFER_SIZE];
    char output[BUFFER_SIZE] = "";
    //Store the output
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        strcat(output, buffer);
    }
    _pclose(pipe);
    // Check if any of the vendor names are in the output and if so kill the process (: 
    int found = 0;
    for (int i = 0; i < LIST_SIZE; i++) {
        if (strstr(output, list[i]) != NULL) {
            exit(0);
        }
    }
    if (!found) {
        printf("No vendor found in output.\n");
    }

    return 0;
}

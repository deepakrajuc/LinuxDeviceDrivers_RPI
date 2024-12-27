/* user space application to read and write the device file */
/*       GCC command to build the application 
        # gcc -o usr_test user_app.c 
*/


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE_PATH "/dev/new_device"
#define BUFFER_SIZE 256

int main() {
    int fd; // File descriptor for the device
    char read_buffer[BUFFER_SIZE];
    char write_buffer[BUFFER_SIZE];
    int choice;

    // Open the device file
    if (fd < 0) {
        perror("Failed to open the device");
        return EXIT_FAILURE;
    }
    printf("Device opened successfully.\n");

    while (1) {
        // Display menu
        printf("\nChoose an operation:\n");
        printf("1. Write to device\n");
        printf("2. Read from device\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: // Write to the device
                printf("Enter the data to write: ");
                fd = open(DEVICE_PATH, O_RDWR);
                getchar(); // Clear newline left by previous input
                fgets(write_buffer, BUFFER_SIZE, stdin);
                write_buffer[strcspn(write_buffer, "\n")] = '\0'; // Remove trailing newline

                ssize_t bytes_written = write(fd, write_buffer, strlen(write_buffer));
                if (bytes_written < 0) {
                    perror("Failed to write to the device");
                } else {
                    printf("Data written to the device: %s\n", write_buffer);
                }
    		close(fd);
                break;

            case 2: // Read from the device
                memset(read_buffer, 0, BUFFER_SIZE); // Clear the buffer
    		fd = open(DEVICE_PATH, O_RDWR);
                ssize_t bytes_read = read(fd, read_buffer, BUFFER_SIZE - 1);
                if (bytes_read < 0) {
                    perror("Failed to read from the device");
                } else {
                    read_buffer[bytes_read] = '\0'; // Null-terminate the buffer
                    printf("Data read from the device: %s\n", read_buffer);
                }
    		close(fd);
                break;

            case 3: // Exit
                printf("Exiting...\n");
                close(fd); // Close the device
                return EXIT_SUCCESS;

            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    // Close the device
    close(fd);
    return EXIT_SUCCESS;
}

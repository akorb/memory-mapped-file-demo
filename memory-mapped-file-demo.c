#include <sys/mman.h> // for mmap()
#include <fcntl.h>    // for open()
#include <unistd.h>   // for close()
#include <stdio.h>    // for printf()
#include <string.h>   // for memcpy()

#define TEXT_LIMIT 200
#define PAGE_SIZE 4096

int main()
{
    int fd = open("text", O_RDWR);
    char *mapfile = mmap(NULL,                   // We don't care where the page will be allocated
                         PAGE_SIZE,              // The system automatically aligns this to be a multiple of the underlying huge page size.
                         PROT_READ | PROT_WRITE, // We want to read and write
                         MAP_SHARED,             // Store the changes to the mapped page to the actual file
                         fd,                     // The file descriptor of the file
                         0);                     // Read from the beginning of the file

    // After the mmap() call has returned, the file descriptor, fd, can be closed immediately without invalidating the mapping.
    close(fd);

    if (mapfile == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }

    char text[TEXT_LIMIT];
    do
    {
        printf("Do you want to read (r), write (w) or quit (q): ");
        fgets(text, TEXT_LIMIT, stdin);
        switch (text[0])
        {
        case 'w':
            printf("Type the text to write into the file: ");
            fgets(text, TEXT_LIMIT, stdin);
            memcpy(mapfile, text, strlen(text) - 1); // -1 to remove the newline
            // break; // Left on porpuse to print the new content
        case 'r':
            printf("%s\n\n", mapfile);
            break;

        case 'q':
            break;

        default:
            printf("Please write 'r', 'w', or 'q'.\n\n");
            break;
        }
    } while (text[0] != 'q');

    // unallocate the page
    munmap(mapfile, PAGE_SIZE);

    return 0;
}

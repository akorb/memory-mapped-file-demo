#include <sys/mman.h> // for mmap()
#include <fcntl.h>    // for open()
#include <unistd.h>   // for close()
#include <stdio.h>    // for printf()
#include <sys/stat.h> // for fstat()
#include <openssl/evp.h>

// gcc -O3 -Wall -o sha256_mapped -I/opt/ssl/include/ -L/opt/ssl/lib/ -lcrypto sha256_mapped.c


int main(int argc, char **argv)
{
    if (argc == 1)
    {
        puts("Please specify a file.");
        exit(EXIT_FAILURE);
    }

    int fd = open(argv[1], O_RDONLY);

    struct stat fileStats;
    fstat(fd, &fileStats);

    char *mapfile = mmap(NULL,              // We don't care where the page will be allocated
                         fileStats.st_size, // Map the whole file
                         PROT_READ,         // We want only to read
                         MAP_PRIVATE,       // Just for us
                         fd,                // The file descriptor of the file
                         0);                // Read from the beginning of the file

    // After the mmap() call has returned, the file descriptor, fd, can be closed immediately without invalidating the mapping.
    close(fd);

    if (mapfile == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }

    EVP_MD_CTX *context = EVP_MD_CTX_new();
    EVP_DigestInit(context, EVP_sha256());
    EVP_DigestUpdate(context, mapfile, fileStats.st_size);

    unsigned int lengthOfHash;
    unsigned char hash[EVP_MAX_MD_SIZE];
    EVP_DigestFinal(context, hash, &lengthOfHash);

    for (size_t i = 0; i < lengthOfHash; ++i)
        printf("%02x", hash[i]);
    putchar('\n');

    // unallocate the page
    munmap(mapfile, fileStats.st_size);

    return 0;
}

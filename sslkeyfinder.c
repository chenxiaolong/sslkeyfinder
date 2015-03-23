/*
 * Copyright (C) 2015  Andrew Gunnerson <andrewgunnerson@gmail.com>
 *
 * sslkeyfinder is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * sslkeyfinder is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with sslkeyfinder.  If not, see <http://www.gnu.org/licenses/>.
 */

#define _GNU_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: sslkeyfinder [input file]\n");
        return EXIT_FAILURE;
    }

    const char *path = argv[1];

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Failed to open %s: %s\n", path, strerror(errno));
        return EXIT_FAILURE;
    }

    off_t size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    void *map = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) {
        fprintf(stderr, "Failed to mmap %s: %s\n", path, strerror(errno));
        close(fd);
        return EXIT_FAILURE;
    }

    const char *magic = "-----BEGIN RSA PRIVATE KEY-----";
    const size_t magic_size = strlen(magic);
    const size_t search_len = 1750;

    void *found;
    void *begin = map;

    while ((found = memmem(begin, size - (begin - map),
                           magic, magic_size))) {
        // Found magic, print it out
        printf("\n\n\n\n\n");

        size_t offset = found - map;
        size_t print_len;

        if (offset + search_len > size) {
            // Less than search_len bytes left in the file
            print_len = size - offset;
        } else {
            // Can print the full search_len bytes
            print_len = search_len;
        }

        fwrite(found, 1, print_len, stdout);

        // Start next search after the found item
        begin = found + magic_size;
    }

    munmap(map, size);
    close(fd);

    return EXIT_SUCCESS;
}
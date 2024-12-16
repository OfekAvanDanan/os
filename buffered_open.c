// Azriel Erenkrantz - 315336719 | Ofek Avan Danan - 211824727
#include "buffered_open.h"
#include <stdlib.h>
#include<stdio.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>

buffered_file_t *globalBuffer; //for sigint signal handle
buffered_file_t *buffered_open(const char *pathname, int flags, ...) {
    int mode = 0644; // Default mode for creating files if O_CREAT is used
    va_list args;

    // Allocate memory for the buffered file structure
    buffered_file_t *bf = (buffered_file_t*)calloc(1, sizeof(buffered_file_t));
    if (bf == NULL) {
        perror("Failed to allocate memory for buffered file");
        return NULL;
    }

    // Remove O_PREAPPEND from flags (it should not interfere with open)
    if (flags & O_PREAPPEND) {
        bf->preappend = 1;
    }
    flags &= ~O_PREAPPEND;

    // Store the flags
    bf->flags = flags;

    // Check if O_CREAT is part of the flags
    if (flags & O_CREAT) {
        // If O_CREAT is set, we need to retrieve the mode 
        va_start(args, flags);
        mode = va_arg(args, int);
        va_end(args);
    }

    // Open the file with the flags and mode (if applicable)
    bf->fd = open(pathname, flags, mode);
    if (bf->fd == -1) {
        perror("Failed to open file");
        free(bf);
        return NULL;
    }

    // Allocate memory for read and write buffers
    bf->read_buffer = (char*)malloc(BUFFER_SIZE);
    bf->write_buffer = (char*)malloc(BUFFER_SIZE);
    if (bf->read_buffer == NULL || bf->write_buffer == NULL) {
        perror("failed to allocate buffers");
        close(bf->fd);
        free(bf->read_buffer);
        free(bf->write_buffer);
        free(bf);
        return NULL;
    }

    // Set buffer sizes and positions
    bf->read_buffer_size = 0;
    bf->write_buffer_size = 0;
    bf->read_buffer_pos = 0;
    bf->write_buffer_pos = 0;

     globalBuffer = bf; //set the global buffer to point on the current exsist buffer
    //close the program properly
    if (atexit(atexit_care) != 0) {
        perror("failed to register atexit");
        return NULL;
    }
    if (signal(SIGINT,sigint_care) == SIG_ERR) {
        perror("failed to register signal care");
        return NULL;
    }

    // Return the buffered file structure
    return bf;
}


ssize_t buffered_write(buffered_file_t *bf, const void *buf, size_t count) {
    int written = 0;
    int buffer_space = 0;
    int savebufferpos = bf->write_buffer_pos;
    void* temporaryBuffer = calloc(1, BUFFER_SIZE);
    if (!temporaryBuffer) {
        perror("memory allocation failed");
        return -1;
    }
    //if preappend flag is on -save the buffer data first
    if (bf->preappend == 1) {
     memcpy(temporaryBuffer,bf->write_buffer,bf->write_buffer_pos);
        bf->write_buffer_pos = 0;
    }
    while (count > written) {
        buffer_space = BUFFER_SIZE - bf->write_buffer_pos;
        //check buffer space is enough
        if (buffer_space == 0 ) {
            buffered_flush(bf);
            buffer_space = BUFFER_SIZE;
            }
        //copy from given buf to write_buffer
      int sizeToCopy = (count - written < buffer_space)?count - written:buffer_space;
        memcpy(bf->write_buffer + bf->write_buffer_pos, buf, sizeToCopy);
        bf->write_buffer_pos += sizeToCopy;
        written += sizeToCopy;
    }
    //restore buffer data - for preappend flag
    if (bf->preappend == 1) {
        int written = 0;
        while (savebufferpos > written) {
            buffer_space = BUFFER_SIZE - bf->write_buffer_pos;
            if (buffer_space == 0 ) {
                buffered_flush(bf);
                buffer_space = BUFFER_SIZE;
            }
            int sizeToCopy = (savebufferpos - written < buffer_space)?savebufferpos - written:buffer_space;
            memcpy(bf->write_buffer + bf->write_buffer_pos, temporaryBuffer, savebufferpos);
            bf->write_buffer_pos += sizeToCopy;
            written += sizeToCopy;
        }
    }
    return written;
}

ssize_t buffered_read(buffered_file_t *bf, void *buf, size_t count) {
    //flush the buffer - before reading
    if (bf->preappend) {
        buffered_flush(bf);
    }
    //start reading
    int  actuallyRead = 0;
    int aviliableToRead = 0;

    while(actuallyRead < count) {
        if (bf->read_buffer_pos >= bf->read_buffer_size || bf->read_buffer_size == 0) {
            int readFromFile = read(bf->fd,bf->read_buffer, BUFFER_SIZE);
            if (readFromFile < 0) {
                return -1;  // Error
            }
            if(readFromFile == 0) { //end od file
                break;
            }
            bf->read_buffer_size = readFromFile;
            bf->read_buffer_pos = 0;
        }
        aviliableToRead = bf->read_buffer_size - bf->read_buffer_pos;
        int toCopy =(count - actuallyRead < aviliableToRead)?count - actuallyRead:aviliableToRead;
        memcpy(buf + actuallyRead, bf->read_buffer + bf->read_buffer_pos ,toCopy);
        bf->read_buffer_pos += toCopy;
        actuallyRead += toCopy;
    }
    return actuallyRead;
}

int buffered_flush(buffered_file_t *bf) {
    if (bf->write_buffer_pos == 0) {
        return 0;
    }
    int written = 0;
    int towrite = bf->write_buffer_pos;

    // claculate file size
    int filesize = lseek(bf->fd, 0, SEEK_END);
    if (filesize < 0) {
        perror("Error getting file size");
        return -1;
    }
    //allocate temporary buffer
    void* temporaryBuffer = calloc(1, filesize);
    if (!temporaryBuffer) {
        perror("memory allocation failed");
        return -1;
    }

    // if O_PREAPPEND is on -save the current file content
    if (bf->preappend) {
        if (lseek(bf->fd, 0, SEEK_SET) < 0) {
            perror("error seeking to start of file");
            free(temporaryBuffer);
            return -1;
        }
        if (read(bf->fd, temporaryBuffer, filesize) < 0) {
            perror("error reading file");
            free(temporaryBuffer);
            return -1;
        }
        //seek to the start of the file
        lseek(bf->fd, 0, SEEK_SET);
    }

    // operate the flush
    while (written < towrite) {
        int bytesflush = write(bf->fd, bf->write_buffer + written, towrite - written);
        if (bytesflush < 0) {
            perror("error writing to file");
            free(temporaryBuffer);
            return -1;
        }
        written += bytesflush;
    }
    bf->write_buffer_pos = 0;

    // write back the saved content
    if (bf->preappend) {
        if (lseek(bf->fd, written, SEEK_SET) < 0) {
            perror("Error seeking to position");
            free(temporaryBuffer);
            return -1;
        }
        if (write(bf->fd, temporaryBuffer, filesize) < 0) {
            perror("Error writing saved content");
            free(temporaryBuffer);
            return -1;
        }
    }

    free(temporaryBuffer);
    return written;
}
void sigint_care(int sigint) {
    buffered_close(globalBuffer);
    exit(0);
}
void atexit_care() {
    buffered_close(globalBuffer);
}



         int buffered_close(buffered_file_t *bf) {
          if(bf!=NULL) {
              buffered_flush(bf);
              close(bf->fd);
              free(bf->write_buffer);
              free(bf->read_buffer);
              free(bf);
          }
    return 0;
}








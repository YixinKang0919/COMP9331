/** 
 * COMP3331/9331 Computer Networks and Applications
 * Programming Tutorial - Online Banking System
 * Written by Tim Arney <t.arney@unsw.edu.au>
 * 
 * This file contains the server implementation for the online banking system.
 * The server listens for incoming requests from clients, processes the requests,
 * and sends a response back to the client.
 * 
 * The server is implemented using a UDP socket, and the server logic is executed
 * in a separate thread for each request.  This allows the server to handle
 * multiple clients concurrently.
 * 
 * The server uses a Bank ADT to manage the account information and process
 * the requests.  The Bank ADT is responsible for opening accounts, checking
 * balances, and transferring funds between accounts.  It is initialised with the
 * account information provided in a file.  The file should contain one account
 * per line, with the account name, hashed password, and balance separated by
 * whitespace.
 * 
 * Compilation: make server
 *   or simply: make
 * Usage: ./server <accounts_file> <server_port>
 * Example: ./server accounts.tsv 54321
 *
 * Please refer to Beej's Guide to Network Programming for the best introduction
 * to using sockets in C: https://beej.us/guide/bgnet/
 */

#include <arpa/inet.h>
#include <errno.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "bank.h"

/* We'll set a limit of 1024 bytes for all requests/responses. */
#define BUFFER_SIZE 1024

/* Rate limit for responses in milliseconds. */
#define RATE_LIMIT 10

/* A request struct to store the data and client address for each request. 
   Once the server is multi-threaded we'll spawn a thread for each request (not 
   a great design, creating a thread pool would be better, but for our learning 
   purposes this is fine), and we'll store the ID of the thread that's been 
   spawned for the request. */
typedef struct {
    pthread_t thread_id;
    char data[BUFFER_SIZE];
    struct sockaddr_in client_address;
    socklen_t client_address_len;
} request_t;

/* The main server "control block", with hooks to the bank and the socket, which
   are shared by all threads. */
static struct {
    bank_ptr bank;
    int sock;
    pthread_attr_t attr;  // thread attributes, e.g., detached, only stored here for cleanup
    request_t *request;   // pointer to next awaited request, only stored here for cleanup
} server = {0};

/* TODO 4: Implement this function for our single threaded server. */
static void process_request(request_t *request);

/* TODO 6: Revise the `process_request` function prototype so it can be used with 
     multi-threading. */


/* Helper functions */
void cleanup(void);
void handle_sigint(int sig);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <accounts_file> <server_port>\n", argv[0]);
    }
    
    char *accounts_file = argv[1];
    uint16_t server_port = (uint16_t)strtol(argv[2], NULL, 10);

    server.bank = bank_init(accounts_file);
    if (server.bank == NULL) {
        fprintf(stderr, "Failed to initialise bank\n");
        return EXIT_FAILURE;
    }

    /* TODO 1.1: Create a `struct sockaddr_in`, which we'll use to store the 
         server's address.  Zero out the memory and set the socket family to 
         `AF_INET`, i.e. IPv4. */
    
    
    /* TODO 1.2: Convert the `server_port` from host to network byte order and 
         assign it to the struct's `sin_port`. */
    
    
    /* TODO 1.3: Convert the `INADDR_LOOPBACK` constant from host to network byte 
         order and assign it to the struct's `s_addr`. */
    
    
    /* TODO 1.4: Create the UDP socket with family `AF_INET` and type 
         `SOCK_DGRAM`. */
    
    
    /* TODO 1.5: Bind the server socket to the server address. */
    
    
    printf("Server running on port %hu...\n", server_port);
    printf("Press Ctrl+C to exit.\n");

    /* TODO 6.1: Initialise the server's thread attributes and set it to a 
         detached state, so threads will automatically clean up once they 
         finish and don't need to join with the main thread. */
    

    // Register the cleanup function to be called at normal program termination.
    if (atexit(cleanup) != 0) {
        fprintf(stderr, "Unable to set exit function.\n");
        return EXIT_FAILURE;
    }

    // Set up the signal handler for SIGINT (Ctrl-C).
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        fprintf(stderr, "Unable to set signal handler.\n");
        return 1;
    }

    // Loop forever (or until Ctrl-C) to receive incoming requests.
    while (true) {
        // Allocate memory for a request.
        server.request = malloc(sizeof(*server.request));
        if (server.request == NULL) {
            fprintf(stderr, "Failed to allocate request struct\n");
            return EXIT_FAILURE;
        }

        /* TODO 2.1: Set the `client_address_len` in the request block 
             to the size of the `struct sockaddr_in` that we'll read the 
             client's address into. */
        
        
        /* TODO 2.2: Wait to receive a request and make sure we capture the 
             length of the received data. */
        

        /* TODO 2.3: Add a null byte to the buffer at the end of the received 
             data so we can treat it like a string. */
        

        /************************** single threaded ***************************/
        /* TODO 3: Call `process_request`, passing it the request. */


        /************************** multi threaded ***************************/
        /* TODO 6: Modify the `process_request` prototype to be used directly 
             with pthread_create, and replace the above call with one that spawns
             a new thread.  Make sure to pass it the thread attributes that will 
             put it in a detached state. */
        
        /************************ end multi threaded **************************/

        // Once we've spawned the thread or processed the request, we can forget 
        // about the request.
        server.request = NULL;
    }

    return EXIT_SUCCESS;
}

/* TODO 6: Modify this function definition so it can be used with multi-threading. */
/** 
 * @brief The main server logic, which processes the incoming request and sends  
 *        the response back to the client.
 *
 * @param request The request to process, which includes the request itself and
 *                client address.  The request will be freed after processing.
 */
static void process_request(request_t *request) {   // single threaded
    /* Comment this line when single-threading, uncomment when multi-threading. */
    // request_t *request = (request_t *)arg;

    // Parse the request data.  All very unsafe, but we're keeping it simple.
    char *operation = NULL;
    char *account = NULL;
    char *password_hash = NULL;
    char *to_account = NULL;
    float amount = 0.0;

    operation = request->data;
    char *pos = operation;
    
    while (*pos != '\n' && *pos != '\0') pos++;
    *pos = '\0';
    pos++;
    account = pos;

    while (*pos != '\n' && *pos != '\0') pos++;
    if (*pos == '\0') {
        *pos = '\0';
        pos++;
        password_hash = pos;
    } else {
        *pos = '\0';
        pos++;
        password_hash = pos;

        while (*pos != '\n' && *pos != '\0') pos++;
        *pos = '\0';
        pos++;
        to_account = pos;

        while (*pos != '\n' && *pos != '\0') pos++;
        *pos = '\0';
        pos++;
        amount = strtof(pos, NULL);
    }

    // Create a buffer to store the response.
    char response[BUFFER_SIZE];
    int response_len = BUFFER_SIZE;
    
    /* TODO 4.3: Call the appropriate bank function, based on the operation. */
    

    // Extract the client's address and port from the request.
    char client_host[NI_MAXHOST];
    char client_port[NI_MAXSERV];

    getnameinfo((struct sockaddr *) &request->client_address,
                        request->client_address_len, client_host, NI_MAXHOST,
                        client_port, NI_MAXSERV, NI_NUMERICSERV);

    /* TODO 5: Generate a timestamp and log the request/response. */
    

    /* TODO 7: Sleep fpr RATE_LIMIT milliseconds delay. */
    

    /* TODO 4.4: Send the response back to the client. */
    

    // Free the request memory.
    free(request);
    request = NULL;

    /* Comment these 2 lines when single-threading, uncomment when multi-threading. */
    // pthread_exit(NULL);
    // return NULL;
}

/* The cleanup function to be called at normal program termination. */
void cleanup(void) {
    sleep(2);   // wait for any remaining threads to finish
    pthread_attr_destroy(&server.attr); // destroy the thread attributes
    close(server.sock); // close the socket
    bank_rupt(server.bank); // free the bank memory
    if (server.request != NULL) {
        free(server.request);   // free any remaining request memory
        server.request = NULL;
    }
}

/* The signal handler for SIGINT (Ctrl-C). */
void handle_sigint(int sig) {
    printf("\nRunning cleanup...\n");
    exit(EXIT_SUCCESS); // This will trigger the cleanup function
}

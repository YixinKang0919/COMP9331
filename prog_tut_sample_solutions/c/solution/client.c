/**
 * COMP3331/9331 Computer Networks and Applications
 * Programming Tutorial - Online Banking System
 * Written by Tim Arney <t.arney@unsw.edu.au>
 * 
 * This file contains the client implementation for the online banking system.
 * The client sends requests to the server to open accounts, check balances, and
 * transfer funds between accounts.
 * 
 * The client is implemented using a UDP socket.  The client reads the command line
 * arguments to determine the server port and the operation to perform.  The client
 * then sends a request to the server, displays the response, and terminates.  So 
 * the client is more like a command line tool than a traditional client.
 * 
 * Compilation: make client
 *   or simply: make
 * 
 * Usage: ./client server_port {open,balance,transfer,crack} ... 
 * 
 * The client supports the following operations:
 * 
 * - open: Open a new account with the provided account name and password.
 * 
 *   Usage: ./client <server_port> open <account> <password>
 *   Example: ./client 54321 open alice password
 * 
 * - balance: Check the balance of the account with the provided account name and
 *   password.
 * 
 *   Usage: ./client <server_port> balance <account> <password>
 *   Example: ./client 54321 balance alice password
 * 
 * - transfer: Transfer funds from one account to another account.
 * 
 *   Usage: ./client <server_port> transfer <from_account> <from_password> <to_account> <amount>
 *   Example: ./client 54321 transfer alice password bob 100.0
 * 
 * - crack: Crack the password of the account using a wordlist file.
 * 
 *   Usage: ./client <server_port> crack <account> <wordlist>
 *   Example: ./client 54321 crack alice wordlist.txt
 * 
 * The server is expected to be running on the same machine as the client, and the
 * server should be started before the client is run.
 */

#include <netinet/in.h>
#include <openssl/sha.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

/* We'll set a limit of 1024 bytes for all requests/responses. */
#define BUFFER_SIZE 1024

/* Functions to implement */
static void open_account(uint16_t server_port, const char *account, const char *password_hash);
static void check_balance(uint16_t server_port, const char *account, const char *password_hash);
static void transfer_funds(uint16_t server_port, const char *account, const char *password_hash, const char *to_account, float amount);
static void send_request(uint16_t server_port, const char *request, size_t request_len, char *response, size_t *response_len);
static void crack_account(uint16_t server_port, const char *account, const char *wordlist);

/* Helper functions */
static void hash_password(const char *password, char *password_hash);

int main(int argc, char *argv[]) {
    /* You can basically ignore the main function, it's just a lot of argument 
       parsing then calling the appropriate function based on the operation. */
    if (!(argc == 5 || argc == 7)) {
        fprintf(stderr, "Usage: %s <server_port> <operation> <account> <password|wordlist> [<to_account> <amount>]\n", argv[0]);
        return EXIT_FAILURE;
    }

    uint16_t server_port = (uint16_t)strtol(argv[1], NULL, 10);
    char *operation = argv[2];
    char *account = argv[3];
    char *password = argv[4];
    char password_hash[SHA_DIGEST_LENGTH * 2 + 1];

    hash_password(password, password_hash);

    if (strcmp(operation, "open") == 0) {
        open_account(server_port, account, password_hash);
    } else if (strcmp(operation, "balance") == 0) {
        check_balance(server_port, account, password_hash);
    } else if (strcmp(operation, "transfer") == 0) {
        if (argc != 7) {
            fprintf(stderr, "Invalid number of arguments for transfer operation.\n");
            return EXIT_FAILURE;
        }
        char *to_account = argv[5];
        float amount = strtof(argv[6], NULL);
        transfer_funds(server_port, account, password_hash, to_account, amount);
    } else if (strcmp(operation, "crack") == 0) {
        char *wordlist = argv[4];
        crack_account(server_port, account, wordlist);
    }else {
        fprintf(stderr, "Invalid operation: %s\n", operation);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/**
 * @brief Contacts the server to open a new account with the provided account 
 *        name and password.
 *
 * @param server_port    The UDP port number of the server.
 * @param account        The name of the account to open.
 * @param password_hash  The hashed password for the account.
 */
static void open_account(uint16_t server_port, const char *account, const char *password_hash) {
    /* TODO: Create buffers of size `BUFFER_SIZE` to store the request and the 
         response, and a variable for the length of the response, initialised 
         to the size of the response buffer. */
    char request[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    size_t response_len = BUFFER_SIZE;

    /* TODO: Construct an "open" account message in the request buffer and 
         record the length of the request. */
    int request_len = snprintf(request, BUFFER_SIZE, "open\n%s\n%s", account, password_hash);
    if (request_len < 0 || request_len >= BUFFER_SIZE) {
        fprintf(stderr, "Failed to create request\n");
        return;
    }

    /* TODO: Call the `send_request` function, and print the response that 
         comes back. */
    send_request(server_port, request, (size_t)request_len, response, &response_len);
    if (response_len > 0) {
        printf("%s\n", response);
    }
}

/**
 * @brief Contacts the server to check_balance the balance of the account with 
 *        the provided account name and password hash.
 *
 * @param server_port    The UDP port number of the server.
 * @param account        The name of the account to get the balance of.
 * @param password_hash  The hashed password for the account.
 */
static void check_balance(uint16_t server_port, const char *account, const char *password_hash) {
    /* TODO: As per `open_account`, but now with an account "balance" message. */
    char request[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    size_t response_len = BUFFER_SIZE;

    int request_len = snprintf(request, BUFFER_SIZE, "balance\n%s\n%s", account, password_hash);
    if (request_len < 0 || request_len >= BUFFER_SIZE) {
        fprintf(stderr, "Failed to create request\n");
        return;
    }

    send_request(server_port, request, (size_t)request_len, response, &response_len);
    if (response_len > 0) {
        printf("%s\n", response);
    }
}

/**
 * @brief Contacts the server to transfer funds from one account to another.
 *
 * @param server_port    The UDP port number of the server.
 * @param account        The name of the account to transfer funds from.
 * @param password_hash  The hashed password for the account.
 * @param to_account     The name of the account to transfer funds to.
 * @param amount         The amount of funds to transfer.
 */
static void transfer_funds(uint16_t server_port, const char *account, const char *password_hash, const char *to_account, float amount) {
    /* TODO: As per `open_account`, but now with a "transfer" funds message. */
    char request[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    size_t response_len = BUFFER_SIZE;

    int request_len = snprintf(request, BUFFER_SIZE, "transfer\n%s\n%s\n%s\n%.2f", account, password_hash, to_account, amount);
    if (request_len < 0 || request_len >= BUFFER_SIZE) {
        fprintf(stderr, "Failed to create request\n");
        return;
    }

    send_request(server_port, request, (size_t)request_len, response, &response_len);
    if (response_len > 0) {
        printf("%s\n", response);
    }
}

/**
 * @brief Send a request to the server and wait for a response.
 * 
 * @param server_port  The port number of the server.
 * @param request      A buffer containing the request.
 * @param request_len  The length of the request in the buffer.
 * @param response     A buffer to store the response in.
 * @param response_len The length of the response buffer, which will updated 
                       to the length of the actual response.
 */
static void send_request(uint16_t server_port, const char *request, size_t request_len, char *response, size_t *response_len) {
    /* TODO: Create a `struct sockaddr_in`, which we'll use to store the server's 
         address.  Zero out the memory and set the socket family to `AF_INET`, 
         i.e. IPv4. */
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;

    /* TODO: Convert the `server_port` from host to network byte order and 
         assign it to the struct's `sin_port`. */
    server_address.sin_port = htons(server_port);
    
    /* TODO: Convert the `INADDR_LOOPBACK` constant from host to network byte 
         order and assign it to the struct's `s_addr`. */
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    
    /* TODO: Create the UDP socket with family `AF_INET` and type `SOCK_DGRAM`. */
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return;
    }

    /* TODO: Send the request through the socket to the server. */
    int send_len = sendto(sock, request, request_len, 0,(struct sockaddr*)&server_address, sizeof(server_address));
    if (send_len < 0) {
        perror("sendto");
        return;
    }

    /* TODO: Create another `struct sockaddr_in` to store address of host that 
         sends the response.  Also create a `socklen_t` to store the size of 
         the struct. */
    struct sockaddr_in response_address;
    socklen_t response_address_len = sizeof(response_address);

    /* TODO: Wait to receive a response and make sure we capture the length of 
         the received data.
         Question: is it sensible for this call to block indefinitely? */
    int recv_len = recvfrom(sock, response, *response_len, 0, (struct sockaddr*)&response_address, &response_address_len);
    if (recv_len < 0) {
        perror("recvfrom");
        return;
    }

    /* TODO: Add a null byte to the buffer at the end of the received data so 
         we can treat it like a string, and update the `response_len` for the 
         calling function. */
    response[recv_len] = '\0';
    *response_len = (size_t)recv_len;

    /* TODO: Close the socket. */
    close(sock);
}

/**
 * @brief Crack the password of an account using a wordlist.
 * 
 * @param server_port The port number of the server.
 * @param account     The name of the account to crack.
 * @param wordlist    The path to the wordlist file.
 */
static void crack_account(uint16_t server_port, const char *account, const char *wordlist) {
    FILE *file = fopen(wordlist, "r");
    if (file == NULL) {
        perror("fopen");
        return;
    }

    /* TODO: Create the usual request and response buffers, as well as a 
         buffer to store the current password we're testing. */
    char request[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    char password[BUFFER_SIZE];

    /* TODO: Complete the same socket setup as in `send_request`.  We don't want 
         to use `send_request` directly as it will create a new socket for each 
         password, which will be costly.  In general, the client code could 
         benefit from some refactoring, but that can be left as an exercise. */
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return;
    }

    printf("Account: %s\n", account);
    bool cracked = false;

    while (fgets(password, sizeof(password), file) != NULL) {
        password[strcspn(password, "\n")] = '\0'; // Remove the newline character
        printf("\rPassword: %s", password);
        fflush(stdout);

        char password_hash[SHA_DIGEST_LENGTH * 2 + 1];
        hash_password(password, password_hash);

        /* TODO: Construct a "balance" request message. */
        int request_len = snprintf(request, BUFFER_SIZE, "balance\n%s\n%s", account, password_hash);
        if (request_len < 0 || request_len >= BUFFER_SIZE) {
            fprintf(stderr, "Failed to create request\n");
            return;
        }

        /* TODO: Send the request. */
        int send_len = sendto(sock, request, request_len, 0,(struct sockaddr*)&server_address, sizeof(server_address));
        if (send_len < 0) {
            perror("sendto");
            return;
        }

        /* TODO: Create a struct to store the address of the host that sends 
             the response. */
        struct sockaddr_in response_address;
        size_t response_len = sizeof(response);
        socklen_t response_address_len = sizeof(response_address);

        /* TODO: Receive the response. */
        int recv_len = recvfrom(sock, response, response_len, 0, (struct sockaddr*)&response_address, &response_address_len);
        if (recv_len < 0) {
            perror("recvfrom");
            return;
        }

        /* TODO: Add a null-byte after the received data. */
        response[recv_len] = '\0';

        /* TODO: Check if the response is "not authorised".  If it is, we 
             move onto the next password, otherwise we've found the password 
             and can print out the balance and break the loop. */
        if (strcmp(response, "not authorised") != 0) {
            printf("\nBalance: %s\n", response);
            cracked = true;
            break;
        }

        // Some ugly code to clear the previous password from the screen.
        printf("\r");
        for (int i = 0; i < 2 * strlen(password); i++) {
            printf(" ");
        }
    }

    if (!cracked) {
        printf("\nPassword not found in wordlist: %s\n", wordlist);
    }

    /* TODO: Close the socket. */
    close(sock);

    fclose(file);
}


/**
 * @brief Hash a password using SHA1 into hex-string format.
 * 
 * @param password       The password to hash.
 * @param password_hash  A buffer to store the hashed password.
 */
static void hash_password(const char *password, char *password_hash) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1((unsigned char *)password, strlen(password), hash);

    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        snprintf(password_hash + (i * 2), 3, "%02x", hash[i]);
    }

    password_hash[SHA_DIGEST_LENGTH * 2] = '\0'; // null-terminate the string
}

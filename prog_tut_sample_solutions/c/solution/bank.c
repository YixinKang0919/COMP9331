/**
 * COMP3331/9331 Computer Networks and Applications
 * Programming Tutorial - Online Banking System
 * Written by Tim Arney <t.arney@unsw.edu.au>
 * 
 * This file contains the bank implementation for the online banking system.
 */

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bank.h"

/* `struct bank` is a simple singly-linked list of accounts */
struct bank {
    struct account *head;
    /* TODO 8.1: add a lock to the bank to prevent race conditions */
    pthread_mutex_t lock;
};

/* `struct account` is a node in the singly-linked list of accounts */
typedef struct account {
    char *account;
    char *password_hash;
    float balance;
    struct account *next;
} account_t;

/* Helper functions */
static account_t *make_account(const char *account, const char *password_hash);
static void free_account(account_t *account);
static account_t *find_account(const bank_ptr b, const char *account);
static bool is_authorised(const account_t *account, const char *password_hash);
static bool has_sufficient_funds(const account_t *account, float amount);
static void update_balance(account_t *account, float amount);

/* Response messages */
static const char success[] = "successful";
static const char not_authorised[] = "not authorised";
static const char insufficient_funds[] = "insufficient funds";
static const char account_already_exists[] = "account already exists";

bank_ptr bank_init(const char *accounts_file) {
    bank_ptr b = malloc(sizeof(*b));
    
    if (b == NULL) {
        return NULL;
    }
    
    b->head = NULL;

    /* TODO 8.1: initialise the bank's lock */
    pthread_mutex_init(&b->lock, NULL);

    if (accounts_file == NULL) {
        return b;
    }

    FILE *file = fopen(accounts_file, "r");

    if (file == NULL) {
        bank_rupt(b);
        return NULL;
    }

    char input_buffer[BUFSIZ];

    while (fgets(input_buffer, BUFSIZ, file) != NULL) {
        const char *account = strtok(input_buffer, " \t\n");
        const char *password_hash = strtok(NULL, " \t\n");
        float balance = strtof(strtok(NULL, " \t\n"), NULL);

        account_t *new_account = make_account(account, password_hash);
        new_account->balance = balance;
        new_account->next = b->head;
        b->head = new_account;
    }

    fclose(file);
    return b;
}

int bank_open_account(bank_ptr b, const char *account, const char *password_hash, char *response_buf, size_t buf_size) {
    if (find_account(b, account) != NULL) {
        strcpy(response_buf, account_already_exists);
        return strlen(response_buf);
    }

    account_t *new_account = make_account(account, password_hash);
    new_account->next = b->head;
    b->head = new_account;

    strncpy(response_buf, success, buf_size);
    response_buf[buf_size - 1] = '\0';
    return strlen(response_buf);
}

int bank_get_balance(bank_ptr b, const char *account, const char *password_hash, char *response_buf, size_t buf_size) {
    account_t *account_to_check = find_account(b, account);

    if (!account_to_check || !is_authorised(account_to_check, password_hash)) {
        strncpy(response_buf, not_authorised, buf_size - 1);
        response_buf[buf_size - 1] = '\0';
        return strlen(response_buf);
    }

    int r = snprintf(response_buf, buf_size, "%.2f", account_to_check->balance);
    return r;
}

int bank_transfer_funds(const bank_ptr b, const char *from_account, const char *password_hash, const char *to_account, float amount, char *response_buf, size_t buf_size) {
    account_t *from = find_account(b, from_account);
    account_t *to = find_account(b, to_account);

    if (!from || !to || !is_authorised(from, password_hash)) {
        strncpy(response_buf, not_authorised, buf_size - 1);
        response_buf[buf_size - 1] = '\0';
        return strlen(response_buf);
    }

    /* TODO 8.2: critical code section, ensure only one thread can transfer 
         funds at a time.  Note, this bank ADT actually has many critical code 
         sections that could result in race conditions, but you can resolve 
         those another time. */
    pthread_mutex_lock(&b->lock);
    if (!has_sufficient_funds(from, amount)) {
        pthread_mutex_unlock(&b->lock);
        strncpy(response_buf, insufficient_funds, buf_size - 1);
        response_buf[buf_size - 1] = '\0';
        return strlen(response_buf);
    }

    update_balance(from, -amount);
    update_balance(to, amount);
    pthread_mutex_unlock(&b->lock);
    /* end of critical section */

    strncpy(response_buf, success, buf_size);
    response_buf[buf_size - 1] = '\0';
    return strlen(response_buf);
}

void bank_rupt(bank_ptr b) {
    account_t *current = b->head;
    while (current != NULL) {
        account_t *next = current->next;
        free_account(current);
        current = next;
    }
    free(b);
}

/***************************** HELPER FUNCTIONS *******************************/

static account_t *find_account(const bank_ptr b, const char *account) {
    account_t *current = b->head;

    while (current != NULL) {
        if (strcmp(current->account, account) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

static bool is_authorised(const account_t *account, const char *password_hash) {
    return strcmp(account->password_hash, password_hash) == 0;
}

static bool has_sufficient_funds(const account_t *account, float amount) {
    return account->balance >= amount;
}

static void update_balance(account_t *account, float amount) {
    account->balance += amount;
}

static account_t *make_account(const char *account, const char *password_hash) {
    account_t *new_account = malloc(sizeof(struct account));
    new_account->account = strdup(account);
    new_account->password_hash = strdup(password_hash);

    if (new_account->account == NULL || new_account->password_hash == NULL) {
        free_account(new_account);
        return NULL;
    }

    new_account->balance = 0;
    new_account->next = NULL;
    return new_account;
}

static void free_account(account_t *account) {
    free(account->account);
    free(account->password_hash);
    free(account);
}

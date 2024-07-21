/**
 * COMP3331/9331 Computer Networks and Applications
 * Programming Tutorial - Online Banking System
 * Written by Tim Arney <t.arney@unsw.edu.au>
 * 
 * This file contains the bank interface for the online banking system.
 */

#pragma once

#include <stdlib.h>

/**
 * @brief A pointer to a bank.  This is an opaque type, and should not be 
 *        accessed directly.
 */
typedef struct bank *bank_ptr;

/**
 * @brief Initialise a new bank.
 *
 * @param accounts_file The file containing the accounts to initialise the bank 
 *                      with.  If NULL, the bank will be initialised with no 
 *                      accounts.
 * @return A pointer to the new bank, or NULL if the bank could not be 
 *         initialised.
 */
bank_ptr bank_init(const char *accounts_file);

/**
 * @brief Open a new account in the bank.
 *
 * @param b The bank to open the account in.
 * @param account The name of the account to open.
 * @param password_hash The password hash for the account.
 * @param response_buf A buffer to store the response message in.
 * @param buf_size The size of the response buffer.
 * @return The length of the response message.
 */
int bank_open_account(bank_ptr b, const char *account, const char *password_hash, char *response_buf, size_t buf_size);

/**
 * @brief Get the balance of an account.
 *
 * @param b The bank to get the balance from.
 * @param account The name of the account to get the balance of.
 * @param password_hash The password hash for the account.
 * @param response_buf A buffer to store the response message in.
 * @param buf_size The size of the response buffer.
 * @return The length of the response message.
 */
int bank_get_balance(bank_ptr b, const char *account, const char *password_hash, char *response_buf, size_t buf_size);

/**
 * @brief Transfer funds between two accounts.
 *
 * @param b The bank to transfer the funds in.
 * @param from_account The name of the account to transfer the funds from.
 * @param password_hash The password hash for the account to transfer the funds 
 *                      from.
 * @param to_account The name of the account to transfer the funds to.
 * @param amount The amount of funds to transfer.
 * @param response_buf A buffer to store the response message in.
 * @param buf_size The size of the response buffer.
 * @return The length of the response message.
 */
int bank_transfer_funds(bank_ptr b, const char *from_account, const char *password_hash, const char *to_account, float amount, char *response_buf, size_t buf_size);

/**
 * @brief Free the memory used by a bank.
 *
 * @param b The bank to free.
 */
void bank_rupt(bank_ptr b);

/**
COMP3331/9331 Computer Networks and Applications
Programming Tutorial - Online Banking System
Written by Gary Hu

This file contains the java bank class for the online banking system.
 */
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.Map;

public class Bank {
    private Map<String, Account> accounts;

    /**
     * Initialise the bank, using the account information provided in the accountsFile.
     * The file should contain one account per line, with the account name, hashed password,
     * and balance separated by whitespace.
     *
     * @param accountsFile Path to the file containing account information.
     */
    public Bank(String accountsFile) {
        this.accounts = loadAccounts(accountsFile);
    }

    /**
     * Adds an account to the bank database. The account name must be unique. The opening balance is zero.
     *
     * @param account      A name to unique identify the account.
     * @param passwordHash A SHA-1 hash of the account password.
     * @return A response phrase, indicating success or failure.
     */
    public String openAccount(String account, String passwordHash) {
        if (accounts.containsKey(account)) {
            return "account already exists";
        }
        accounts.put(account, new Account(passwordHash, 0.0));
        return "successful";
    }

    /**
     * Returns the balance of the account. The request is only successful if the account exists
     * and the provided password hash matches the database.
     *
     * @param account      Account name.
     * @param passwordHash SHA-1 hash of the account password.
     * @return The balance, formatted as a string with two decimal places.
     */
    public String getBalance(String account, String passwordHash) {
        if (!isAuthorized(account, passwordHash)) {
            return "not authorised";
        }
        return String.format("%.2f", accounts.get(account).getBalance());
    }

    /**
     * Transfer funds from one account to another. The transfer is only successful if the fromAccount exists,
     * the hashed password matches, the toAccount exists, and the fromAccount has sufficient funds.
     *
     * @param fromAccount  Account name of the sender.
     * @param passwordHash SHA-1 password hash of the sender.
     * @param toAccount    Account name of the receiver.
     * @param amount       Amount to transfer.
     * @return A response phrase, indicating success or failure.
     */
    public String transferFunds(String fromAccount, String passwordHash, String toAccount, double amount) {
        if (!isAuthorized(fromAccount, passwordHash) || !accountExists(toAccount)) {
            return "not authorised";
        }

        // TODO 8: Critical section of code, ensure that only one thread can
        // access the bank accounts at a time.
    }

    /**
     * Check if the account exists in the bank database.
     *
     * @param account Account name.
     * @return True if the account exists, false otherwise.
     */
    private boolean accountExists(String account) {
        return accounts.containsKey(account);
    }

    /**
     * Check if the account exists and the provided password hash matches.
     *
     * @param account      Account name.
     * @param passwordHash SHA-1 hash of the account password.
     * @return True if authorized, false otherwise.
     */
    private boolean isAuthorized(String account, String passwordHash) {
        if (!accountExists(account)) {
            return false;
        }
        return accounts.get(account).getPasswordHash().equals(passwordHash);
    }

    /**
     * Check if the account has sufficient funds to transfer the amount.
     *
     * @param account Account name.
     * @param amount  Amount to transfer.
     * @return True if sufficient funds, false otherwise.
     */
    private boolean hasSufficientBalance(String account, double amount) {
        return accounts.get(account).getBalance() >= amount;
    }

    /**
     * Update the balance of the account by the specified amount.
     *
     * @param account Account name.
     * @param amount  Amount to update.
     */
    private void updateBalance(String account, double amount) {
        Account acc = accounts.get(account);
        acc.setBalance(acc.getBalance() + amount);
    }

    /**
     * Load the account information from a file.
     *
     * @param filename Path to the file containing account information.
     * @return A map of account names to Account objects.
     */
    private static Map<String, Account> loadAccounts(String filename) {
        Map<String, Account> accounts = new HashMap<>();
        Path filepath = Paths.get(filename);

        if (!Files.exists(filepath)) {
            System.err.println("Error: " + filename + " does not exist.");
            System.exit(1);
        }

        try (BufferedReader reader = new BufferedReader(new FileReader(filename))) {
            String line;
            while ((line = reader.readLine()) != null) {
                String[] parts = line.split("\\s+");
                String account = parts[0];
                String passwordHash = parts[1];
                double balance = Double.parseDouble(parts[2]);
                accounts.put(account, new Account(passwordHash, balance));
            }
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(1);
        }

        return accounts;
    }
}

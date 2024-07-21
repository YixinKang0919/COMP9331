/**
COMP3331/9331 Computer Networks and Applications
Programming Tutorial - Online Banking System
Written by Gary Hu

This file contains the java account class for the online banking system.
 */
public class Account {
    private String passwordHash;
    private double balance;

    /**
     * Constructor for the Account class.
     *
     * @param passwordHash The SHA-1 hash of the account password.
     * @param balance      The initial balance of the account.
     */
    public Account(String passwordHash, double balance) {
        this.passwordHash = passwordHash;
        this.balance = balance;
    }

    public String getPasswordHash() {
        return passwordHash;
    }

    public double getBalance() {
        return balance;
    }

    public void setBalance(double balance) {
        this.balance = balance;
    }
}

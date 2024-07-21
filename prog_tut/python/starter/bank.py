"""
COMP3331/9331 Computer Networks and Applications
Programming Tutorial - Online Banking System
Written by Tim Arney <t.arney@unsw.edu.au>

This file contains the bank class for the online banking system.
"""

from dataclasses import dataclass
from pathlib import Path
import sys
import threading


@dataclass
class Account:
    """Bank will store a dict of accounts, where the key is the account name"""
    password_hash: str
    balance: float = 0.0


class Bank:
    def __init__(self, accounts_file: str):
        """Initialise the bank, using the account information provided in the
           accounts_file.  The file should contain one account per line, with
           the account name, hashed password, and balance separated by 
           whitespace.

        Args:
            accounts_file (str): Path to the file containing account information.
        """
        self.accounts = self._load_accounts(accounts_file)

        # TODO 8.1: Add a lock primitive, assign it to a member variable of the class.


    def open_account(self, account: str, password_hash: str) -> str:
        """Adds an account to the bank database.  The account name must be 
           unique.  The opening balance is zero.

        Args:
            account (str): A name to unique identify the account.
            password_hash (str): A SHA-1 hash of the account password.

        Returns:
            str: A response phrase, indicating success or failure.
        """
        if account in self.accounts:
            return "account already exists"

        self.accounts[account] = Account(password_hash)
        return "successful"

    def get_balance(self, account: str, password_hash: str) -> str:
        """Retuns the balance of the account.  The requests is only successful 
           if the account exists and the provided password hash matches the 
           database.

        Args:
            account (str): Account name.
            password_hash (str): SHA-1 hash of the account password.

        Returns:
            str: The balance, formatted as a string with two decimal places.
        """
        if not self._is_authorised(account, password_hash):
            return "not authorised"
        
        return f"{self.accounts[account].balance:.2f}"

    def transfer_funds(self, from_account: str, password_hash: str, to_account: str, amount: float) -> str:
        """Transfer funds from one account to another.  The transfer is only 
              successful if the from_account exists, the hashed password matches, 
              the to_account exists, and the from_account has sufficient funds.

        Args:
            from_account (str): Account name of the sender.
            password_hash (str): SHA-1 password hash of the sender.
            to_account (str): Account name of the receiver.
            amount (float): Amount to transfer.

        Returns:
            str: A response phrase, indicating success or failure.
        """
        if not self._is_authorised(from_account, password_hash) or not self._account_exists(to_account):
            return "not authorised"
        
        # TODO 8.2: Critical section of code, ensure that only one thread can
        #   access the bank accounts at a time.
        if not self._has_sufficient_balance(from_account, amount):
            return "insufficient funds"

        self._update_balance(from_account, -amount)
        self._update_balance(to_account, amount)
        # end of critical section

        return "successful"

    def _account_exists(self, account: str) -> bool:
        """Check if the account exists in the bank database."""
        return account in self.accounts
    
    def _is_authorised(self, account: str, password_hash: str) -> bool:
        """Check if the account exists and the provided password hash matches."""
        if not self._account_exists(account):
            return False

        return self.accounts[account].password_hash == password_hash

    def _has_sufficient_balance(self, account: str, amount: str) -> bool:
        """Check if the account has sufficient funds to transfer the amount."""
        return self.accounts[account].balance >= amount

    def _update_balance(self, account: str, amount: float) -> None:
        """Update the balance of the account by the specified amount."""
        self.accounts[account].balance += amount
    
    @staticmethod
    def _load_accounts(filename: str) -> dict[str, Account]:
        """Load the account information from a file."""
        accounts = {}
        filepath = Path(filename)
        
        if not filepath.exists():
            sys.exit(f'Error: {filename} does not exist.')

        with open(filename, 'r') as f:
            for line in f:
                account, password_hash, balance = line.split()
                accounts[account] = Account(password_hash, float(balance))

        return accounts

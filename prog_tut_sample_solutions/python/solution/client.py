#! /usr/bin/env python3

"""
COMP3331/9331 Computer Networks and Applications
Programming Tutorial - Online Banking System
Written by Tim Arney <t.arney@unsw.edu.au>

This file contains the client implementation for the online banking system.
The client sends requests to the server to open accounts, check balances, and
transfer funds between accounts.

The client is implemented using a UDP socket.  The client reads the command line
arguments to determine the server port and the operation to perform.  The client
then sends a request to the server, displays the response, and terminates.  So 
the client is more like a command line tool than a traditional client.

Usage: python3 client.py server_port {open,balance,transfer,crack} ... 

The client supports the following operations:

- open: Open a new account with the provided account name and password.

  Usage: python3 client.py <server_port> open <account> <password>
  Example: python3 client.py 54321 open alice password

- balance: Check the balance of the account with the provided account name and
  password.

  Usage: python3 client.py <server_port> balance <account> <password>
  Example: python3 client.py 54321 balance alice password

- transfer: Transfer funds from one account to another account.

  Usage: python3 client.py <server_port> transfer <from_account> <from_password> <to_account> <amount>
  Example: python3 client.py 54321 transfer alice password bob 100.0

- crack: Crack the password of the account using a wordlist file.

  Usage: python3 client.py <server_port> crack <account> <wordlist>
  Example: python3 client.py 54321 crack alice wordlist.txt

The server is expected to be running on the same machine as the client, and the
server should be started before the client is run.
"""

import argparse
import hashlib
import socket
import sys

def main():
    # You can basically ignore the main function, it's just a lot of argument 
    # parsing and then calling the appropriate function based on the command.
    # Review the argparse documentation in the Python standard library though
    # if you'd like to understand more.
    parser = argparse.ArgumentParser()
    parser.add_argument('server_port', type=int, help='UDP port of the server')
    subparsers = parser.add_subparsers(dest='operator', required=True, help='operator help')

    open_parser = subparsers.add_parser('open', help='open a new account')
    open_parser.add_argument('account', help='account name')
    open_parser.add_argument('password', help='account password')

    balance_parser = subparsers.add_parser('balance', help='check the balance of an account')
    balance_parser.add_argument('account', help='account name')
    balance_parser.add_argument('password', help='account password')

    transfer_parser = subparsers.add_parser('transfer', help='transfer funds between accounts')
    transfer_parser.add_argument('from_account', help='sending account name')
    transfer_parser.add_argument('from_password', help='sending account password')
    transfer_parser.add_argument('to_account', help='receiving account name')
    transfer_parser.add_argument('amount', type=float, help='amount to transfer')

    crack_parser = subparsers.add_parser('crack', help='crack the password of an account')
    crack_parser.add_argument('account', help='account name')
    crack_parser.add_argument('wordlist', help='wordlist file')

    args = parser.parse_args()

    if args.operator == 'open':
        open_account(args.server_port, args.account, args.password)
    elif args.operator == 'balance':
        check_balance(args.server_port, args.account, args.password)
    elif args.operator == 'transfer':
        transfer_funds(args.server_port, args.from_account, args.from_password, args.to_account, args.amount)
    elif args.operator == 'crack':
        crack_account(args.server_port, args.account, args.wordlist)
    else:
        sys.exit('Invalid command')

def open_account(server_port: int, account: str, password: str) -> None:
    """Contacts the server to open a new account with the provided account name
       and password.

    Args:
        server_port (int): UDP port of the server.
        account (str): Name of the account to open.
        password (str): Password for the account.
    """
    # Bank doesn't store plaintext passwords, so hash the password before 
    # sending it to the server.
    password_hash = hashlib.sha1(password.encode()).hexdigest()

    # TODO: Construct a request to open an account, create a UDP socket, encode 
    #   the request and send it to the `server_port` of the loopback interface.
    #   Then wait to receive the response, decode and print it.
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.sendto(f'open\n{account}\n{password_hash}'.encode(), ('localhost', server_port))
        response, _ = sock.recvfrom(1024)
        print(response.decode())

def check_balance(server_port: int, account: str, password: str) -> None:
    """Contacts the server to check the balance of the account with the provided
       account name and password.
    """
    password_hash = hashlib.sha1(password.encode()).hexdigest()

    # TODO: Construct a request to check an account balance, create a UDP socket,  
    #   encode the request and send it to the `server_port` of the loopback 
    #   interface. Then wait to receive the response, decode and print it.
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.sendto(f'balance\n{account}\n{password_hash}'.encode(), ('localhost', server_port))
        response, _ = sock.recvfrom(1024)
        print(response.decode())

def transfer_funds(server_port: int, from_account: str, from_password: str, to_account: str, amount: float) -> None:
    """Contacts the server to transfer funds from one account to another account."""
    password_hash = hashlib.sha1(from_password.encode()).hexdigest()
    
    # TODO: Construct a request to transfer funds, create a UDP socket, encode 
    #   the request and send it to the `server_port` of the loopback interface.
    #   Then wait to receive the response, decode and print it.
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.sendto(f'transfer\n{from_account}\n{password_hash}\n{to_account}\n{amount}'.encode(), ('localhost', server_port))
        response, _ = sock.recvfrom(1024)
        print(response.decode())

def crack_account(server_port: int, account: str, wordlist: str) -> None:
    """Cracks the password of the account using a wordlist file."""
    print(f'Account: {account}')

    with open(wordlist, 'r') as f:
        # TODO: Create a UDP socket.  For each password in the wordlist, hash 
        #   password, construct a request to check the balance of the account 
        #   with the hashed password, encode the request and send it to the
        #   server.  Then wait to receive the response, decode it, and check if 
        #   the request was successful.  If so, break the loop having found the
        #   password.
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
            for line in f:
                password = line.strip()
                password_hash = hashlib.sha1(password.encode()).hexdigest()

                # We use carriage return to overwrite the previous password
                # attempt on the terminal.
                display = f'Password: {password}'
                print(display, end='\r')

                sock.sendto(f'balance\n{account}\n{password_hash}'.encode(), ('localhost', server_port))
                response, _ = sock.recvfrom(1024)

                if response.decode() != 'not authorised':
                    print('\nBalance:', response.decode())
                    break

                # Clear the previous password attempt from the terminal.
                print(' ' * len(display), end='\r', flush=True)

if __name__ == '__main__':
    main()
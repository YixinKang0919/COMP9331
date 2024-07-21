#! /usr/bin/env python3

"""
COMP3331/9331 Computer Networks and Applications
Programming Tutorial - Online Banking System
Written by Tim Arney <t.arney@unsw.edu.au>

This file contains the server implementation for the online banking system.
The server listens for incoming requests from clients, processes the requests,
and sends a response back to the client.

The server is implemented using a UDP socket, and the server logic is executed
in a separate thread for each request.  This allows the server to handle
multiple clients concurrently.

The server uses a Bank object to manage the account information and process
the requests.  The Bank object is responsible for opening accounts, checking
balances, and transferring funds between accounts.  It is initialised with the
account information provided in a file.  The file should contain one account
per line, with the account name, hashed password, and balance separated by
whitespace.

The server is started by running the main() function, which reads the command
line arguments and initialises the server and bank objects.  The server is
then executed by calling its run() method.

Usage: python3 server.py <accounts_file> <server_port>
Example: python3 server.py accounts.tsv 54321
"""

import datetime
import socket
import sys
import threading
import time

"""
Standard libraries included above that you may find helpful to complete the task:

[datetime]: https://docs.python.org/3/library/datetime.html
[socket]: https://docs.python.org/3/library/socket.html
[threading]: https://docs.python.org/3/library/threading.html
[time]: https://docs.python.org/3/library/time.html
"""

from bank import Bank

def main():
    if len(sys.argv) != 3:
        sys.exit(f'Usage: {sys.argv[0]} <accounts_file> <server_port>')

    accounts_file = sys.argv[1]
    try:
        server_port = int(sys.argv[2])
    except ValueError:
        sys.exit('Error: server_port must be an integer.')

    bank = Bank(accounts_file)
    server = Server(server_port, bank)

    try:
        server.run()
    except KeyboardInterrupt:
        print('\nExiting...')


class Server:
    _RATE_LIMIT = 0.05 # Time in seconds to delay sending responses.

    def __init__(self, server_port: int, bank: Bank):
        """Initialise the server with the specified port and bank.

        Args:
            server_port (int): The UDP port to listen on.
            bank (Bank): The bank object to process requests.
        """
        self.server_port = server_port
        self.bank = bank

        # TODO 1: Create a UDP socket, bind it to the `server_port` of the 
        #   loopback interface, and assign it to a member variable of the class.


    def run(self):
        """The main server loop, where the server listens for incoming requests."""
        print(f'Server running on port {self.server_port}...')
        print('Press Ctrl+C to exit.')

        while True:
            # TODO 2: (Wait to) receive an incoming client request.
            pass # Remove this line when implementing the above.

            # TODO 3: Once received, call the function responsible for processing 
            #   requests, passing it the data and the client address.
            #   (single threaded version)
            # self._process_request(data, addr)


            # TODO 6: Instead of the above, create a new thread to process the 
            #   request and start it running.



    def _process_request(self, data: bytes, addr: tuple[str, int]) -> None:
        """The main server logic, which processes the incoming request and sends
           the response back to the client.

           Additional error checking should be added here, to handle invalid
           requests, but this has been omitted for simplicity.

        Args:
            data (bytes): The incoming request data.
            addr (tuple[str, int]): The address of the client.
        """
        # TODO 4.1: Decode the data back into string form, split on line breaks.


        # TODO 4.2: Extract the operation, account, and password hash from the request.
        

        # TODO 4.3: Process the request based on the operation.


        # TODO 5: Log the request and response to the terminal.


        # TODO 7: Simulate a rate limiting delay to discourage brute force attacks.


        # TODO 4.4: Encode the response string and send it back to the client.
        


if __name__ == '__main__':
    main()
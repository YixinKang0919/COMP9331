
# Import relavent packages
import socket
import sys
import threading
import datetime
import time


def main():
    if len


class Server:    
    def __init__(self, master_file: str, server_port: int ) -> None:
        """Initialise the server with the specified port and 
        
        Args: 
            server_port (int):  The UDP port to listen on
            master_file: The file name of the master file
        """
        self.master_file = master_file
        self.server_host = 'localhost'
        self.server_port = server_port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind((self.server_host, self.server_port))
        self.cache = self.load_master_file
    
    
    # Load data from the master file into the Server Class in data type of dictionary
    def load_master_file(self):
        cache = {}
        with open(self.master_file,'r') as file:
            for line in file:
                domain, record_type, value = line.strip().split()
                if domain not in cache:
                    # Create dictionary of dictionary, easy for look up
                    # {'foo.example.com.': {'CNAME': 'bar.example.com.'}
                    cache[domain] = {}
                if record_type not in cache[domain]:
                    cache[domain][record_type] = []
                cache[domain][record_type].append(value)
        return cache
                
        
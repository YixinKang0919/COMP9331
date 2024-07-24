
# Import relavent packages
import socket
import sys
import datetime
import time
import random

# $ python3 client.py server_port qname qtype timeout
def main():
    if len(sys.argv) != 5:
        sys.exit(f'Usage: {sys.argv[0]} <server_port> <qname> <qtype> <timeout>')
    try:
        server_port = int(sys.argv[1])
    except ValueError:
        sys.exit('Error: server_port must be an integer.')
    qname = sys.argv[2]
    qtype = sys.argv[3]
    timeout = float(sys.argv[4])
    client = Client(server_port, qname, qtype, timeout)
    try:
        client.query()
    except KeyboardInterrupt:
        print('\nExiting...')

class Client:
    def __init__(self, server_port: int, qname: str, qtype: str, timeout: float) -> None:
        """Initialise the client with the specified port, query name, query type and the timeout limit
        
        Args: 
            server_port (int):  The UDP port to listen on
            qname: Domain to be query
            qtype: query type, A CNAME NS MX etc...
            timeout: the time set to wait before kill
        """
        self.server_host = 'localhost'
        self.server_port = server_port
        # Generate a random qid
        self.qid = random.randint(0, 65535)
        self.qname = qname
        self.qtype = qtype
        self.timeout = timeout
        
    def build_query(self) ->str:
        """The message is in format of              query_id 
                                               domain  record type                             
        """
        # Create the header
        query_id = str(self.qid)
        
        # Question Section
        domain = self.qname
        record_type = self.qtype
        
        message = query_id + '\n' + domain + ' ' + record_type
        
        return message
    
    def analyze_response(self, response) -> str: 
        response = response.decode()
        print(response)
        
    
    def query(self) -> None:
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
            message = self.build_query()
            # Send the message
            s.sendto(message.encode(), (self.server_host, self.server_port))
            try:
                # Set timeout
                s.settimeout(self.timeout)
                response, serverAddress = s.recvfrom(4096)
                self.analyze_response(response)
            except socket.timeout:
                print('timed out.')
                

if __name__ == '__main__':
    main()

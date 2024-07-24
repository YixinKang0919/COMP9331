
# Import relavent packages
import socket
import sys
import threading
import datetime
import time
import random



#$ python3 server.py server_port


def main():
    if len(sys.argv) != 2:
        sys.exit(f'Usage: {sys.argv[0]} <server_port>')

    try:
        server_port = int(sys.argv[1])
    except ValueError:
        sys.exit('Error: server_port must be an integer.')

    server = Server("master.txt", server_port)
    
    try:
        server.run()
    except KeyboardInterrupt:
        print('\nExiting...')
        
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
        # Create a UDP socket
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind((self.server_host, self.server_port))
        
        # Load the master file into the cache of the server
        self.cache = self.load_master_file()

    # Load data from the master file into the Server Class in data type of dictionary
    def load_master_file(self) -> dict:
        cache = {}
        with open(self.master_file,'r') as file:
            for line in file:
                domain, record_type, value = line.strip().split()
                if domain not in cache:
                    # Create dictionary of dictionary, easy for look up
                    # {'foo.example.com.': {'CNAME': ['bar.example.com.']}
                    cache[domain] = {}
                if record_type not in cache[domain]:
                    cache[domain][record_type] = []
                cache[domain][record_type].append(value)
        return cache
                
    def run(self) -> None:
        """The main server loop, where the server listens for incoming requests"""
        print(f'Server running on port {self.server_port}...')
        print('Press Ctrl+C to exit.')
        
        while True:
            data, addr = self.sock.recvfrom(4096)
            # Threading
            child = threading.Thread(target=self._process_request, args=(data,addr))
            child.start()
    
    def build_response(self, domain:str, record_type:str, message:str)-> str:
        # When we dig the CNAME of the domain, we need to recursively dig deeper
        if domain in self.cache:
            # If we can find a CNAME node, we will print the CNAME record
            if 'CNAME' in self.cache[domain]:
                new_domains = self.cache[domain]['CNAME']
                for new_domain in new_domains:
                    message += domain + ' ' + 'CNAME' + ' ' + new_domain + '\n'
                    if record_type == 'CNAME':
                        return message
                    else:
                        # If the record type is not CNAME, we will keep tracing on the CNAME we just found
                        return self.build_response(new_domain, record_type, message)
            elif record_type in self.cache[domain]:
                values = self.cache[domain][record_type]
                for value in values:
                    message += domain + ' ' + record_type + ' ' + value + '\n'
                return message
        else:
            domain_parts = domain.split('.')
            message += '\n' + 'AUTHORITY SECTION' + '\n'
            for i in range(1, len(domain_parts)):
                for j in range(i, len(domain_parts)):
                    # find the first ancestor and then break the for loop
                    ancestor = domain_parts[j] + '.'
                    if ancestor in self.cache and 'NS' in self.cache[ancestor]:
                        nss = self.cache[ancestor]['NS']
                        for ns in nss:
                            message += ancestor + ' ' + 'NS' + ' ' + ns + '\n'
                        # check if there is a corresponding IP address recorded
                        for ns in nss:
                            if ns in self.cache and 'A' in self.cache[ns]:
                                message += '\n' + 'ADDITIONAL SECTION' + '\n'
                                break
                        for ns in nss:
                            if ns in self.cache and 'A' in self.cache[ns]:
                                values = self.cache[ns]['A']
                                for value in values:
                                    message += ns + ' ' + 'A' + ' ' + value + '\n'
                    break
                break
            return message
                            
    def _process_request(self, data: bytes, addr: tuple[str, int]) -> None:
        """ The main server logic, which processes the incoming request and sends 
            the response back to the client.
            
            The final message sent from server to the client is in form of    
                    ID
                    
                    QUESTION SECTION
                    
                    ANSWER SECTION
                    
                    AUTHORITY SECTION
                    
                    ADDITIONAL SECTION
        """
        # record the recevied time
        start_time = datetime.datetime.now()
        start_time = start_time.strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]
        delay = random.choice([0, 1, 2, 3, 4])
        
        # extract value 
        header, question = data.decode().split('\n')
        domain, record_type = question.split()
        
        # log
        print(f'{start_time} rcv {addr[1]}: {header} {domain} {record_type} (delay: {delay}s)')
        
        # Delay processing the query for a random amount of time of 0, 1, 2, 3, or 4 seconds. 
        time.sleep(delay)
        
        # build the response
        answer_message = ''
        answer_message += self.build_response(domain, record_type, answer_message)
        message = ''
        message += 'ID: ' + header + '\n'
        message += '\n' + 'QUESTION SECTION' + '\n'
        message += domain + ' ' + record_type + '\n'
        message += '\n' + 'ANSWER SECTION' + '\n'
        message += answer_message
        self.sock.sendto(message.encode(), addr)
        
        # record the sent time
        end_time = datetime.datetime.now()
        end_time = end_time.strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]
        
        # log
        print(f'{end_time} rcv {addr[1]}: {header} {domain} {record_type}')
        
        
if __name__ == '__main__':
    main()

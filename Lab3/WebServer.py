import socket
from socket import *
import sys

# record the serverport from cmd line
hostname = 'localhost'
serverPort = int(sys.argv[1])

# print(f"\nIP address:{hostname}")

# create a TCP socket
serverSocket = socket(AF_INET, SOCK_STREAM)

# create socket for listenning
serverSocket.bind((hostname,serverPort))
serverSocket.listen(5)
serverSocket.setsockopt(SOL_SOCKET, SO_KEEPALIVE, 1)

# Loop for listen
while True:
    # create TCP connection
    # print('The server is ready')
    connectionSocket, addr = serverSocket.accept()
    connected = True
    # set timeout for connection
    connectionSocket.settimeout(10.0)
    # print('Connected')
    while connected:
        try:
            request = connectionSocket.recv(1024).decode('utf-8')
            # print(request)
            if not request:
                break
            requested_file = request.split()[1]
            # print(requested_file)
            file = open('.'+requested_file,'rb')
            # print('file opened!')
            content = file.read()
            # print(f'content is {content}')
            response_header = "HTTP/1.1 200 OK"
            response_header += "Connection: keep-alive\r\n\r\n"
            connectionSocket.send(response_header.encode('utf-8'))
            connectionSocket.send(content)
            # connectionSocket.close()
            # print('correctly closed')

        except timeout:
            connectionSocket.close()
            connected = False
            # print("closed")


        except IOError:
            response_header = "HTTP/1.1 404 Not Found"
            response_header += "Connection: keep-alive\r\n\r\n"
            connectionSocket.send(response_header.encode('utf-8'))
            connectionSocket.send(b"<html><body><h1>404 Not Found</h1></body></html>")
            connectionSocket.close()
            # print('error closed')
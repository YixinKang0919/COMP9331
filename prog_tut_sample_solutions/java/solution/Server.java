/**
COMP3331/9331 Computer Networks and Applications
Programming Tutorial - Online Banking System
Written by Gary Hu

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

Compile: javac Server.java
Usage: java Server <accounts_file> <serverPort>
Example: java Server accounts.tsv 54321
 */

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.nio.charset.StandardCharsets;
import java.util.Date;

public class Server {
    private static final double RATE_LIMIT = 0.01; // Time in seconds to delay sending responses
    private int serverPort;
    private Bank bank;
    private DatagramSocket socket;

    /**
     * Initialise the server with the specified port and bank.
     *
     * @param serverPort The UDP port to listen on.
     * @param bank       The bank object to process requests.
     */
    public Server(int serverPort, Bank bank) throws SocketException {
        this.serverPort = serverPort;
        this.bank = bank;

        // TODO 1: Create a UDP socket, bind it to the `serverPort` of the 
        // loopback interface, and assign it to a member variable of the class.
        this.socket = new DatagramSocket(this.serverPort, InetAddress.getLoopbackAddress());
    }

    /**
     * The main server loop, where the server listens for incoming requests.
     */
    public void run() {
        System.out.println("Server running on port " + serverPort + "...");
        System.out.println("Press Ctrl+C to exit.");

        while (true) {
            try {
                // TODO 2: (Wait to) receive an incoming client request.
                byte[] buffer = new byte[1024];
                DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
                socket.receive(packet);

                // TODO 3: Once received, call the function responsible for processing requests, passing it the data and the client address.
                // (single threaded version)
                // processRequest(packet);
                // TODO 6: Instead of processing the request directly, create a new thread to process the request and start it running.
                Thread child = new Thread(() -> processRequest(packet));
                child.start();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    /**
     * The main server logic, which processes the incoming request and sends the response back to the client.
     * Additional error checking should be added here, to handle invalid requests, but this has been omitted for simplicity.
     *
     * @param packet The incoming request packet.
     */
    private void processRequest(DatagramPacket packet) {
        try {
            // TODO 4.1: Decode the data back into string form, split on line breaks.
            String data = new String(packet.getData(), 0, packet.getLength(), StandardCharsets.UTF_8);
            String[] request = data.split("\n");

            // TODO 4.2: Extract the operation, account, and password hash from the request.
            String operation = request[0];
            String account = request[1];
            String passwordHash = request[2];

            String response;

            // TODO 4.3: Process the request based on the operation.
            switch (operation) {
                case "open":
                    response = bank.openAccount(account, passwordHash);
                    break;
                case "balance":
                    response = bank.getBalance(account, passwordHash);
                    break;
                case "transfer":
                    String toAccount = request[3];
                    double amount = Double.parseDouble(request[4]);
                    response = bank.transferFunds(account, passwordHash, toAccount, amount);
                    break;
                default:
                    response = "bad Request";
            }

            // TODO 5: Log the request and response to the terminal.
            Date timestamp = new Date();
            InetAddress clientAddress = packet.getAddress();
            int clientPort = packet.getPort();
            System.out.printf("[%s] %s:%d - %s %s - %s%n", timestamp, clientAddress.getHostAddress(), clientPort, operation, account, response);

            // TODO 7: Simulate a rate limiting delay to discourage brute force attacks.
            Thread.sleep((long) (RATE_LIMIT * 1000));

            // TODO 4.4: Encode the response string and send it back to the client.
            byte[] responseData = response.getBytes(StandardCharsets.UTF_8);
            DatagramPacket responsePacket = new DatagramPacket(responseData, responseData.length, packet.getAddress(), packet.getPort());
            socket.send(responsePacket);
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }

    public static void main(String[] args) {
        if (args.length != 2) {
            System.err.println("Usage: java Server <accounts_file> <serverPort>");
            System.exit(1);
        }

        String accountsFile = args[0];
        int serverPort;
        try {
            serverPort = Integer.parseInt(args[1]);
        } catch (NumberFormatException e) {
            System.err.println("Error: serverPort must be an integer.");
            System.exit(1);
            return;
        }

        Bank bank = new Bank(accountsFile);
        Server server;
        try {
            server = new Server(serverPort, bank);
            server.run();
        } catch (SocketException e) {
            e.printStackTrace();
            System.exit(1);
        }
    }
}

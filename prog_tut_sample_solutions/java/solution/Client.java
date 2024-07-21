/**
COMP3331/9331 Computer Networks and Applications
Programming Tutorial - Online Banking System
Written by Gary Hu

This file contains the client implementation for the online banking system.
The client sends requests to the server to open accounts, check balances, and
transfer funds between accounts.

The client is implemented using a UDP socket.  The client reads the command line
arguments to determine the server port and the operation to perform.  The client
then sends a request to the server, displays the response, and terminates.  So 
the client is more like a command line tool than a traditional client.

Compile: javac Client.java
Usage: java Client serverPort {open,balance,transfer,crack} ... 

The client supports the following operations:

- open: Open a new account with the provided account name and password.

  Usage: java Client <serverPort> open <account> <password>
  Example: java Client 54321 open alice password

- balance: Check the balance of the account with the provided account name and
  password.

  Usage: java Client <serverPort> balance <account> <password>
  Example: java Client 54321 balance alice password

- transfer: Transfer funds from one account to another account.

  Usage: java Client <serverPort> transfer <from_account> <from_password> <to_account> <amount>
  Example: java Client 54321 transfer alice password bob 100.0

- crack: Crack the password of the account using a wordlist file.

  Usage: java Client <serverPort> crack <account> <wordlist>
  Example: java Client 54321 crack alice wordlist.txt

The server is expected to be running on the same machine as the client, and the
server should be started before the client is run.
 */
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.net.*;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

public class Client {
    /**
     * main() function: argument parsing and then calling the appropriate function
     */
    public static void main(String[] args) {
        if (args.length < 3) {
            System.err.println("Usage: java Client <serverPort> <operation> <params...>");
            System.exit(1);
        }

        int serverPort = Integer.parseInt(args[0]);
        String operation = args[1];
        try {
            switch (operation) {
                case "open":
                    if (args.length != 4) {
                        System.err.println("Usage: java Client <serverPort> open <account> <password>");
                        System.exit(1);
                    }
                    openAccount(serverPort, args[2], args[3]);
                    break;
                case "balance":
                    if (args.length != 4) {
                        System.err.println("Usage: java Client <serverPort> balance <account> <password>");
                        System.exit(1);
                    }
                    checkBalance(serverPort, args[2], args[3]);
                    break;
                case "transfer":
                    if (args.length != 6) {
                        System.err.println("Usage: java Client <serverPort> transfer <from_account> <from_password> <to_account> <amount>");
                        System.exit(1);
                    }
                    transferFunds(serverPort, args[2], args[3], args[4], Double.parseDouble(args[5]));
                    break;
                case "crack":
                    if (args.length != 4) {
                        System.err.println("Usage: java Client <serverPort> crack <account> <wordlist>");
                        System.exit(1);
                    }
                    crackAccount(serverPort, args[2], args[3]);
                    break;
                default:
                    System.err.println("Invalid command");
                    System.exit(1);
            }
        } catch (IOException | NoSuchAlgorithmException e) {
            e.printStackTrace();
        }
    }

    /**
     * Contacts the server to open a new account with the provided account name and password.
     *
     * @param serverPort UDP port of the server.
     * @param account    Name of the account to open.
     * @param password   Password for the account
     */
    private static void openAccount(int serverPort, String account, String password) throws IOException, NoSuchAlgorithmException {
        String passwordHash = hashPassword(password);
        String request = "open\n" + account + "\n" + passwordHash;

        // TODO: Construct a request to open an account, create a UDP socket, encode 
        //   the request and send it to the `serverPort` of the loopback interface.
        //   Then wait to receive the response, decode and print it.
        String response = sendRequest(serverPort, request);
        System.out.println(response);
    }

    /**
     * Contacts the server to check the balance of the account with the provided
       account name and password.
     */
    private static void checkBalance(int serverPort, String account, String password) throws IOException, NoSuchAlgorithmException {
        String passwordHash = hashPassword(password);
        String request = "balance\n" + account + "\n" + passwordHash;

        // TODO: Construct a request to check an account balance, create a UDP socket,  
        //   encode the request and send it to the `serverPort` of the loopback 
        //   interface. Then wait to receive the response, decode and print it.
        String response = sendRequest(serverPort, request);
        System.out.println(response);
    }

    /**
     * Contacts the server to transfer funds from one account to another account.
     */
    private static void transferFunds(int serverPort, String fromAccount, String fromPassword, String toAccount, double amount) throws IOException, NoSuchAlgorithmException {
        String passwordHash = hashPassword(fromPassword);
        String request = "transfer\n" + fromAccount + "\n" + passwordHash + "\n" + toAccount + "\n" + amount;

        // TODO: Construct a request to transfer funds, create a UDP socket, encode 
        //   the request and send it to the `serverPort` of the loopback interface.
        //   Then wait to receive the response, decode and print it.
        String response = sendRequest(serverPort, request);
        System.out.println(response);
    }

    /**
     * Cracks the password of the account using a wordlist file.
     */
    private static void crackAccount(int serverPort, String account, String wordlist) throws IOException, NoSuchAlgorithmException {
        try (BufferedReader reader = new BufferedReader(new FileReader(wordlist))) {
            String line;
            while ((line = reader.readLine()) != null) {
                String passwordHash = hashPassword(line.strip());
                String request = "balance\n" + account + "\n" + passwordHash;

                // TODO: Create a UDP socket. For each password in the wordlist, hash 
                //   password, construct a request to check the balance of the account 
                //   with the hashed password, encode the request and send it to the
                //   server. Then wait to receive the response, decode it, and check if 
                //   the request was successful. If so, break the loop having found the
                //   password.
                String response = sendRequest(serverPort, request);
                if (!response.equals("not authorised")) {
                    System.out.println("Password found: " + line);
                    System.out.println("Balance: " + response);
                    break;
                }
            }
        }
    }

    /**
     * sendRequest() is a helper function to send the UDP packet
     */
    private static String sendRequest(int serverPort, String request) throws IOException {
        InetAddress address = InetAddress.getLoopbackAddress();
        byte[] buf = request.getBytes(StandardCharsets.UTF_8);
        DatagramPacket packet = new DatagramPacket(buf, buf.length, address, serverPort);

        try (DatagramSocket socket = new DatagramSocket()) {
            socket.send(packet);
            byte[] buffer = new byte[1024];
            DatagramPacket responsePacket = new DatagramPacket(buffer, buffer.length);
            socket.receive(responsePacket);
            return new String(responsePacket.getData(), 0, responsePacket.getLength(), StandardCharsets.UTF_8);
        }
    }

    /**
     * Bank doesn't store plaintext passwords, so hash the password before sending it to the server.
     */
    private static String hashPassword(String password) throws NoSuchAlgorithmException {
        MessageDigest md = MessageDigest.getInstance("SHA-1");
        byte[] bytes = md.digest(password.getBytes(StandardCharsets.UTF_8));
        StringBuilder sb = new StringBuilder();
        for (byte b : bytes) {
            sb.append(String.format("%02x", b));
        }
        return sb.toString();
    }
}

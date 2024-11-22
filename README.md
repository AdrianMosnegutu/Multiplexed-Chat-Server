# Multiplexed Chat Server

## Overview

This project is a multi-client TCP-based group chat application built in C. It includes both a server and client implementation, enabling real-time communication between multiple users over a network. The server employs efficient I/O multiplexing using the select system call, while the client offers a simple interface for connecting to the server and participating in the chat. Shared utility functions are included for streamlined socket management.

## Features

### Server Features

- **Concurrent Client Connections**: Manages multiple clients simultaneously using a single-threaded approach with select.
- **Real-Time Messaging**: Broadcasts messages from any client to all other connected clients efficiently.
- **Dynamic Client Management**: 
    - Handles new connections dynamically.
    - Removes inactive or disconnected clients gracefully.
- **Server-Side Commands**:
    - quit: Shuts down the server and disconnects all clients.
- **Comprehensive Error Handling**: Ensures stability and clean shutdown during unexpected errors.

### Client Features

- **User Authentication**: Allows users to set a username when connecting to the server.
- **Real-Time Chat**: Sends messages to the server for broadcasting to other connected clients.
- **Interactive Interface**:
	- Displays incoming messages in real-time.
	- Supports text input for sending messages.
- **Disconnection Handling**: Handles clean disconnection from the server when a client exits or the server shuts down.

### Shared Utilities

- **Socket Utilities**: Common functions for socket creation, error handling, and IPv4 address initialization, shared between the server and client.


## How it works

1. **Server**

- Initializes a TCP socket, binds it to a specified port, and begins listening for incoming client connections.
- Manages incoming and outgoing messages and broadcasts them to all connected clients.

2. **Client**

- Connects to the server using the server’s IP and port.
- Sends a username upon connection, which is announced to other clients.
- Allows users to send messages and receive real-time updates from other users in the chat.

## Requirements

- **Operating System**: Linux (or any POSIX-compliant system).
- **Dependencies**:
	- gcc (for compilation).
	- Basic networking libraries (sys/socket.h, arpa/inet.h, etc.).

## Installation and usage

1. **Clone this repository**

    ``` bash
    git clone https://github.com/AdrianMosnegutu/Multiplexed-Chat-Server.git
    cd Multiplexed-Chat-Server
    ```

2. **Compile the server and client**

    #### Compile the server

    ``` bash
    gcc -o chat_server server.c utils/server_utils.c utils/general_utils.c 
    ```

    #### Compile the client
    
    ``` bash
    gcc -o chat_client client.c utils/client_utils.c utils/general_utils.c 
    ```

3. **Run the server**

    Start the server to listen for incoming connections:
    ``` bash
    ./chat_server
    ```

4. **Run any number of clients**

    Clients can connect to the server using the compiled client binary:
    ``` bash
    ./chat_client
    ```

## File Structure

- [`utils/`](./utils/)
    - [`general_utils.h`](./utils/general_utils.h)/[`server_utils.h`](./utils/server_utils.h)/[`client_utils.h`](./utils/client_utils.h): Header files for respective utility functions.
    - [`general_utils.c`](./utils/general_utils.c): Shared utilities for socket creation, IPv4 address initialization, and error handling.
    - [`server_utils.c`](./utils/server_utils.c): Utility functions for server-side socket management and error handling.
    - [`client_utils.c`](./utils/client_utils.c): Utility functions for client-side socket management and input handling.
- [`server.c`](./server.c): Main server logic, including connection handling and message broadcasting.
- [`client.c`](./client.c): Main client logic, including server connection, message sending, and receiving.

## License

This project is licensed under the MIT License. See the [LICENSE](./LICENSE) file for details.

#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <string>

const int PORT = 8080;
const int MAX_CONNECTIONS = 5;

int main() {
  // Create socket
  const int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    std::cerr << "Cannot create socket." << std::endl;
    return 1;
  }

  // Bind socket to IP:port
  sockaddr_in server{};
  memset((char *) &server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  socklen_t server_len = sizeof(server);

  if (bind(server_socket, (const sockaddr *) &server, server_len) == -1) {
    std::cerr << "Cannot bind socket to port." << std::endl;
    return 1;
  }

  // Listen for incoming requests
  if (listen(server_socket, MAX_CONNECTIONS) == -1) {
    std::cerr << "Error occurred while listening for incoming requests." << std::endl;
    return 1;
  }

  while (true) {
    // Accept incoming request
    const int client_socket = accept(server_socket, (sockaddr *) &server, &server_len);
    if (client_socket == -1) {
      std::cerr << "Cannot accept client connection." << std::endl;
      return 1;
    }

    // Receive data from client
    std::vector<char> buffer(4097);  // allow 1 extra byte for '0'
    ssize_t num_bytes = recv(client_socket, buffer.data(), buffer.size() - 1, 0);  // recv is not null-terminated. Allow for '0' at the end
    if (num_bytes == -1) {
      std::cerr << "Error occurred while reading data from client." << std::endl;
      return 1;
    } else if (num_bytes == 0) {
      std::cerr << "Client disconnected." << std::endl;
      break;
    }

    std::cout << "Received: " << std::string(buffer.data(), buffer.size()) << std::endl;

    // Send response
    std::string res = "Hello from server";
    send(client_socket, res.c_str(), res.length() + 1, 0);  // +1 for terminating '0' of c_str

    close(client_socket);
  }
}


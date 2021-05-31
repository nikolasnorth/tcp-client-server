#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <unistd.h>

const int PORT = 8080;

int main() {
  const int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s == -1) {
    std::cerr << "Cannot create socket." << std::endl;
    return 1;
  }

  // Define client to connect with
  const std::string ip_addr = "127.0.0.1";
  sockaddr_in server{};
  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);

  // Convert IP address to binary format
  const int ip_addr_conv = inet_pton(AF_INET, ip_addr.c_str(), &server.sin_addr);
  if (ip_addr_conv == -1) {
    std::cerr << "Error occurred while converting IP address form text to binary" << std::endl;
    return 1;
  } else if (ip_addr_conv == 0) {
    std::cerr << "IP address was not parsable" << std::endl;
    return 1;
  }

  // Connect to client
  const int con = connect(s, (const sockaddr *) &server, sizeof(server));
  if (con == -1) {
    std::cerr << "Cannot connect to client" << std::endl;
    return 1;
  }

  std::vector<char> buffer(4097);
  std::string user_input;
  std::cout << "> ";
  getline(std::cin, user_input);

  ssize_t num_bytes_sent = send(s, user_input.c_str(), user_input.size() + 1, 0);  // +1 needed for terminating '0' of c_str
  if (num_bytes_sent == -1) {
    std::cerr << "Cannot send data to client" << std::endl;
  }

  ssize_t num_bytes_recv = recv(s, buffer.data(), buffer.size() - 1, 0);  // recv is not null-terminated. Allow for '0' at the end
  if (num_bytes_recv == -1) {
    std::cerr << "Cannot receive data from client" << std::endl;
  }

  std::cout << "[Server]: " << std::string(buffer.data(), buffer.size()) << std::endl;

  close(s);
}

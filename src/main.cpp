#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <sstream>

int main(){
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1){
     perror("socket failed");
     return 1;   
    }
  sockaddr_in address;
  std::memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(8080);
if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
    perror("bind failed");
    return 1;
}
if (listen(server_fd, 3) < 0){
    perror("listen failed");
    return 1;
}
std::cout << "Server is listening on port 8080..." << std::endl;
while (true){
    socklen_t addrlen = sizeof(address);
    int client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);
    if (client_fd < 0){
        perror("Accept failed");
        continue;
    }
   char buffer[1024] = {0};
   ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0); //left space for null terminator, just for extra safety
   if(bytes_read >= 0){
    buffer[bytes_read] = '\0'; // Null-terminate the buffer
    }
  
    if (bytes_read <= 0){
        std::cout << "No Data received from client, or connection closed." << std::endl;
    }
    //print the request to the console
    std::string data(buffer, bytes_read);
   std::istringstream stream(data);
   std::string line;
   //Get request line
   std::getline(stream, line);
   if (!line.empty() && line.back() == '\r' ){
    line.pop_back(); //Remove the trailing "r"
   }
    std::string method, path, version;
    std::istringstream requestline(line);
    requestline >> method >> path >> version;
    std::cout << "Method: " << method << std::endl;
    std::cout << "Path: " << path << std::endl;
    std::cout << "Version: " << version << std::endl;
    //Header lines
    while (std::getline(stream, line) && line != "\r"){
     if(!line.empty() && line.back() == '\r'){
        line.pop_back();
     }
     std::cout << "Header: " << line << std:: endl;   
    }
    std::cout << "Received request:\n" << bytes_read << " bytes: ";
    std::cout.write(data.data(), data.size());
    std::cout << std::endl;
    
    //respond to the client 
    send(client_fd, data.data(), data.size(), 0);
    close(client_fd);
}
close(server_fd);
return 0;
}
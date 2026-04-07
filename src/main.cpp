#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <arpa/inet.h>
#include <ctime>

std::string get_timestamp();
void log_request(
    const std::string& client_ip,
    const std::string& method,
    const std::string& path,
    const std::string& status_line);

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
   std::string client_ip = inet_ntoa(address.sin_addr);
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
    if (!(requestline >> method >> path >> version)){
        // malformed request
    std::string response =
        "HTTP/1.1 400 Bad Request\r\n"
        "Content-Length: 0\r\n"
        "\r\n";

    }
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
    //route dynamically
   std::string body;
   std::string status_line;
  
   if (method != "GET"){
    body = "405 Method not allowed";
    status_line = "HTTP/1.1 405 Method not allowed";
   }else if(path == "/"){
    body = "Welcome to my server";
    status_line = "HTTP/1.1 200 OK\r\n";
   }else if (path == "/about"){
    body = "This is the about page";
    status_line = "HTTP/1.1 200 OK\r\n";
   }else{
    body = "404 Not found";
    status_line = "HTTP/1.1 404 Not found\r\n";
   }
  std::string response = 
  status_line +
  "Content-type: text/plain\r\n" +
  "Content-length: " + std::to_string(body.size()) + "\r\n" + 
  "Connection : close\r\n" + 
  "\r\n" +
  body;
  //Send response
 size_t total_sent = 0;
  while(total_sent < response.size()){
   ssize_t sent = send(client_fd, response.data() + total_sent, response.size() - total_sent, 0);
  if(sent <= 0) {
   break; //Connection closed or error
  }
   total_sent += sent;
}
//Close connetion
close(client_fd);
log_request(client_ip, method, path, status_line);
}
close(server_fd);
return 0;
}
std::string get_timestamp(){ 
    time_t now = time(0);
    char* dt = ctime(&now);
    std::string ts(dt);
    ts.pop_back();
    return ts;
   }
void log_request(
    const std::string& client_ip,
    const std::string& method,
    const std::string& path,
    const std::string& status_line){
 std::cout << "[" << get_timestamp() << "]" <<
             client_ip << " " <<
             method << " " <<
             path << " -> " << 
             status_line << std::endl;
 }

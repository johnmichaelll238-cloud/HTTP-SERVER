#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <arpa/inet.h>
#include <ctime>
#include <thread>

// --- Logging utilities ---
std::string get_timestamp() {
    time_t now = time(0);
    char* dt = ctime(&now);
    std::string ts(dt);
    if (!ts.empty()) ts.pop_back(); // remove trailing newline
    return ts;
}

void log_request(
    const std::string& client_ip,
    const std::string& method,
    const std::string& path,
    const std::string& status_line) 
{
    std::cout << "[" << get_timestamp() << "] "
              << client_ip << " "
              << method << " "
              << path << " -> "
              << status_line << std::endl;
}

// --- Per-client handler ---
void handle_client(int client_fd, std::string client_ip) {
    char buffer[4096] = {0};
    ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read <= 0) {
        std::cout << "No data received from client or connection closed.\n";
        close(client_fd);
        return;
    }
    buffer[bytes_read] = '\0'; // Null-terminate

    // Parse request
    std::string data(buffer, bytes_read);
    std::istringstream stream(data);
    std::string line;

    std::getline(stream, line);
    if (!line.empty() && line.back() == '\r') line.pop_back();

    std::string method, path, version;
    std::istringstream requestline(line);
    requestline >> method >> path >> version;

    // Optional: print request for debugging
    std::cout << "Method: " << method << " | Path: " << path << " | Version: " << version << std::endl;

    // Route & response
    std::string body, status_line;
    if (method != "GET") {
        body = "405 Method not allowed";
        status_line = "HTTP/1.1 405 Method not allowed\r\n";
    } else if (path == "/") {
        body = "Welcome to my server";
        status_line = "HTTP/1.1 200 OK\r\n";
    } else if (path == "/about") {
        body = "This is the about page";
        status_line = "HTTP/1.1 200 OK\r\n";
    } else {
        body = "404 Not found";
        status_line = "HTTP/1.1 404 Not found\r\n";
    }

    std::string response =
        status_line +
        "Content-type: text/plain\r\n" +
        "Content-length: " + std::to_string(body.size()) + "\r\n" +
        "Connection: close\r\n\r\n" +
        body;

    // Send response
    size_t total_sent = 0;
    while (total_sent < response.size()) {
        ssize_t sent = send(client_fd, response.data() + total_sent, response.size() - total_sent, 0);
        if (sent <= 0) break;
        total_sent += sent;
    }

    close(client_fd);
    log_request(client_ip, method, path, status_line);
}    

// --- Main server ---
int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket failed"); return 1; }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed"); return 1;
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen failed"); return 1;
    }

    std::cout << "Server is listening on port 8080...\n";

    while (true) {
        socklen_t addrlen = sizeof(address);
        int client_fd = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (client_fd < 0) { perror("accept failed"); continue; }

        // Thread-safe IP copy using inet_ntop
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(address.sin_addr), ip_str, INET_ADDRSTRLEN);
        std::string client_ip(ip_str);

        // Spawn thread per client
        std::thread([client_fd, client_ip]() {
            handle_client(client_fd, client_ip);
        }).detach();
    }

    close(server_fd);
    return 0;
}
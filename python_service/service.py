import socket
import threading 

HOST = '127.0.0.1'
PORT = 9000

def handle_client(conn, addr):
    print(f"Connected by {addr}")
    while True:
        data = conn.recv(4096)
        if not data:
            break
        print(f"Received from {addr}: {data}")
    conn.close()

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((HOST, PORT))
server.listen()

print(f"Python service listening on {PORT}")

while True:
    conn, addr = server.accept()
    threading.Thread(target=handle_client, args=(conn, addr), daemon=True).start()
   
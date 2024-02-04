import socket

# Set up the server socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host = '192.168.4.91'  # Replace with the actual IP of your server machine
port = 20  # Choose a port number

server_socket.bind((host, port))
server_socket.listen(1)  # Listen for one incoming connection

print(f"Server listening on {host}:{port}")

# Accept a connection
client_socket, client_address = server_socket.accept()
print(f"Connection from {client_address}")

# Receive and print numbers
while True:
    data = client_socket.recv(1024)
    if not data:
        break
    print("Received:", data.decode('utf-8'))

# Close the connection
client_socket.close()
server_socket.close()

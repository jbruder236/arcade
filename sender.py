import socket

# Set up the client socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host = 'your_server_ip'  # Replace with the actual IP of your server machine
port = 12345  # Use the same port as the server

# Connect to the server
client_socket.connect((host, port))

# Send numbers
numbers = [1, 2, 3, 4, 5]  # Replace with your list of numbers
for num in numbers:
    client_socket.send(str(num).encode('utf-8'))

# Close the connection
client_socket.close()

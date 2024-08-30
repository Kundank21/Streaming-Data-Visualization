import socket
import struct
import numpy as np

# Create a socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect to the server
server_address = ('127.0.0.1', 5566)  # Replace 'server_ip' with the actual server IP
client_socket.connect(server_address)

# Receive the string from the server
# Receive the data as a 2D array
data = np.empty((64,64))
for row in range(data.shape[0]):
    for col in range(data.shape[1]):
        
        value_bytes = client_socket.recv(8)  # Assuming a double is 8 bytes
        value = struct.unpack('d', value_bytes)[0]
        data[row,col]=value
        # float(d)
        # print(value)
        # data[row,col]=d
# data = client_socket.recv(64*64)  # 1024 is the buffer size

# Convert the received data back to a 2D array
# array = np.frombuffer(data, dtype=float).reshape(64, 64)
print(data[0][0])
print(data[0][1])
print(data[1][0])
print(data[1][1])

#Visualize array data

# Close the socket
client_socket.close()

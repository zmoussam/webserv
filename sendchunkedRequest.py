import http.client

# Create a connection to the server
conn = http.client.HTTPConnection("localhost")

# Set up headers for chunked transfer
headers = {
    "Content-Type": "application/octet-stream",
    "Transfer-Encoding": "chunked"
}

# Start the request
conn.request("POST", "/path/to/endpoint", headers=headers)

# Send chunks
data_chunks = ["Chunk 1", "Chunk 2", "Final Chunk"]
for chunk in data_chunks:
    chunk_size = hex(len(chunk))[2:]  # Convert size to hexadecimal
    chunk_data = f"{chunk_size}\r\n{chunk}\r\n"
    conn.send(chunk_data.encode())

# Send the last, empty chunk to indicate the end of the data
conn.send(b"0\r\n\r\n")

# Get the response
response = conn.getresponse()
print(response.read())

# Close the connection
conn.close()
import socket
import threading

def start_tcp_server(port):
    tcp_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    tcp_server.bind(('127.0.0.1', port))
    tcp_server.listen(5)
    print(f"TCP server listening on port {port}")
    
    while True:
        client_socket, addr = tcp_server.accept()
        print(f"Received TCP connection from {addr}")
        data = client_socket.recv(1024)
        print(f"Received TCP data: {data.decode()}")
        client_socket.close()

def start_udp_server(port):
    udp_server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_server.bind(('127.0.0.1', port))
    print(f"UDP server listening on port {port}")
    
    while True:
        data, addr = udp_server.recvfrom(1024)
        print(f"Received UDP data from {addr}: {data.decode()}")

if __name__ == "__main__":
    port = 8080
    threading.Thread(target=start_tcp_server, args=(port,)).start()
    threading.Thread(target=start_udp_server, args=(port,)).start()

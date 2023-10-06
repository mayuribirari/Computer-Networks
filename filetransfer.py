import socket
from typing import BinaryIO

def file_server(iface: str, port: int, use_udp: bool, fp: BinaryIO) -> None:
    print("Hello, I am a server")

    if use_udp:
        udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    else:
        tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    main_socket = udp_socket if use_udp else tcp_socket
    main_socket.bind((iface, port))

    if not use_udp:
        main_socket.listen(10)
        client_connection, client_address = main_socket.accept()

    while True:
        if use_udp:
            packet, sender_address = main_socket.recvfrom(256)
            if len(packet) == 0:
                break
            fp.write(packet)
        else:
            block = client_connection.recv(256)
            if len(block) == 0:
                break
            fp.write(block)

    fp.close()
    main_socket.close()
    exit(0)

def file_client(host: str, port: int, use_udp: bool, fp: BinaryIO) -> None:
    print("Hello, I am a client")

    if host == "localhost":
        host = "127.0.0.1"

    if use_udp:
        client_udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    else:
        client_tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_tcp_socket.connect((host, port))

    active_socket = client_udp_socket if use_udp else client_tcp_socket

    while True:
        segment = fp.read(256)
        if not segment:
            if use_udp:
                active_socket.sendto(b'', (host, port))
            else:
                active_socket.send(b'')
            break
        if use_udp:
            active_socket.sendto(segment, (host, port))
        else:
            active_socket.send(segment)

    fp.close()
    active_socket.close()
    exit(0)

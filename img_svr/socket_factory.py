import socket


def create_socket(host, port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    open_address = (host, port)
    sock.bind(open_address)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 8388608)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.listen(20)
    print 'Socket now listening on {}:{}'.format(host, port)
    return sock

# -*- coding:utf-8 -*-

import socket
import sys


host = socket.gethostbyname(socket.gethostname())
print host
port = 8889
s = socket.socket()
s.bind((host, port))
s.listen(20)

DATA = ''
count = 0
while True:
    clnt,addr = s.accept()
    print 'client address:', addr
    while True:
        data = clnt.recv(1024000)
        if not data: continue
        if data == 'start':
            print('start')
            DATA = ''
            count = 0
            clnt.sendall("ok...start")
        elif data == 'stop':
            print('stop')
            f = open(r'D:\tempdata\server.nii.gz', 'wb')
            f.write(DATA)
            f.close()
            clnt.sendall("ok...stop")
        else:
            DATA += data
            count += 1
            print('Count {}, total data size {}'.format(count, sys.getsizeof(DATA)))
            clnt.sendall("data {}".format(count))

clnt.close()

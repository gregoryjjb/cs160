import socket
import os

def Main():
	host = "127.0.0.1"
	port = 5000
	os.system("g++ main.cpp -o main")
	
	sock = socket.socket()
	sock.bind((host,port))
	sock.listen(1)
	print "Server started..."
	
	con, addr = sock.accept()
	print "Client connected ip<" + str(addr) + ">"
	
	while True:
		messageIn = con.recv(1024)
		if not messageIn:
			break
		if messageIn == 'y':
			# run cpp file
			os.system("./main")
			con.send("cpp file executed")
		else:
			con.send("file not executed")
	sock.close()
	
if __name__ == "__main__":
	Main()

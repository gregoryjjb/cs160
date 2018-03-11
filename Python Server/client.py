import socket

def Main():
	host = '127.0.0.1'
	port = 5000
	
	sock = socket.socket()
	sock.connect((host, port))
	print "Server found"
	
	messageOut = raw_input("Run cpp file (y/n/q)?: ")
	
	while messageOut != "q":
		sock.send(messageOut)
		messageIn = sock.recv(1024)
		print "From the server: " + str(messageIn)
		messageOut = raw_input("Run cpp file (y/n/q)?: ")
	sock.close()	
	
if __name__ == "__main__":
	Main()

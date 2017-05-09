import socket
import sys
import time
import Global_variables as G

getaddrm = 'getaddr'
getpeerm = 'getpeer'
pollm = 'poll'
getconm = 'getcon'
endm = 'e'

separator = '_'

maxsize=4096

class Server():
	def __init__(self):
		self.s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		self.s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		self.s.bind(('',G.self_port))
		self.s.connect((G.server_ip, G.server_port))
 
	def recv(self):
                try:
		        msg = self.s.recv(maxsize)
                        if msg != None :
                                return msg
                        return self.recv()
		except socket.error, msg:
	       		print 'Error Code : ' + str(msg[0]) + ' Message ' + msg[1]

	def send (self,msg):
                try:
			self.s.send(msg)
		except socket.error, msg:
	       		print 'Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
			sys.exit()

        def getaddr (self):
                self.s.send(getaddrm)
                return self.recv()
        
        def getpeer (self):
                self.s.send(getpeerm)
                return self.recv()


        def poll(self):
                self.send(pollm)
                msg = self.recv()
                duties = msg.split(separator)
                if duties[-1] == endm :
                        del duties[-1]
                return duties

        def getcon(self, addr):
                msg = getconm+ separator + addr
                self.send(msg)
                msg = self.recv()
                #if msg != endm :
                 #       self.getcon(addr)
                return msg
        
	def __del__(self):
		self.s.close()
        
                
if __name__ == "__main__":
        G.self_port = int(sys.argv[1])
	serv=Server()
	while (1) :
		msg = raw_input('Enter message to send : ')
		if msg== 'exit':
			break;
		serv.send(msg);
		data=serv.recv()
		print 'Server reply : ' + data
	del serv

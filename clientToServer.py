import socket
import sys
import Global_variables as G


class Server():
	def __init__(self):
		self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		self.s.bind(('',G.self_port))
		self.s.connect((G.server_ip, G.server_port))
	def send(self,msg):
		try :
	                self.s.send(msg)
	        except socket.error, msg:
	       		print 'Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
			sys.exit()
	def recv(self):
		try :
	                data = self.s.recv(1024)
			return data      
	        except socket.error, msg:
	       		print 'Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
	        	sys.exit()
	def __del__(self):	
		self.s.close()
if __name__ == "__main__":
	serv=Server()
	while (1) :
		msg = raw_input('Enter message to send : ')
		if msg== 'exit':
			break;
		serv.send(msg);
		data=serv.recv()
		print 'Server reply : ' + data
	del serv
	

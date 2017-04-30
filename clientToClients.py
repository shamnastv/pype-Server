import socket
import sys
import time
import Global_variables as G
from collections import deque

maxsize=4096
punchTimeout = 10
nOfIteration = 10
waiting_time = 10

class Client() :
	def __init__(self , addr):
		self.id = addr
		self.s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		self.s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		self.s.bind(('',G.self_port))
		addr = self.stringToTuple(addr)
		self.s.connect(addr)
		self.isPunched = False
		self.s.settimeout(None)
	

	def tupleToString(self, taddr):
		self.ip, self.port = taddr
		return self.ip+':'+str(self.port)	

	def stringToTuple(self,addr):
		self.ip, self.port = addr.split(':')
		return self.ip, int(self.port)
	
	def send(self,msg):
		try:
			self.s.send(msg)
		except socket.error, msg:
	       		print 'Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
			sys.exit()

	def recv(self):
		try:
			msg = self.s.recv(maxsize)
			if msg != 'punched' and msg != 'punch' and msg != None :
				return msg
			elif msg == 'punch' :
				self.send('punched')
				return self.recv()
			else :
				return self.recv()
		except socket.error, msg:
	       		print 'Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
			sys.exit()			

	def punch(self):
		for i in range(nOfIteration) :
			self.send('punch')
			self.s.settimeout(punchTimeout)
			data = ''
			try :
				data = self.s.recv(maxsize)
			except :
                                time.wait(waiting_time)
				continue
			if data == 'punch' :
				print 'received punch\n'
				self.send('punched')
				self.isPunched = True
				self.s.settimeout(None)
				return 1
			if data == 'punched' :
				print 'received punched\n'
				self.isPunched = True
				self.s.settimeout(None)		
				return 1
		self.s.settimeout(None)
		return 0
	
	def __del__(self):
		self.s.close()

if __name__=="__main__":
	#if sys.argc==2:
	G.self_port = int(sys.argv[1])
	clients = []
	while(1):
		print '1. New conn\n2. Communicate with client\n3. Exit'
		op = input('Enter option : ')
		if op == 1 :
			addr = raw_input('Enter address : ')
			clients.append(Client(addr))
		if op == 2 :
			i=0
			for c in clients :
				print str(i) + ':' + c.id + '\n'
				i=i+1 	
			op = input('Enter option : ')
			while 1 :
				print '1. Send \n2. Receive\n3. Punch\n4. Back '
				opt = input('Enter option : ')
				if opt == 1 :
					msg = raw_input('Enter message to send : ')
					clients[op].send(msg)
				if opt == 2 :
					msg = clients[op].recv()
					print 'recieved : ' + str(msg)
				if opt == 3 :
					pr = clients[op].punch()
	 				if pr == 1 :
						print 'punch : success\n'
					elif pr == 0 :
						print 'punch : unsuccess\n'
				if opt == 4 :
					break
		elif op == 3:			
			break	
	del clients

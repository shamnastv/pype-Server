import sys
from clientToServer_udp2 import Server
from clientToClients import Client
import Global_variables as G


if __name__ == '__main__' :
    G.self_port = int(sys.argv[1])
    if len(sys.argv) == 4 :
        G.server_ip = sys.argv[2]
        G.server_port = int(sys.argv[3])
    clients = []
    while 1 :
        print '1. Connect to Server\n2. Connect to Clients\n3. exit'
        op1 = input('Enter option : ')
        if op1 == 1 :
            serv=Server()
	    while (1) :
                print '1. get my address\n2. get first peer\n3. poll\n4. getconnection\n5. exit'
                op = input('Enter option : ')
                print '\nServer reply'
	        if op == 1:
	            print serv.getaddr()
                elif op == 2 :
                    print serv.getpeer()
                elif op == 3 :
                    print serv.poll()
                elif op == 4:
                    address = raw_input('enter address : ')
                    print serv.getcon(address)
                elif op == 5:
                    break
                print '\n'
	    del serv
        if op1 == 2 :
	    while(1):
	        print '1. New conn\n2. Communicate with client\n3. Exit'
	        op = input('Enter option : ')
	        if op == 1 :
		    addr = raw_input('Enter address : ')
		    clients.append(Client(addr))
                if op == 2 :
		    i=0
		    for c in clients:
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
        if op1 == 3 :
            del clients
            sys.exit()

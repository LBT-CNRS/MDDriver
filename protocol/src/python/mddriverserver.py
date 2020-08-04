
from time import *
from mddriver import *

port=3000
message=2
wait=1
filenameserver="server.log"

mddriver_server(port,wait, message,filenameserver)

print("MDDriver server waiting !!")

tstep=0
while(1) :

	mddriver_probeconnection()
	pos = [(1.0,1.0,1.0),(2.0,2.0,2.0),(2.0,2.0,2.0), (2.0,2.0,2.0) ,(3.0,3.0,3.0)]
	print ("send pos")
	print(pos)
	mddriver_treatprotocol()
	mddriver_sendcoords(pos)

	mddriver_treatprotocol()
	forces=	mddriver_getforces()
	print ("get force")
	print(forces)

	print ("send energies")
	energies = {"tstep":tstep,"T":0.0,"Etot":1.0, "Epot" : 2.0, "Evdw" :3.0,"Eelec" : 4.0,"Eangle" : 5.0,"Edihe" : 6.0,"Eimpr" :7.0}
	print(energies)	
	mddriver_treatprotocol()
	mddriver_sendcurrentenergies(energies)
	sleep(0.01)
	tstep=tstep+1
	
mddriver_terminate	()
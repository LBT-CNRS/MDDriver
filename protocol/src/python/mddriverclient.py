from mddriver import *
from time import *

clienthostname="127.0.0.1"
port=3000
message=2
wait=0
filenameclient="client.log"

while(not mddriver_probeconnection()) :
	mddriver_client(clienthostname,port,message,filenameclient)
	sleep(0.01)
	print("Try to connect !!")


atomspos=[]
while(1) :
	print("MDDriver client connected !!")
	atomsforces={ 0 : (0.0,0.0,1.0), 3 : (2.0,0.0,1.0), 4 : (0.0,0.0,5.0)}
	
	print("Get Pos")
	print(atomspos)
	mddriver_treatprotocol()
	atomspos=mddriver_getcoords()
	
	print("Send Forces")
	print(atomsforces)
	mddriver_treatprotocol()
	mddriver_sendforces(atomsforces)
	
	print("Get Energies")
	mddriver_treatprotocol()
	energies=mddriver_getenergies(atomsforces)
	print(energies)

	sleep(0.01)
	
mddriver_terminate	()	
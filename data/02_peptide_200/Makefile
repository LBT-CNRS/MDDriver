MDRUN=gmx mdrun
GROMPP=gmx grompp
PDB2GMX=gmx pdb2gmx

# MDDriver option 
# -imdwait if you want to wait for interaction connection before starting simulation
# -imdport port port 
# -imdterm to allow interaction component to stop simulation
# -imdpull to take into account forces coming from interaction component in simulation  

IMDOPT=-imdwait -imdpull -imdterm -imdport 8888


PDB=model.pdb
MDP=imd.mdp

TOP=model.top
NDX=model.ndx
GRO=model.gro

TPR=model.tpr

OUTPUT=output/imd
 
prep: 
	${PDB2GMX} -f ${PDB} -o ${GRO} -p ${TOP}
	mkdir -p output
	

$(TPR): prep $(TOP) $(GRO) $(MDP)
	$(GROMPP) -v -f $(MDP) -c $(GRO) -p $(TOP) -o $@


run: $(TPR)
	 $(MDRUN)  -v -s $(TPR) -x $(OUTPUT) -deffnm $(OUTPUT) $(IMDOPT) 

clean:
	rm -rf $(TPR) $(TOP) ${GRO} $(OUTPUT) ${MDDRIVERLOG} mdout.mdp \#*\# gmximd.* posre.itp

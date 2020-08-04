MPIRUN=mpirun
MDDRIVER=mdrun_mpi
GROMPP=grompp_mpi
PDB2GMX=pdb2gmx_mpi

# MDDriver option 
# -IMDfreq nb number of simulation iteration steps between sending
# -IMDwait 0 or 1, if the simulation server has to wait for client connection before starting
# -IMDport port port 
# -IMDmsg loglevel 0 no log - 2 log ()  

MDDRIVERLOG=test.log

IMDOPT=-IMDfreq 1 -IMDwait 1 -IMDport 3000 -IMDmsg 0 -IMDlog ${MDDRIVERLOG}



PDB=model.pdb
MDP=imd.mdp

TOP=model.top
NDX=model.ndx
GRO=model.gro

TPR=model.tpr

OUTPUT=output

NBPROCS=4
 
prep: 
	${PDB2GMX} -f ${PDB} -o ${GRO} -p ${TOP}
	mkdir -p output
	

$(TPR): prep $(TOP) $(GRO) $(MDP)
	$(GROMPP) -v -f $(MDP) -c $(GRO) -p $(TOP) -o $@


run: $(TPR)
	 $(MPIRUN) -np $(NBPROCS) $(MDDRIVER) -v $(IMDOPT) -np $(NPROCS) -s $(TPR) -x $(OUTPUT) -deffnm $(OUTPUT)


run_no_mpi:	$(TPR)
	$(MDDRIVER) -v $(IMDOPT) -s $(TPR) -x $(OUTPUT) -deffnm $(OUTPUT)
clean:
	rm -rf $(TPR) $(TOP) ${GRO} ${GRO} $(OUTPUT) ${MDDRIVERLOG} mdout.mdp \#*\# gmximd.* posre.itp

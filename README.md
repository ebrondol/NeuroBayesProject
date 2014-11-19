=================
NeuroBayesProject
=================

NeuroBayes MVA for the Higgs to tau tau analysis

=================
Setting NeuroBayes tools:
	https://twiki.cern.ch/twiki/bin/view/Main/NeuroBayes
	TMVA interface
	https://github.com/sroecker/tmva-neurobayes/blob/master/README
*** on the .tcshrc
 setenv ROOTSYS /afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.09/x86_64-slc5-gcc46-opt/root
 source /afs/cern.ch/sw/lcg/external/gcc/4.6.2/x86_64-slc6-gcc46-opt/setup.csh
 source $ROOTSYS/bin/thisroot.csh $ROOTSYS
 setenv PHIT_LICENCE_SERVER lcgapp09:16820
 setenv NEUROBAYES /afs/cern.ch/sw/lcg/external/neurobayes/3.16.0/x86_64-slc6-gcc44-opt
 setenv LD_LIBRARY_PATH ~/private/tmva-neurobayes-master:$NEUROBAYES/lib:$ROOTSYS/lib:$LD_LIBRARY_PATH
 setenv PATH $NEUROBAYES/external:$PATH

=================
Description of the code:
	This code run a ROOT script (run_plot.C) which execute the train (work_scripts/nb_train.C) and the test (work_scripts/nb_test.C) of the NB MVA.



=================
Running code:
	root run_plot.C

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

work_scripts/nb_train.C:
	1. Create a TMVA::Factory ::
		this allows the communication of the user with the data sets and the MVA methods (which can be choosen later). 
		The first argument is the base of the name of all the weightfiles in the directory weight/i.
		The second argument is the output file for the training results.
		The third is a list of options.
	2. Adding Variables :: 
	   A variable can be added to the MVA also like a Spectator, which means that is part of the input data set, 
	   but is not used in the MVA training, test nor during the evaluation. These are just copied into the TestTree.
	   That can be useful for correlation tests or others.
	3. Loading Sig and Bkg Trees
	4. Setting weights ::
	   This would set individual event weights (the variables defined in theexpression need to exist in the original TTree)
	5. Setting the preparation ::
	   The input events that are handed to the Factory are internally copied and split into one training and one test ROOT tree (Options in pg. 22). 
	   It is possible to apply selection requirements (TCuts) upon the input events.
	6. Book MVA Neurobayes method 
	7. Training for classification


=================
Running code:
	root run_plot.C

=================
Booking different method:
please lookup the various method configuration options in the corresponding cxx files, eg:src/MethoCuts.cxx, etc, or here: http://tmva.sourceforge.net/optionRef.html


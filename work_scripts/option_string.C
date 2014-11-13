// This file creates the option string to hand over to nb_train.C
// It can be used to try different options of NeuroBayes
// This version can be used for automatisation purposes to, if something goes wrong a backup is saved under backup_option_string.C
#include <iostream>
#include <fstream>
#include <sstream>
#include "TString.h"

TString option_string ( TString variablesfile, TString stringfile )
{
  // Opening option-files
  ifstream flags (variablesfile);
  if (!flags.is_open()) { 
    std::cerr << "Unable to open option file " << variablesfile << std::endl;
    exit(1);
  };
  string readout;
  unsigned int nline(1), NVAR(0);

  ifstream sfile (stringfile);
  if (!flags.is_open()) { 
    std::cerr << "Unable to open option file " << stringfile << std::endl;
    exit(1);
  };

   // Specifying Options ----------------------------------------------------------------------------------------

   // Set individual preprocessing flags in a coma seperated string, e.g. 12,12,12,12. See HowTo p.28ff
   TString NBIndiPreproFlagList="";
   for (nline; nline<=5; ++nline) getline(flags, readout);
   if ( readout[0]=='#' || readout[0]=='\0' ) {
     std::cerr << "In option_string.C: No number of variables specified in line 5." << std::endl;
     std::cerr << "Check " << flags << std::endl;
   } else {
     NVAR = atoi(readout.c_str());
   }
   for ( int it(1); it<=NVAR; ++it ) {
     for (nline; nline<=(10*it+1); ++nline) getline( flags, readout); // Go to line ##1 for the boolean
     if ( not ( readout[0]=='0' || readout[0]=='1' ) ) {
       std::cerr << "In option_string.C: Inputline " << 10*it+1 << " is " << readout << " and not a boolean." << std::endl;
       std::cerr << "Check " << flags << std::endl;
       break;
     } else {
       if ( readout[0]=='#' || readout[0]=='\0' ) {
	 std::cerr << "In option_string.C: No PreproFlag specified in inputline number " << nline << std::endl;
       } else if ( readout[0]=='1' ) {
       for ( nline; nline<=(10*it+8); ++nline ) getline( flags, readout); // Read in line ##8 for preproflag
       if (NBIndiPreproFlagList!="") NBIndiPreproFlagList+=",";
       NBIndiPreproFlagList += readout;
       }
     }
   }


   // ----------------------------------------------------------------------------------------------------------

 
  // Set options String with the Options defined in the stringfile
   TString options = "!H:V";
   string a, b;
   while ( std::getline( sfile, a ) ){
     std::stringstream line (a, ios_base::in);
     if ( line >> a >> b ) {
       if ( a[0] == '#' ) {
       } else if ( a == "Analysis" ) {
	 if ( b != "true" ) options +=":!Analysis";
	 if ( b == "true" ) options +=":Analysis";
       } else if ( a == "Boost_num" ) {
	 options +=(":Boost_num=" + b);
       } else if ( a == "CreateMVAPdfs" ) {
	 options += ":CreateMVAPdfs";
       } else if ( a == "NTrainingIter" ) {
	 options +=(":NTrainingIter=" + b);
       } else if ( a == "Preprocessing" ) {
	 options +=(":Preprocessing=" + b);
       } else if ( a == "NBPreproString" ) {
	 options +=(":NBIndiPreproFlagByVarname=" + b);
       } else if ( a == "VarTransform" ) {
	 options +=(":VarTransform=" + b);
       } else if ( a == "IgnoreNegWeightsInTraining" ) {
	 if ( b=="true"  ) {
	   options += ":IgnoreNegWeightsInTraining"; 
	 } else if ( b!="false"  ) {
	   std::cerr << "Error in optionfile, IgnoreNegWeights has to be set to true or false" << std::endl;
	 }
       } else if ( a == "Regularisation" ) {
	 options +=(":Regularisation=" + b);
       } else if ( a == "WeightUpdate" ) {
	 options +=(":WeightUpdate=" + b);
       } else if ( a == "Momentum" ) {
	 options +=(":Momentum=" + b);
       } else if ( a == "ShapeTreat" ) {
	 options +=(":ShapeTreat=" + b);
       } else if ( a == "LossFunc" ) {
	 options +=(":LossFunc=" + b);
       } else if ( a == "LearningSpeed" ) {
	 options +=(":LearingSpeed=" + b);
       } else if ( a == "LimitLearningSpeed" ) {
	 options +=(":LimitLearingSpeed=" + b);
       } else if ( a == "TrainingMethod" ) {
	 options +=(":TrainingMethod=" + b);
       } else {
	 std::cerr << "Error in option file: " << a << " " << b << " couldn't be interpreted." << std::endl;
       }
     }
   }

   options += (":NBIndiPreproFlagList=" + NBIndiPreproFlagList);

   return options;
}

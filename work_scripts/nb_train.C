/**********************************************************************************
 * First alonestanding training macro for Neurobayes                              *
 **********************************************************************************/

#include <cstdlib>
#include <fstream> 
#include <map>
#include <string>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TPluginManager.h"

#if not defined(__CINT__) || defined(__MAKECINT__)
// needs to be included when makecint runs (ACLIC)
#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#endif

// directory of the Inputfiles
const TString dir = "/afs/cern.ch/user/e/ebrondol/nb1/Input/"; 

void nb_train ( TString options, TString nbname = "NeuroBayes", TString optionfile = "optionfiles/variables1.opt" )
{
   // this loads the library
  //   TMVA::Tools::Instance();  // Is done in the program, which loads the NeuroBayes-Plugin

   // ---------------------------------------------------------------

   std::cout << std::endl;
   std::cout << "==> Start TMVATraining" << std::endl;


   // Create a new root output file.
   TString outfileName( "TMVA.root" );
   TFile* outputFile = TFile::Open( outfileName, "RECREATE" );
   // Load options file(s)
   ifstream varoptions ( optionfile );
   if (!varoptions.is_open()) { 
     std::cerr << "Unable to open option file " << optionfile << std::endl;
     exit(1);
   };

  //-----
  // Create the Factory

  //ERICA :: LOOK into Transformations OPTION!!
  TMVA::Factory *factory = new TMVA::Factory("TMVAClassification", 
					     outputFile, 
                                             "!V:!Silent:Color:!DrawProgressBar" ); 
                                           
  //-----
  // Define the NVAR input variables 

  string varon, varname, varshort, varunit, vartype;
   unsigned int NVAR=0, nline(1);
   // Get NVAR
   for (nline; nline<=5; ++nline) getline( varoptions, varon);
   if ( varon[0]=='#' || varon[0]=='\0' ) {
     std::cerr << "In nb_train.C: No number of variables specified in line 5." << std::endl;
     std::cerr << "Check " << optionfile << std::endl;
   } else {
     NVAR = std::atoi(varon.c_str());
   }

   //-----
   // Get Variables : used in MVA or spectators

   for (unsigned int it(1); it<=NVAR; ++it ) {
     for (nline; nline<=(10*it+1); ++nline) getline( varoptions, varon); // Go to line ##1 for the boolean
     if ( not ( varon[0]=='0' || varon[0]=='1' ) ) {
       std::cerr << "In nb_train.C: Line " << 10*it+1 << " is " << varon << " and not a boolean." << std::endl;
       std::cerr << "Check " << optionfile << std::endl;
       break;
     } else {
       getline( varoptions, varname); ++nline; // Read in line ##2
       if ( varon[0]=='0' ) {
	 getline( varoptions, varshort ); ++nline; // Read in line ##3
	 getline( varoptions, varunit );  ++nline; // Read in line ##4
	 getline( varoptions, vartype );  ++nline; // Read in line ##5
         factory->AddSpectator( varname,  varshort, varunit, vartype[0] );
	 std::cout << "--- TMVA Training: Variable " << varname << " is spectator variable :: NOT used for training." << std::endl;
	 std::cout << "--- TMVA Training: Bool " << varon << ", short " << varshort << ", unit " << varunit << ", type " << vartype << std::endl;
       } else {
	 getline( varoptions, varshort ); ++nline; // Read in line ##3
	 getline( varoptions, varunit );  ++nline; // Read in line ##4
	 getline( varoptions, vartype );  ++nline; // Read in line ##5
	 factory->AddVariable( varname, varshort, varunit, vartype[0] );
	 std::cout << "--- TMVA Training: Variable " << it << ": " << varname << endl;
	 std::cout << "--- TMVA Training: Bool " << varon << ", short " << varshort << ", unit " << varunit << ", type " << vartype << std::endl;
       }
     }

  }


  //-----
  //Load the signal and background event samples from ROOT trees

  std::vector<TString> SignalFiles;
  std::vector<TString> BkgFiles;

  SignalFiles.push_back(dir+"train_mvain_mu_sync_vbfhiggs_0.root");
  //  SignalFiles.push_back(dir+"train_mvain_mu_sync_vbfhiggs_norecoil_0.root"); // Check if same as in nb_test.C

  BkgFiles.push_back(dir+"train_mvain_mu_sync_dy1j_0.root");
  BkgFiles.push_back(dir+"train_mvain_mu_sync_dy2j_0.root");
  BkgFiles.push_back(dir+"train_mvain_mu_sync_dy3j_0.root");
  BkgFiles.push_back(dir+"train_mvain_mu_sync_dy4j_0.root");


  std::cout << "Input files:" << std::endl;
  TChain* ntu_Sig = new TChain("TauCheck");
  for(unsigned int i = 0; i < SignalFiles.size(); i++){
    ntu_Sig -> Add(SignalFiles.at(i));
    std::cout << "\t" << SignalFiles.at(i) << " added at the Sig chain." << std::endl;
  }
  TChain* ntu_Bkg = new TChain("TauCheck");
  for(unsigned int i = 0; i < BkgFiles.size(); i++){
    ntu_Bkg -> Add(BkgFiles.at(i));
    std::cout << "\t" << BkgFiles.at(i) << " added at the Bkg chain." << std::endl;
  }

  if( ntu_Sig->GetEntries() == 0 || ntu_Bkg->GetEntries() == 0 )
  {
    std::cout << "Error!! at least one file is empty" << std::endl;
    return;
  }

  factory->AddSignalTree( ntu_Sig, 1., TMVA::Types::kTraining );
  factory->AddBackgroundTree( ntu_Bkg, 1., TMVA::Types::kTraining );


  //-----
  //Setting weights and TCuts

  factory->SetWeightExpression( "lumiWeight*weight*splitFactor" );
  //factory->SetBackgroundWeightExpression("weight");

  TCut presel = "nbtag==0"; // What does this do?

  std::cout << "--- TMVA Training: PrepareTrainingAndTestTree" << std::endl;
  factory->PrepareTrainingAndTestTree( presel, "nTrain_Signal=0:nTrain_Background=0:MixMode=Random:NormMode=NumEvents:!V" ); 
  // Could think about trying different NormMode ...


  //-----
  //Book NeuroBayes MVA with specified option string

  std::cout << "--- TMVA Training: BookMethod" << std::endl;
  factory->BookMethod( TMVA::Types::kPlugins, nbname, options );

  //-----
  // Train MVAs using the set of training events

  std::cout << "--- TMVA Training: TrainAllMethodsForClassification" << std::endl;
  //ERICA :: why not factory->TrainAllMethods();
  factory->TrainAllMethodsForClassification();
  std::cout << "--- TMVA Training: done!" << std::endl;      

  outputFile->Close();
  delete factory;

}

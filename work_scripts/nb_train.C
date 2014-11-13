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

   // Create the factory object. Later you can choose the methods
   // The first argument is the base of the name of all the
   // weightfiles in the directory weight/ 
   // The second argument is the output file for the training results
   TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", outputFile, 
                                               "!V:!Silent:Color:!DrawProgressBar" ); // Removed Transformations
                                               //"!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D" );
   // -----------------------------------------------------------------------

   // Define the input variables that shall be used for the MVA training
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

   // Get Variables
   for ( int it(1); it<=NVAR; ++it ) {
     for (nline; nline<=(10*it+1); ++nline) getline( varoptions, varon); // Go to line ##1 for the boolean
     if ( not ( varon[0]=='0' || varon[0]=='1' ) ) {
       std::cerr << "In nb_train.C: Line " << 10*it+1 << " is " << varon << " and not a boolean." << std::endl;
       std::cerr << "Check " << optionfile << std::endl;
       break;
     } else {
       getline( varoptions, varname); ++nline; // Read in line ##2
       if ( varon[0]=='0' ) {
	 std::cout << "--- TMVA Training: Variable " << varname << " is not used for training." << std::endl;
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

   // You can add so-called "Spectator variables", which are not used in the MVA training, 
  //   factory->AddSpectator( "spec2:=var1*3",  "Spectator 2", "units", 'F' );

   //-------------------------------------------------------------------------------------------------

   // load the signal and background event samples from ROOT trees
  std::vector<TString> SignalFiles;
  std::vector<TString> BkgFiles;
  const TString dir = "/afs/cern.ch/user/e/ebrondol/nb1/Input/"; // directory of the Inputfiles
  // specify signal files
  SignalFiles.push_back(dir+"train_mvain_mu_sync_vbfhiggs_0.root");
  //  SignalFiles.push_back(dir+"train_mvain_mu_sync_vbfhiggs_norecoil_0.root"); // Check if same as in nb_test.C
  // background files
  BkgFiles.push_back(dir+"train_mvain_mu_sync_dy1j_0.root");
  BkgFiles.push_back(dir+"train_mvain_mu_sync_dy2j_0.root");
  BkgFiles.push_back(dir+"train_mvain_mu_sync_dy3j_0.root");
  BkgFiles.push_back(dir+"train_mvain_mu_sync_dy4j_0.root");

  // Create Vector of Inputfiles
  std::vector<TFile*> Input;
  size_t IterInput(0);

  // Fill in Signalfiles
  for (IterInput; IterInput < SignalFiles.size(); ++IterInput) {
    TFile* tf1 = TFile::Open( SignalFiles.at(IterInput) ); // at() returns a reference on the element i
    Input.push_back( tf1 );
    if ( gSystem->AccessPathName( SignalFiles.at(IterInput) ) ) { // file does not exist in local directory
      std::cout << "Input File " << Input.at(IterInput)->GetName() << " does not exist." << std::endl;
      return;
    }
    std::cout << "--- TMVA Training: Using Input File " << Input.at(IterInput)->GetName() << " as signal." << std::endl;
    // Give Tree to factory
    TTree* tt1   = (TTree*)Input.at(IterInput)->Get("TauCheck");
    factory->AddSignalTree( tt1, 1., TMVA::Types::kTraining );
  }
  --IterInput; // because gets increased one time too often in the last loop

  // Fill in Backgroundfiles
  for (size_t i(0); i<BkgFiles.size(); ++i) {
    TFile* tf1 = TFile::Open( BkgFiles.at(i) ); // at() returns a reference on the element i
    Input.push_back( tf1 );
    IterInput++;
    if ( gSystem->AccessPathName( BkgFiles.at(i) ) ) {
      std::cout << "Input File " << Input.at(IterInput)->GetName() << " does not exist." << std::endl;
      return;
    }
    std::cout << "--- TMVA Training: Using Input File " << Input.at(IterInput)->GetName() << " as background." << std::endl;
  // register trees
    TTree* tt1   = (TTree*)Input.at(IterInput)->Get("TauCheck");
    factory->AddBackgroundTree( tt1, 1., TMVA::Types::kTraining );
  }

  // -------------------------------------------------------------------------------------------------------------------------

   // This would set individual event weights (the variables defined in the 
   // expression need to exist in the original TTree)
  factory->SetWeightExpression( "lumiWeight*weight*splitFactor" );
  //   factory->SetBackgroundWeightExpression("weight");

   // Apply additional cuts on the (signal and background) samples (can be different)
   TCut presel = "nbtag==0"; // What does this do?

   // tell the factory to use all remaining events in the trees after training for testing:
   std::cout << "--- TMVA Training: PrepareTrainingAndTestTree" << std::endl;
   factory->PrepareTrainingAndTestTree( presel,
   "nTrain_Signal=0:nTrain_Background=0:MixMode=Random:NormMode=NumEvents:!V" ); 
   // Could think about trying different NormMode
   //                                      nTrain_Signal=0 takes all signal events for Training
   // If no numbers of events are given, half of the events in the tree are used for training, and 
   // the other half for testing:
   //    factory->PrepareTrainingAndTestTree( mycut, "SplitMode=random:!V" );  
   // To also specify the number of testing events, use:
   //    factory->PrepareTrainingAndTestTree( mycut, 
   //                                         "NSigTrain=3000:NBkgTrain=3000:NSigTest=3000:NBkgTest=3000:SplitMode=Random:!V" );  

   // ---- Book MVA methods ---------------------------------------------------------------
   //
   // please lookup the various method configuration options in the corresponding cxx files, eg:
   // src/MethoCuts.cxx, etc, or here: http://tmva.sourceforge.net/optionRef.html

   /*  ----------------------- (Un)comment for comparision -----------------------------
   // Cut optimisation
   factory->BookMethod( TMVA::Types::kCuts, "Cuts", 
                           "!H:!V:FitMethod=MC:EffSel:SampleSize=200000:VarProp=FSmart" );

   // Boosted Fisher discriminant   
   factory->BookMethod( TMVA::Types::kFisher, "BoostedFisher",
			   "!H:!V:Boost_Num=20:Boost_Transform=log:Boost_Type=AdaBoost:Boost_AdaBoostBeta=0.2");

   // Boosted Decisision Tree
   factory->BookMethod( TMVA::Types::kBDT, "BDT", 
                           "!H:!V:NTrees=400:nEventsMin=400:MaxDepth=3:BoostType=AdaBoost:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning" );
   */


   // NeuroBayes ================================================================================================

   // book Neurobayes with specified option string
   std::cout << "--- TMVA Training: BookMethod" << std::endl;
   factory->BookMethod( TMVA::Types::kPlugins, nbname, options );
   //==================================================================================================

   std::cout << "Start here" << std::endl;
   // Train MVAs using the set of training events
   factory->TrainAllMethodsForClassification();
   std::cout << "==> TMVA Training is done!" << std::endl;      

   for ( size_t i(0); i<Input.size(); ++i ) Input.at(i)->Close();
   delete factory;

}

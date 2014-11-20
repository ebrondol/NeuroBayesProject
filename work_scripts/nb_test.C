//#include <cstdlib>
#include <fstream>
#include <vector>
//#include <iostream>
//#include <map>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TSystem.h"
//#include "TROOT.h"
//#include "TStopwatch.h"

#if not defined(__CINT__) || defined(__MAKECINT__)
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"
#endif

// directory of the Inputfiles
const TString dir = "/afs/cern.ch/user/e/ebrondol/nb1/Input/";

void EvaluateTree( TFile* input, TH1F* histogramm, TMVA::Reader* reader, TString nbname, TString optionfile);

namespace VA {
  // for type declarations/linkings in the TMVATesting framework
  float weight, lumiWeight, splitFactor;
  int nbtag;
  vector<float> varfloat;
}

// Main
void nb_test ( TString nbname="NeuroBayes", TString optionfile="optionfiles/workvar.opt", TString datatype="test" )
{
  //  Library is loaded in another program
  //  TMVA::Tools::Instance();

  std::cout << "==> Start TMVA Testing" << std::endl;

  TH1::SetDefaultSumw2(kTRUE); // All new histograms automatically store the sum of squares of errors

  // --- Create the Reader object ------------------------------------------------
  TMVA::Reader *reader = new TMVA::Reader( ":V:Color:!Silent:!Error" );    


  // Define the input variables that shall be used for the MVA training
  // Load options file(s)
  ifstream varoptions ( optionfile );
  if (!varoptions.is_open()) { 
    std::cerr << "Unable to open option file." << std::endl;
    exit(1);
  };
  string varon, varname;
  unsigned int NVAR=0, nline(1);
  // Get NVAR
  for (nline; nline<=5; ++nline) getline( varoptions, varon);
  if ( varon[0]=='#' || varon[0]=='\0' ) {
    std::cerr << "In nb_test.C: No number of variables specified in line 5." << std::endl;
    std::cerr << "Check " << optionfile << std::endl;
  } else {
    NVAR = atoi(varon.c_str());
  }
  VA::varfloat = vector<float>(NVAR); 
  // Get Variables
  for ( int it(1); it<=NVAR; ++it ) {
    for (nline; nline<=(10*it+1); ++nline) getline( varoptions, varon); // Go to line ##1 for the boolean
    if ( not ( varon[0]=='0' || varon[0]=='1' ) ) {
      std::cerr << "In nb_test.C: Line " << 10*it+1 << " is " << varon << " and not a boolean." << std::endl;
      std::cerr << "Check " << optionfile << std::endl;
      break;
    } else {
      getline( varoptions, varname); ++nline; // Read in line ##2
      if ( varon[0]=='0' ) {
	reader->AddSpectator( varname, &VA::varfloat[it] );
	std::cout << "--- TMVA Testing: Spectator variable " << varname << ":: is NOT used for testing." << std::endl;
      } else {
	reader->AddVariable( varname, &VA::varfloat[it] );
	std::cout << "--- TMVA Testing: Variable " << it << ": " << varname << std::endl;
      }
    }

  }

  // Also possible spectator variables have to be specified

  // --- Book Methods -----------------------------------------------------

  TString nbpath = "weights/TMVAClassification_" + nbname + ".weights.xml";
   // book Neurobayes
  reader->BookMVA( nbname +" method", nbpath );

   // Book output histograms --------------------------------------------------
   TH1F *hist[2];
   hist[0] = new TH1F( nbname+"_B", nbname+"_B", 500, -1.0, 1.0 ); // Background entries
   hist[1] = new TH1F( nbname+"_S", nbname+"_S", 500, -1.0, 1.0 ); // Signal entries

  // ---------------------------------------------------------------------------------------

   // load the signal and background event samples from ROOT trees
  std::vector<TString> SignalFiles;
  std::vector<TString> BkgFiles;
  if ( datatype != "test" && datatype != "train" ) {
    std::cerr << "Datatype should be test or train" << std::endl;
  };
  // specify signal files
  SignalFiles.push_back(dir+datatype+"_mvain_mu_sync_vbfhiggs_0.root");
  //SignalFiles.push_back(dir+"train_mvain_mu_sync_vbfhiggs_norecoil_0.root"); // check if same as in nb_train.C
  SignalFiles.push_back(dir+"test_mvain_mu_sync_ggfhiggs_0.root"); // train_sample is empty
  // background files
  BkgFiles.push_back(dir+datatype+"_mvain_mu_sync_dy1j_0.root");
  BkgFiles.push_back(dir+datatype+"_mvain_mu_sync_dy2j_0.root");
  BkgFiles.push_back(dir+datatype+"_mvain_mu_sync_dy3j_0.root");
  BkgFiles.push_back(dir+datatype+"_mvain_mu_sync_dy4j_0.root");

  // Create Vector of Inputfiles
  std::vector<TFile*> Input; // Can be erased, and only local variables can be used, but do not forget to Close all Opened TFiles
  size_t IterInput(0);

  // Fill in Signalfiles
  for (IterInput; IterInput < SignalFiles.size(); ++IterInput) {
    TFile* tf1 = TFile::Open( SignalFiles.at(IterInput) ); // at() returns a reference on the element i
    Input.push_back( tf1 );
    if ( gSystem->AccessPathName( SignalFiles.at(IterInput) ) ) { // file does not exist in local directory
      std::cout << "Input File " << Input.at(IterInput)->GetName() << " does not exist." << std::endl;
      return;
    }
    std::cout << "--- TMVA Testing: Using Input File " << Input.at(IterInput)->GetName() << " as signal." << std::endl;

    // Evaluate Event Tree
    EvaluateTree( Input[IterInput], hist[1], reader, nbname, optionfile);
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
    std::cout << "--- TMVA Testing: Using Input File " << Input.at(IterInput)->GetName() << " as background." << std::endl;

    // Evaluate Event Tree
    EvaluateTree( Input[IterInput], hist[0], reader, nbname, optionfile);
  }
  // Output is a filled histogramm

  // Write histogramm in a TFile
  TString histoname = "histos/hist_"+ nbname +"_"+ datatype +".root";
  if ( gSystem->AccessPathName("histos") ){ 
    gSystem->MakeDirectory("histos");
    std::cout << "Created directory histos/" << std::endl;
  };
  TFile* output = new TFile( histoname, "RECREATE" );
  hist[0]->Write();
  hist[1]->Write();
  output->Close();

  for ( size_t i(0); i<Input.size(); ++i ){ Input.at(i)->Close(); };
  delete reader;

  std::cout << "Testing done... file " << histoname << " written." << std::endl;
}


// === Functions ========================================================================================================

// This function evaluates all variables in the TTree of a file and fill the outcome into a histogramm
void EvaluateTree( TFile* input, TH1F* histogramm, TMVA::Reader* reader, TString nbname, TString optionfile )
{
  // Variables used as storage of TTree-variables

  // Get and prepare the TTree
  TTree* ttmp = (TTree*)input->Get("TauCheck");

  // Load options file(s)
  ifstream varoptions ( optionfile );
  if (!varoptions.is_open()) { 
    std::cerr << "Unable to open option file." << std::endl;
    exit(1);
  };
  string varon, varname;
  unsigned int NVAR=0, nline(1);
  // Get NVAR
  for (nline; nline<=5; ++nline) getline( varoptions, varon);
  NVAR = atoi(varon.c_str());

  // Get Variables
  for ( int it(1); it<=NVAR; ++it ) {
    for (nline; nline<=(10*it+1); ++nline) getline( varoptions, varon); // Go to line ##1 for the boolean
    if (varon[0]=='1') {
      getline( varoptions, varname); ++nline;
      // Set Address
      ttmp->SetBranchAddress(varname.c_str(), &VA::varfloat[it]);
    }
  }

  ttmp->SetBranchAddress("weight", &VA::weight );
  ttmp->SetBranchAddress("lumiWeight", &VA::lumiWeight );
  ttmp->SetBranchAddress("splitFactor", &VA::splitFactor );
  ttmp->SetBranchAddress("nbtag", &VA::nbtag );

  std::cout << "--- TMVA Testing: Evaluating file: " << input->GetName() << " with " << ttmp->GetEntries() << " events" << std::endl;

  for (Long64_t ievt=0; ievt<ttmp->GetEntries(); ievt++) {

    //       if (ievt%1000 == 0) std::cout << "--- ... Processing event: " << ievt << std::endl;
    ttmp->GetEntry(ievt);

    //Preselection
    if (VA::nbtag>0) continue; // If nbtag>0 start from the beginning with next iterator

    // --- Return the MVA outputs and fill into histograms
    float retval=reader->EvaluateMVA( nbname +" method" );
    histogramm->Fill(retval,VA::weight*VA::lumiWeight*VA::splitFactor);
  }
  //  std::cout << "geht\n";
}
 

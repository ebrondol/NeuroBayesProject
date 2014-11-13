#include <vector>
#include <fstream>
#include <string>

#include "TROOT.h"
#include "TString.h"
#include "TFile.h"
#include "TH1F.h"
#include "TGraphAsymmErrors.h"

// own headers
#include "headers/load_histos.h"
//#include "headers/plot_sb.h"
//#include "TAMS/TAMS.h"

void sh_run ( string variableoptions="optionfiles/workvar.opt", string optionstring="optionfiles/workstring.opt" )
{
  TMVA::Tools::Instance();
  gSystem->Load("libTMVANeuroBayes.so");
  TMVA::MethodNeuroBayes::RegisterNeuroBayes();

  gROOT->LoadMacro("work_scripts/nb_train.C+");
  gROOT->LoadMacro("work_scripts/nb_test.C+");
  gROOT->LoadMacro("work_scripts/option_string.C+");

  string options = option_string( variableoptions, optionstring );
  nb_train( options, "NeuroBayes", variableoptions ); // 
  nb_test("NeuroBayes", variableoptions, "train"); //
  nb_test("NeuroBayes", variableoptions, "test"); //

  //  use this also in the shell-script to access the file (the file is overwritten every new time in this version)
  //  TFile* histfile = TFile::Open("histos/hist_NeuroBayes.root");
  //  std::vector<TH1F*> histos = load_histos (histfile, "NeuroBayes");
  //  histfile->Close()
}

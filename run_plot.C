#include <cmath>
#include <iostream> 
#include <vector>
#include <fstream>
#include <string>

#include "TROOT.h"
#include "TString.h"
#include "TFile.h"
#include "TH1F.h"
#include "TGraphAsymmErrors.h"
#include "TSystem.h"
#include "TMVA/Tools.h"

// own headers
#include <stdio.h>
#include "headers/load_histos.h"
//#include "headers/plot_sb.h"
#include "TAMS/TAMS.h"

//#include "Utilities/ConfigFileLine.h"
//#include "Utilities/ConfigParser.h"

using namespace std;

float fom_plot ( string type, string variableoptions );
int parseConfigFile (const TString& config);

void run_plot ( string variableoptions="workvar.opt", string optionstring="workstring.opt")
{

  TMVA::Tools::Instance();
  gSystem->Load("libTMVANeuroBayes.so");
  TMVA::MethodNeuroBayes::RegisterNeuroBayes();

  gROOT->LoadMacro("work_scripts/plot_variables.C+");
  gROOT->LoadMacro("work_scripts/nb_train.C+");
  gROOT->LoadMacro("work_scripts/nb_test.C+");
  gROOT->LoadMacro("work_scripts/option_string.C+");
//  gROOT->LoadMacro("Utilities/ConfigFileLine.h+");
//  gROOT->LoadMacro("Utilities/ConfigParser.h+");

//  parseConfigFile("cfg/standardConf.cfg");

  variableoptions = ("optionfiles/"+variableoptions);
  optionstring    = ("optionfiles/"+optionstring);
  string options  = option_string( variableoptions, optionstring );

  plot_variables(variableoptions);
  freopen ( "output/run_plot.log", "w", stdout );
  std::cerr << "Redirecting output to output/run_plot.log" << std::endl;
  std::cerr << "Running Training with the following parameters:" << std::endl;
  std::cerr << "\tvariable options: " << variableoptions << std::endl;
  std::cerr << "\toptionstring: " << optionstring << std::endl;
  nb_train( options, "NeuroBayes", variableoptions ); // 

  std::cerr << "Running Analysis and making plot for Training data" << std::endl;
  fom_plot( "train", variableoptions);
  std::cerr << "Running Analysis and making plot for Test data" << std::endl;
  fom_plot( "test", variableoptions);
  
  fclose ( stdout );
  std::cerr << "Closed output/run_plot.log." << std::endl;

  gROOT->ProcessLine(".q"); 
//  return;
}

float fom_plot ( string type, string variableoptions ) {
/*
  std::cerr << "Testing..." << std::endl;
  nb_test("NeuroBayes", variableoptions, type);
  float bef, aft;

  string filename("histos/hist_NeuroBayes_"+type+".root");
  TFile* histfile = TFile::Open(filename.c_str());
  std::vector<TH1F*> histos = load_histos (histfile, "NeuroBayes", true);

  float sys = 0.2; // systematic error
  TAMS ta (histos[0], histos[1], sys);
  ta.calc();
  bef = ta.ams_syst_stat();
  string filename ("output/pdf_befbin_" + type + ".png" );
  std::cerr << "The Figure of Merit before rebinning is: " << bef << std::endl;
  ta.savePlot( filename );
  std::cerr << "The PDF-plot is saved in " << filename << std::endl;

  ta.rebin();
  ta.calc();
  aft = ta.ams_syst_stat();
  filename="output/pdf_aftbin_" + type + ".png";
  std::cerr << "The Figure of Merit after rebinning is: " << aft << std::endl;
  ta.savePlot( filename );
  std::cerr << "The PDF-plot is saved in " << filename << std::endl;
  
  histfile->Close();
  return aft;
*/
}

/* TENTATIVO FALLITO DI PRENDERE DA UN cfg FILE ESTERNO */
//#include "Utilities/ConfigParser.h"

int parseConfigFile (const TString& config)
{
  std::cout << ">>> Parsing " << config << " file" << std::endl ;
  ConfigParser* gConfigParser = new ConfigParser();

  if( gConfigParser ) delete gConfigParser;
  gConfigParser = new ConfigParser();

  if( !(gConfigParser -> init(config)) )
  {
    std::cout << ">>> parseConfigFile::Could not open configuration file "
              << config << std::endl;
     return -1;
  }

  //gConfigParser -> print();
  
    return 0 ;
}
  

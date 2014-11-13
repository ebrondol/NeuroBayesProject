#ifndef LOAD_HISTOS
#define LOAD_HISTOS

#include <vector>
#include <iostream>
#include <string>

//#include "TROOT.h"
#include "TFile.h"
#include "TH1F.h"

// loads Signal Histogram in [0] and background in [1]
std::vector<TH1F*> load_histos (TFile* histfile, std::string nbname="NeuroBayes", bool Silent = false)
{
  // Load histrograms from TFile
  vector<TH1F*> histos;
  std::string b(nbname+"_B");
  std::string s(nbname+"_S");
  histos.push_back( (TH1F*)histfile->Get(s.c_str()) );
  histos.push_back( (TH1F*)histfile->Get(b.c_str()) );
  if (!Silent)  std::cout << "Loaded Histograms " << s << " and " << b << std::endl;

  return histos;
}

#endif // LOAD_HISTOS

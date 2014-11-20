//#include <cstdlib>
#include <fstream>
#include <vector>
//#include <iostream>
#include <map>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TSystem.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TLegend.h"

#if not defined(__CINT__) || defined(__MAKECINT__)
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"
#endif

// directory of the Inputfiles
const TString dir = "/afs/cern.ch/user/e/ebrondol/nb1/Input/";

//outputFile info
TFile* outFile;
std::string extension = "pdf";

//histos maps
std::map<std::string,TH1F*> h_Sig;
std::map<std::string,TH1F*> h_Bkg;

void DrawVar(const std::string& category, const int& rebin, const std::string& outFileName);

TH1F* ratioHisto(TH1F* h_num, TH1F* h_den);

namespace VA {
  // for type declarations/linkings in the TMVATesting framework
  float weight, lumiWeight, splitFactor;
  int nbtag;
  vector<float> varfloat;
}

void plot_variables ( TString optionfile="optionfiles/workvar.opt", bool plotAllVariables = false )
{

  std::cout << "==> Plotting Variables" << std::endl;

  TH1::SetDefaultSumw2(kTRUE); // All new histograms automatically store the sum of squares of errors

  //-----  
  // Load input variables options file(s)
  fstream namefile;
  namefile.open(optionfile, ios::in);
  if(!namefile.good())
    std::cout << "FILE OPENING ERROR!!" << endl;
  
  ifstream varoptions ( optionfile );
  if (!varoptions.is_open()) { 
    std::cerr << "Unable to open option file." << std::endl;
    exit(1);
  };

  //-----
  //Get Variables Total Number

  vector< pair<int,string> > StatusAndNameVar;
  string buffer_line;
  unsigned int NVAR = 0, nline(1);

  //Go to line ##5 for the total number
  for ( nline; nline<=5; ++nline){
    getline( varoptions, buffer_line);
  }

  if ( buffer_line[0]=='#' || buffer_line[0]=='\0' ) {
    std::cerr << "No number of variables specified in line 5." << std::endl;
    std::cerr << "Check " << optionfile << std::endl;
  } else {
    NVAR = atoi(buffer_line.c_str());
    std::cout << "Plotting of " << NVAR << " variables:" << std::endl;
  }


  //-----
  // Get Variables Value
  string buffer_lineB, buffer_lineN;

  for (unsigned int it = 1; it <= NVAR; ++it ) {

    //Go to line ##(10*it+1) for the boolean
    for (nline; nline<=(10*it+1); ++nline) {
      getline( varoptions, buffer_lineB);
    }

    //Go to line ##(10*it+2) for the name
    for (nline; nline<=(10*it+2); ++nline) {
      getline( varoptions, buffer_lineN); 
    }


    if( plotAllVariables ){
      StatusAndNameVar.push_back(make_pair(atoi(buffer_lineB.c_str()), buffer_lineN));
      std::cout << "\tVariable " << StatusAndNameVar.back().second << " is plotted." << std::endl;
    } else {
      if(buffer_lineB[0] == '1' ){
        StatusAndNameVar.push_back(make_pair(atoi(buffer_lineB.c_str()), buffer_lineN));
        std::cout << "\tVariable " << StatusAndNameVar.back().second << " is plotted." << std::endl;
      }
    }

  }

  //-----
  // Get signal and background event samples

  std::vector<TString> SignalFiles;
  std::vector<TString> BkgFiles;
  //ERICA :: nothing, test o train ?
  SignalFiles.push_back(dir+"test_mvain_mu_sync_vbfhiggs_0.root");
  SignalFiles.push_back(dir+"test_mvain_mu_sync_ggfhiggs_0.root");
  BkgFiles.push_back(dir+"test_mvain_mu_sync_dy1j_0.root");
  BkgFiles.push_back(dir+"test_mvain_mu_sync_dy2j_0.root");
  BkgFiles.push_back(dir+"test_mvain_mu_sync_dy3j_0.root");
  BkgFiles.push_back(dir+"test_mvain_mu_sync_dy4j_0.root");

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

  //-----
  // Get Variables

  VA::varfloat = vector<float>(NVAR); 
  for(unsigned int it = 0; it < StatusAndNameVar.size(); ++it ) {
    char * S = new char[StatusAndNameVar.at(it).second.length() + 1];  
    std::copy(StatusAndNameVar.at(it).second.begin(), StatusAndNameVar.at(it).second.end(), S);
    S[StatusAndNameVar.at(it).second.size()] = '\0';
    ntu_Sig->SetBranchAddress(S, &VA::varfloat[it]);
    ntu_Bkg->SetBranchAddress(S, &VA::varfloat[it]);
  }
 
  //-----                    
  //Creation of histos: max and min to be fixed
  
  int nEntries_Sig = ntu_Sig -> GetEntriesFast();
  for(unsigned int it = 0; it < StatusAndNameVar.size(); ++it)
  {
    float min = 99999.;
    float max = 0.0;

    for(int ientry = 0; ientry < nEntries_Sig; ++ientry)
    {
      ntu_Sig -> GetEntry(ientry);
      
      if( VA::varfloat[it] < min )
        min = VA::varfloat[it];      
      if( VA::varfloat[it] > max )
        max = VA::varfloat[it];      

    }
    std::string category = StatusAndNameVar.at(it).second;

    std::string histoName = "h_Sig_"+category;
    h_Sig[category] = new TH1F(histoName.c_str(),"", 100, min, max);
    h_Sig[category] -> Sumw2();

    histoName = "h_Bkg_"+category;
    h_Bkg[category] = new TH1F(histoName.c_str(),"", 100, min, max);
    h_Bkg[category] -> Sumw2();
 
  }

  //----
  //Filling histos

  for(unsigned int it = 0; it < StatusAndNameVar.size(); ++it)
  {
    for(int ientry = 0; ientry < nEntries_Sig; ++ientry)
    {
      ntu_Sig -> GetEntry(ientry);

      h_Sig[StatusAndNameVar.at(it).second] -> Fill(VA::varfloat[it]);

    }
  }


  int nEntries_Bkg = ntu_Bkg -> GetEntriesFast();
  for(unsigned int it = 0; it < StatusAndNameVar.size(); ++it)
  {
    for(int ientry = 0; ientry < nEntries_Bkg; ++ientry)
    {
      ntu_Bkg -> GetEntry(ientry);

      h_Bkg[StatusAndNameVar.at(it).second] -> Fill(VA::varfloat[it]);

    }
  }


  //-----                    
  //Plotting
  std::string outFileName = "plotVar";
  outFile = new TFile((outFileName+".root").c_str(),"RECREATE");

  TCanvas* dummy = new TCanvas("dummy","",0,0,700,600);
  dummy -> Print((outFileName+"."+extension+"[").c_str(),extension.c_str());

  for(unsigned int it = 0; it < StatusAndNameVar.size(); ++it)
  {
    DrawVar(StatusAndNameVar.at(it).second, 1, outFileName);
  }

  outFile -> Close();
  dummy -> Print((outFileName+"."+extension+"]").c_str(),extension.c_str());
  dummy -> Close();

}


//----------------------------------------------------

void DrawVar(const std::string& category, const int& rebin, const std::string& outFileName){

  //----
  //Defining TPads for var histo and ratio

  TCanvas* c = new TCanvas(("c_"+category).c_str(),("Htautau - "+category).c_str(),0,0,700,600);
  c -> cd();

//  TPad* p1 = new TPad("p1","p1",0., 0., 1., 1.);
//  p1 -> SetLeftMargin(0.16);
//  p1 -> SetRightMargin(0.13);
//  p1 -> SetTopMargin(0.08);
//  p1 -> SetBottomMargin(0.2);
//  p1 -> Draw();
//
//  p1 -> cd();
  c -> SetGridx();



  if( h_Sig[category]->Integral() > 0 )
    h_Sig[category] -> Scale( 1. * h_Bkg[category]->Integral() / h_Sig[category]->Integral() );

  char axisTitle[50];
  h_Sig[category] -> Rebin(rebin);
  sprintf(axisTitle,"events",h_Sig[category]->GetBinWidth(1));
  //h_Sig[category] -> GetXaxis() -> SetRangeUser(min,max);
  h_Sig[category] -> GetXaxis() -> SetLabelSize(0.04);
  h_Sig[category] -> GetXaxis() -> SetLabelFont(42);
  h_Sig[category] -> GetXaxis() -> SetTitleSize(0.04);
  h_Sig[category] -> GetXaxis() -> SetTitleOffset(1.10);
  h_Sig[category] -> GetXaxis() -> SetTitle(category.c_str());
  h_Sig[category] -> GetYaxis() -> SetLabelSize(0.04);
  h_Sig[category] -> GetYaxis() -> SetLabelFont(42);
  h_Sig[category] -> GetYaxis() -> SetTitleSize(0.04);
  h_Sig[category] -> GetYaxis() -> SetTitleOffset(1.60);
  h_Sig[category] -> GetYaxis() -> SetTitle(axisTitle);

  h_Sig[category] -> SetLineWidth(1);
  h_Sig[category] -> SetLineColor(kRed);
  h_Sig[category] -> SetFillColor(kYellow);
  h_Sig[category] -> SetMarkerColor(kRed);
  h_Sig[category] -> SetMarkerSize(0);
  gPad->Update();

  h_Bkg[category] -> Rebin(rebin);
  h_Bkg[category] -> SetLineWidth(1);
  h_Bkg[category] -> SetLineColor(kBlack);
  h_Bkg[category] -> SetMarkerColor(kBlack);
  h_Bkg[category] -> SetMarkerStyle(20);
  h_Bkg[category] -> SetMarkerSize(0.7);
  gPad->Update();
 
  float maximum = h_Sig[category] -> GetMaximum();
  if( h_Bkg[category]->GetMaximum() > maximum) maximum = h_Bkg[category]->GetMaximum();

  h_Sig[category] -> SetMinimum(0.);
  h_Sig[category] -> SetMaximum(1.05*maximum);
  h_Sig[category] -> Draw("hist");
  h_Sig[category] -> Draw("P,same");
  h_Bkg[category] -> Draw("P,sames");


  TLegend* legend = new TLegend(0.55, 0.75, 0.80, 0.85);
  legend -> SetFillColor(kWhite);
  legend -> SetFillStyle(1001);
  legend -> SetTextFont(42);
  legend -> SetTextSize(0.035);

  legend -> AddEntry(h_Sig[category],"VBF + ggH Signal","PL");
  legend -> AddEntry(h_Bkg[category],"DY Background","PL");

  legend->Draw("same");

  gPad -> Update();

  c -> Print((outFileName+"."+extension).c_str(),extension.c_str());
  delete c;

  outFile -> cd();

  h_Sig[category] -> Write();
  h_Bkg[category] -> Write();


}

TH1F* ratioHisto(TH1F* h_num, TH1F* h_den)
{
  TH1F* h_ratio = (TH1F*)( h_num->Clone() );

  for(int bin = 1; bin <= h_num->GetNbinsX(); ++bin)
  {
//    double int_num = h_num -> Integral(1,bin);
//    double int_den = h_den -> Integral(1,bin);
    if(h_den->GetBinContent(bin) != 0.0)    
      h_ratio -> SetBinContent(bin,h_num->GetBinContent(bin)/h_den->GetBinContent(bin));
    else
      h_ratio -> SetBinContent(bin,1.0);
      h_ratio -> SetBinError(bin,0.);
  }

  double yMin = 0.75;
  double yMax = 1.25;
  h_ratio -> GetYaxis() -> SetRangeUser(yMin,yMax);
  h_ratio -> GetXaxis() -> SetLabelSize(0.13);
  h_ratio -> GetYaxis() -> SetLabelSize(0.13);
  h_ratio -> GetXaxis() -> SetLabelFont(42);
  h_ratio -> GetYaxis() -> SetLabelFont(42);
  h_ratio -> GetXaxis() -> SetTitleSize(0.17);
  h_ratio -> GetYaxis() -> SetTitleSize(0.17);
  h_ratio -> GetYaxis() -> SetTitleFont(42);
  h_ratio -> GetXaxis() -> SetTitleOffset(1.00);
  h_ratio -> GetYaxis() -> SetTitleOffset(0.50);
  h_ratio -> GetYaxis() -> SetNdivisions(204);
  h_ratio -> GetXaxis() -> SetTitle(h_num->GetXaxis()->GetTitle());
  h_ratio -> GetYaxis() -> SetTitle("ratio");

  return h_ratio;
}

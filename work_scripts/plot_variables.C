#include <iostream>
#include <fstream>
#include "TROOT.h"
#include "TString.h"
#include "TFile.h"
#include "TH1F.h"
#include "TGraphAsymmErrors.h"
#include "TCanvas.h"
#include "TGraph.h"

void plot_variables ( TString filename = "var_down_short" )
{
  ifstream read ( "output/"+filename+".ascii" );
  const int n(12);
  int i(0);
  double a, b;
  TString dump;
  double nvar[n], fom[n];

  while( read >> a >> b >> dump ) {
    nvar[i]=a;
    fom[i]=b;
    ++i;
  } // Doesn't do the last loop..
  nvar[i]=a;
  fom[i]=b;
  ++i;

  if ( i != n ) {
    std::cout << "Wrong number of lines in " << "output/" << filename << ".ascii" << std::endl;
    std::cout << i << " lines instead of expected " << n << std::endl;
    return; };

  
  TCanvas* window=new TCanvas( "window", "FoM for different no. of variables", 700, 500);

  TGraphAsymmErrors* plot = new TGraphAsymmErrors( n, nvar, fom);
  plot->SetTitle("Figure of Merit");
  plot->SetMarkerColor(9);
  plot->SetMarkerStyle(20);
  //  plot->SetMarkerSize(40);
  plot->Draw("ALP");

  plot->GetXaxis()->SetTitle("No. of Variables");
  plot->GetYaxis()->SetTitle("FoM");
  plot->GetXaxis()->CenterTitle();
  plot->GetYaxis()->CenterTitle();

  window->SaveAs("output/plot_"+filename+".pdf");
}



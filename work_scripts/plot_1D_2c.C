#include <iostream>
#include <fstream>
#include <vector>
#include "TROOT.h"
#include "TString.h"
#include "TFile.h"
#include "TH1F.h"
#include "TGraphAsymmErrors.h"
#include "TCanvas.h"
#include "TGraph.h"

void plot_1D_2c ( TString filename = "var_down_short", TString title ="", TString xlabel ="No", TString ylabel ="FoM" )
{
  ifstream read ( "output/"+filename+".ascii" );

  double a, b;
  vector<double> xvar, yvar;

  do{
    read >> a >> b;
    if (!read.good()) {
      std::cout << "Format error in " << "output/" <<filename << ".ascii" << std::endl;
      std::cout << "Input was: " << a << " " << b << std::endl;
    }
    xvar.push_back(a);
    yvar.push_back(b);
  } while ( !read.eof() );

  const int size= xvar.size();
  
  TCanvas* window=new TCanvas( "window", title, 700, 500);

  TGraphAsymmErrors* plot = new TGraphAsymmErrors( size, &xvar[0], &yvar[0]);
  plot->SetTitle(title);
  plot->SetMarkerColor(9);
  plot->SetMarkerStyle(20); // Point
  //  plot->SetMarkerSize(40);
  plot->Draw("ALP"); // axis, points, straight lines

  plot->GetXaxis()->SetTitle(xlabel);
  plot->GetYaxis()->SetTitle(ylabel);
  plot->GetXaxis()->CenterTitle();
  plot->GetYaxis()->CenterTitle();

  window->SaveAs("output/plot_"+filename+".pdf");
}



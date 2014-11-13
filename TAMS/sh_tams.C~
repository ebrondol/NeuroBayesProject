#include <fstream>
#include <vector>
#include "TROOT.h"
#include "TString.h"
#include "TFile.h"
#include "TH1F.h"
#include "headers/load_histos.h"
#include "TAMS/TAMS.h"

using namespace std;

// Analysis tool, which prints the figure of merit of a file
void sh_tams ( TString pathname = "histos/hist_NeuroBayes_test.root", bool verbose=1 )
{
  if (verbose) { cout << "--- TAMS: Starting calculation of Figure of Merit" << endl; };
  float bef, aft;

  TFile* histfile = TFile::Open(pathname);
  std::vector<TH1F*> histos = load_histos (histfile, "NeuroBayes", true);

  float sys = 0.2; // systematic error
  TAMS ta (histos[0], histos[1], sys);
  ta.calc();
  bef = ta.ams_syst_stat();
  if (verbose) {  std::cout << ta.ams_syst_stat() << " " <<  ta.ams() << " " << ta.simple() << " " << ta.simple_syst() << " (ams_syst_stat) vs (ams) vs (simple) vs (simple_syst " << sys << ") " << std::endl; };
  //  ta.savePlot( "fom_plots/pdf_befbin_niter_" + boost + ".png" );

  if (verbose) {cout << "--- TAMS: Rebinning" << endl;};
  ta.rebin();
  ta.calc();
  aft = ta.ams_syst_stat();
  if (verbose) { std::cout << ta.ams_syst_stat() << " " <<  ta.ams() << " " << ta.simple() << " " << ta.simple_syst() << " (ams_syst_stat) vs (ams) vs (simple) vs (simple_syst " << sys << ") , rebinned " << std::endl; };
  //  ta.savePlot( "fom_plots/pdf_aftbin_niter_" + boost + ".png" );

  if (verbose) {
    cout << "#######################################################################\n";
    cout << "The Figure of Merit is: \n";
    cout << "After Rebinning\t" <<  "  (Before Rebinning)" << endl;
    cout << "       " << aft << "\t         (" << bef << ")\n";
    cout << "#######################################################################\n";
  } else {
    cout << "FoM: " << aft << "    (" << bef << ")\n";
  }

  ofstream fout ( "output/tamsout.ascii", ios::app );
  fout << aft << endl;

  fout.close();
  histfile->Close();
}

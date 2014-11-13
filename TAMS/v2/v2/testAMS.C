
TString xs1="0.00197";
TString xs2="0.1938";

void testAMS(){
  gROOT->ProcessLine(".L TAMS.h+");

  TFile *f1=new TFile("../ntuple/input_sync_vbfhiggs_mu.root");
  TFile *f2=new TFile("../ntuple/input_sync_ztautau_mu.root");
  
  TTree *t1=(TTree*)f1->Get("TauCheck");
  TTree *t2=(TTree*)f2->Get("TauCheck");

  TH1F *h1=new TH1F("h1","h1",13,40,300); //nbin, low bin, high bin
  //  t1->Draw("mvis>>h1","weight*"+xs1,"goff");
  t1->Draw("m_sv>>h1","weight*"+xs1,"goff");

  TH1F *h2=new TH1F("h2","h2",13,40,300);
  //  t2->Draw("mvis>>h2","weight*"+xs2,"goff");
  t2->Draw("m_sv>>h2","weight*"+xs2,"goff");

  TAMS a(h1,h2);
  a.calc();
  cout << a.ams() << " (ams) vs (simple) " << a.simple() << endl;

}

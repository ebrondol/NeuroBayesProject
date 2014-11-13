
niceplot(){
  TFile *_file0 = TFile::Open("../histo_111111111111.root");
  gROOT->LoadMacro("TAMS.h+");
  TAMS ta(BDT_S,BDT_B,0.1);
  ta.calc();
  cout << "AMS before: " << ta.ams() << endl;

  ta.rebin();
  ta.calc();
  cout << "AMS after : " << ta.ams() <<endl;

  b->Draw();

  s->SetMarkerColor(kRed);
  s->SetLineColor(kRed);
  s->Draw("same");

}

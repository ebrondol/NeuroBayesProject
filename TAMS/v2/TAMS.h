//TAMS
//Author: Martin Flechl, 2014
//calculate average median significance of two histograms

#ifndef ROOT_TAMS
#define ROOT_TAMS

#ifndef ROOT_TH1F
#include "TH1F.h"
#endif

#include <iostream> //only for cout
#include <vector>

class TH1F;

class TAMS {

protected:
  //  float m_simple; //simple s/sqrt(s+b)
  //  float m_simple_syst; //simple s/sqrt(s+b(1+sys*sys*b))
  float m_simple; //simple s/sqrt(b)
  float m_simple_syst; //simple s/sqrt(b(1+sys*sys*b))
  float m_simple_stat; //simple s/sqrt(b) with stat. unc. on b
  float m_simple_syst_stat; //simple s/sqrt(b(1+sys*sys*b)) with stat unc. on b
  float m_ams; //ams
  float m_ams_stat; //ams with stat unc on b
  float m_ams_syst; //ams with sys unc on b
  float m_ams_syst_stat; //ams with sys unc on b

  float m_simple_err[2];
  float m_simple_syst_err[2];
  float m_simple_stat_err[2];
  float m_simple_syst_stat_err[2];
  float m_ams_err[2];
  float m_ams_stat_err[2];
  float m_ams_syst_err[2];
  float m_ams_syst_stat_err[2];

  float m_rsys;
  TH1F *m_h1; //histo signal
  TH1F *m_h2; //histo bkg

public:
  TAMS() { m_rsys=0.1; m_simple=-1; m_simple_syst=-1; m_ams=-1; m_simple_stat=-1; m_simple_syst_stat=-1; };
  TAMS(TH1F *_h1, TH1F *_h2, float _rsys=0.1) { m_h1=_h1; m_h2=_h2; m_rsys=_rsys; m_simple=-1; m_simple_syst=-1; m_simple_stat=-1; m_simple_syst_stat=-1; m_ams=-1; m_ams_stat=-1; m_ams_syst=-1; m_ams_syst_stat=-1; };

  void seth1(TH1F *_h){ m_h1=_h; };
  void seth2(TH1F *_h){ m_h2=_h; };
  void seth(TH1F *_h1, TH1F *_h2){ m_h1=_h1; m_h2=_h2; };

  float ams(){ if (m_ams<0) this->calc(); return m_ams; };
  float ams_err(int which){ if (m_ams<0) this->calc(); if (which<0 || which>1) return -1; else return m_ams_err[which]; }; //-err: 0; +err: 1

  float ams_stat(){ if (m_ams_stat<0) this->calc(); return m_ams_stat; };
  float ams_syst(){ if (m_ams_syst<0) this->calc(); return m_ams_syst; };
  float ams_syst_stat(){ if (m_ams_syst_stat<0) this->calc(); return m_ams_syst_stat; };
  float simple(){ if (m_simple<0) this->calc(); return m_simple; };
  float simple_stat(){ if (m_simple_stat<0) this->calc(); return m_simple_stat; };
  float simple_syst(float _rsys=-1){ 
    if ( _rsys>0 || m_simple_syst<0 ) m_rsys=_rsys;
    this->calc();
    return m_simple_syst; 
  };
  float simple_syst_stat(float _rsys=-1){ 
    if ( _rsys>0 || m_simple_syst_stat<0 ) m_rsys=_rsys;
    this->calc();
    return m_simple_syst_stat; 
  };

  void listAll(/*float _rsys=-1*/){
    std::cout << "Systematics: " << this->m_rsys << std::endl;
    std::cout << "AMS              :" << this->ams()              << "\t -" << this->ams_err(0) << " +" << this->ams_err(1) << std::endl;
    std::cout << "AMS    stat      :" << this->ams_stat()         << std::endl;
    std::cout << "AMS    syst      :" << this->ams_syst()         << std::endl;
    std::cout << "AMS    stat syst :" << this->ams_syst_stat()    << std::endl;
    std::cout << "Simple           :" << this->simple()           << std::endl;
    std::cout << "Simple stat      :" << this->simple_stat()      << std::endl;
    std::cout << "Simple syst      :" << this->simple_syst()      << std::endl;
    std::cout << "Simple stat syst :" << this->simple_syst_stat() << std::endl;
  }

  void rebin() {
    //    const float RELSTATMAX=0.5;
    const float RELSTATMAX=0.5;

    std::vector<float> bin_edge;
    std::vector<float> bin_s;
    std::vector<float> bin_b;
    std::vector<float> bin_berr;
    std::vector<float> bin_serr;
    
    int nedges=m_h1->GetNbinsX()+1; //edges=bins+1
    float highest_edge=m_h1->GetBinLowEdge( nedges );
    bin_edge.push_back(highest_edge);

    float s=0;
    float b=0;
    float berr2=0;
    float serr2=0;
    float bprev=0;

    for (int i=nedges-1; i>=1; i--){ //loop over bin edges
      s+=m_h1->GetBinContent(i);
      b+=m_h2->GetBinContent(i);
      serr2+=pow(m_h1->GetBinError(i),2);
      berr2+=pow(m_h2->GetBinError(i),2);

      //check if this is a new edge
      if ( b<1e-3 ) continue; //if b is negativ or 0 or very small, continue
      if ( (sqrt(berr2)/b)>RELSTATMAX ) continue; //if the rel stat unc on the background is >X%, continue
      if ( b<bprev ) continue; //more b than bin to the right (previous bin)

      //we have a new edge!
      bin_edge.push_back( m_h1->GetBinLowEdge( i ) );
      bin_s.push_back( s );
      bin_b.push_back( b );
      bin_serr.push_back( sqrt(serr2) );
      bin_berr.push_back( sqrt(berr2) );
      bprev=b;
      b=0; s=0; serr2=0; berr2=0;
    }
    //    lbin=bin_edge.size()-1;
    if (  fabs( bin_edge.back()-m_h1->GetBinLowEdge( 1 )  )>1e-5  ){ //replace lowest bin boarder with lower-most old bin border
      bin_edge.back()=m_h1->GetBinLowEdge( 1 );
      bin_s.back()+=s;
      bin_b.back()+=b;
      bin_serr.back()=sqrt( pow(bin_serr.back(),2)+serr2 );
      bin_berr.back()=sqrt( pow(bin_berr.back(),2)+berr2 );
    }

    //debugging: compare before/after:
    //    for (int i=1; i<=m_h1->GetNbinsX(); i++) 
    //      cout << m_h1->GetBinLowEdge(i) << " - " << m_h1->GetBinLowEdge(i+1) << ":\t" << m_h1->GetBinContent(i) << "\t" << m_h1->GetBinError(i)  << endl;

    //Create new histos
    int nbins=bin_edge.size()-1;
    float *array_edge=new float[nbins+1];
    for (int j=0; j<=nbins; j++) array_edge[j]=bin_edge.at(nbins-j);

    m_h1=new TH1F("s","s",nbins,array_edge);
    m_h2=new TH1F("b","b",nbins,array_edge);

    delete[] array_edge;

    for (int j=0; j<nbins; j++){
      m_h1->SetBinContent(j+1,bin_s.at   (nbins-1-j) );
      m_h1->SetBinError(  j+1,bin_serr.at(nbins-1-j) );
      m_h2->SetBinContent(j+1,bin_b.at   (nbins-1-j) );
      m_h2->SetBinError(  j+1,bin_berr.at(nbins-1-j) );
    }

    //debugging: compare before/after:
    //    cout << "after" << endl;
    //    for (int i=1; i<=m_h1->GetNbinsX(); i++) 
    //      cout << m_h1->GetBinLowEdge(i) << " - " << m_h1->GetBinLowEdge(i+1) << ":\t" << m_h1->GetBinContent(i) << "\t" << m_h1->GetBinError(i)  << endl;

  }


  void calc() { 
    int nbins=m_h1->GetNbinsX();
    if ( nbins != m_h2->GetNbinsX() ){
      std::cerr << "ERROR: Not the same number of bins! Giving up..." << std::endl;
      return;
    }
    if ( m_h1->GetBinLowEdge(nbins) != m_h2->GetBinLowEdge(nbins) ){
      std::cerr << "ERROR: Not the same range! Giving up..." << std::endl;
      return;
    }

    const float br=1.0;
    float s, b, bt, b_stat, b_syst, b_stat2, b_syst2, s_stat;
    m_ams=0; m_ams_stat=0;  m_ams_syst=0; m_ams_syst_stat=0; m_simple=0; m_simple_syst=0; m_simple_stat=0; m_simple_syst_stat=0; 
    float _ams_err[4]={0}; float _ams_stat_err[4]={0};  float _ams_syst_err[4]={0}; float _ams_syst_stat_err[4]={0}; float _simple_err[4]={0}; float _simple_syst_err[4]={0}; float _simple_stat_err[4]={0}; float _simple_syst_stat_err[4]={0}; 
    for (int ibin=1; ibin<m_h1->GetNbinsX(); ibin++){
      s=m_h1->GetBinContent(ibin);
      b=m_h2->GetBinContent(ibin);
      s_stat=m_h1->GetBinError(ibin); //absolute unc
      b_stat=m_h2->GetBinError(ibin); //absolute unc
      if ( b_stat<0.5*sqrt(br) ) b_stat=0.5*sqrt(br);
      b_stat2=b_stat*b_stat;
      b_syst=b*m_rsys; //absolute unc
      if (b_syst<m_rsys*br ) b_syst=m_rsys*br;
      b_syst2=b_syst*b_syst;
      bt=b+br;
      //      m_simple+=pow( s/sqrt( s+bt ) , 2 );
      //      m_simple_syst+=pow( s/sqrt( s+bt*(1+m_rsys*m_rsys*bt) ) , 2 );
      m_simple          += pow( s/sqrt( bt ) , 2 );
      m_simple_stat     += pow( s/sqrt( bt + b_stat2         ) , 2 );
      m_simple_syst     += pow( s/sqrt( bt + b_syst2         ) , 2 );
      m_simple_syst_stat+= pow( s/sqrt( bt + b_stat2+b_syst2 ) , 2 );

      //      m_ams+=              2*(   (s+bt) * log( 1 + s/bt  ) - s  );
      //      m_ams_stat+=         2*(   (s+bt) * log(  (s+bt)*(bt+b_stat2) / ( bt*bt + (s+bt)*b_stat2 )  )    -  bt*bt/b_stat2 * log( 1 + b_stat2*s/( bt*(bt+b_stat2) )   )   );
      //      m_ams_syst+=         2*(   (s+bt) * log(  (s+bt)*(bt+b_syst2) / ( bt*bt + (s+bt)*b_syst2 )  )    -  bt*bt/b_syst2 * log( 1 + b_syst2*s/( bt*(bt+b_syst2) )   )   );
      //      m_ams_syst_stat+=    2*(   (s+bt) * log(  (s+bt)*(bt+b_stat2+b_syst2) / ( bt*bt + (s+bt)*(b_stat2+b_syst2) )  )    -  bt*bt/(b_stat2+b_syst2) * log( 1 + (b_stat2+b_syst2)*s/( bt*(bt+b_stat2+b_syst2) )   )   );
      m_ams+=              calc_ams2(s, bt);
      m_ams_stat+=         calc_ams2(s, bt, b_stat2);
      m_ams_syst+=         calc_ams2(s, bt, b_syst2);
      m_ams_syst_stat+=    calc_ams2(s, bt, b_syst2+b_stat2);

      _ams_err[0]+=              2*(   (s+bt+b_stat) * log( 1 + s/(bt+b_stat)  ) - s  );
      _ams_err[1]+=              2*(   (s+bt-b_stat) * log( 1 + s/(bt-b_stat)  ) - s  );
      _ams_err[2]+=              2*(   (s+bt+s_stat) * log( 1 + (s+s_stat)/bt  ) - (s+s_stat)  );
      _ams_err[3]+=              2*(   (s+bt-s_stat) * log( 1 + (s-s_stat)/bt  ) - (s-s_stat)  );

      //      if ( (s+b)>5 ) m_simple+=pow( s/sqrt(s+b+br) , 2 );
      //      if ( (s+b)>5 ) m_ams+=2*(   (s+b+br) * log( 1 + s/(b + br) ) - s  );
    }
    m_simple=sqrt(m_simple);
    m_simple_stat=sqrt(m_simple_stat);
    m_simple_syst=sqrt(m_simple_syst);
    m_simple_syst_stat=sqrt(m_simple_syst_stat);

    m_ams=sqrt(m_ams);
    m_ams_stat=sqrt(m_ams_stat);
    m_ams_syst=sqrt(m_ams_syst);
    m_ams_syst_stat=sqrt(m_ams_syst_stat);

    m_ams_err[0]= sqrt(   pow( sqrt(_ams_err[0])-m_ams, 2 )  + pow( sqrt(_ams_err[3])-m_ams, 2 ) );
    m_ams_err[1]= sqrt(   pow( sqrt(_ams_err[1])-m_ams, 2 )  + pow( sqrt(_ams_err[2])-m_ams, 2 ) );
  };

  float calc_ams2(float s, float b){
    return 2*(   (s+b) * log( 1 + s/b  ) - s  );
  }
  float calc_ams2(float s, float b, float berr2){
    return 2*(   (s+b) * log(  (s+b)*(b+berr2) / ( b*b + (s+b)*berr2 )  )    -  b*b/berr2 * log( 1 + berr2*s/( b*(b+berr2) )   )   );
  }


  ClassDef(TAMS,0);

};


#endif

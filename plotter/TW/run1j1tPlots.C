#define DEBUGFLAG 
R__LOAD_LIBRARY(Histo.C+)
R__LOAD_LIBRARY(Looper.C+)
R__LOAD_LIBRARY(Plot.C+)
R__LOAD_LIBRARY(TW/AdditionalStuff.C+)
#include "Histo.h"
#include "Looper.h"
#include "Plot.h"
#include "TW/AdditionalStuff.C"

void DrawPlot(TString var, TString cut, TString chan, Int_t nbins, Float_t bin0, Float_t binN, TString Xtitle, bool setLegendLeft=false );
TString pathToTree = "/pool/cienciasrw/userstorage/sscruz/TW/jul25/";
TString NameOfTree = "Mini1j1t";


void run1j1tPlots(){

  DrawPlot("TnBTotal - 1 "    , "(TNJets == 1) && (TNBtags == 1) && (TIsSS == 0) ", "ElMu", 4, -0.5, 3.5, "n_{B}^{20}");
  DrawPlot("TnLooseCentral - 1 ", "(TNJets == 1) && (TNBtags == 1) && (TIsSS == 0) ", "ElMu", 5, -0.5, 4.5, "n_{20}");

  DrawPlot("TDilepMETJetPt_THTtot ","(TNJets == 1) && (TNBtags == 1) && (TIsSS == 0) ", "ElMu", 20, 0., 1., "p_{T}(lljmet) / H_{T}");
  DrawPlot("TC_jll ","(TNJets == 1) && (TNBtags == 1) && (TIsSS == 0) ", "ElMu", 20, 0., 1., "C_{jll}");
  DrawPlot("TTJet1_pt ", "(TNJets == 1) && (TNBtags == 1) && (TIsSS == 0) ", "ElMu", 20, 0., 250., "p^{jet1}_{T} [GeV]");
  DrawPlot("TDilepMETJetPt "    , "(TNJets == 1) && (TNBtags == 1) && (TIsSS == 0) ", "ElMu", 20, 0., 200., "p_{T}(lljmet)");
  DrawPlot("TTHTtot "     , "(TNJets == 1) && (TNBtags == 1) && (TIsSS == 0) ", "ElMu", 20, 70., 550., "H_{T} [GeV]");
  DrawPlot("TTJetLooseCentralpt "    , "(TNJets == 1) && (TNBtags == 1) && (TIsSS == 0) ", "ElMu", 20, 0., 30., "Loose jet p_{T} [GeV]");
  DrawPlot("TMSys ","(TNJets == 1) && (TNBtags == 1) && (TIsSS == 0) ", "ElMu", 20, 50., 750., "m_{sys}[GeV]");
  DrawPlot("THTLepOverHT ","(TNJets == 1) && (TNBtags == 1) && (TIsSS == 0) ", "ElMu", 20, 0., 1., "H_{T}+lep / H_{T}");
  DrawPlot("TDilepJetPt ","(TNJets == 1) && (TNBtags == 1) && (TIsSS == 0) ", "ElMu", 20, 0., 250., "p_{T}^{ll,j} [GeV]");


  
  
}

void DrawPlot(TString var, TString cut, TString chan, Int_t nbins, Float_t bin0, Float_t binN, TString Xtitle, bool setLegendLeft = false){
  Plot* p = new Plot(var, cut, chan, nbins, bin0, binN, "Title", Xtitle);
  p->SetPath(pathToTree); p->SetTreeName(NameOfTree);
  p->SetPathSignal(pathToTree + "");
  p->verbose = false;
  // p->SetVarName("forFit");

  p->AddSample("WZ", "VV", itBkg, kYellow);
  p->AddSample("WW", "VV", itBkg);
  p->AddSample("ZZ", "VV", itBkg);
  p->AddSample("TTWToLNu", "ttV", itBkg, kOrange-3);
  p->AddSample("TTWToQQ", "ttV", itBkg);
  p->AddSample("TTZToQQ", "ttV", itBkg);
  p->AddSample("TTZToLLNuNu", "ttV", itBkg);

  p->AddSample("DYJetsToLL_M10to50_aMCatNLO", " Z/#gamma* #rightarrow e^{#pm}#mu^{#mp}", itBkg, TColor::GetColor("#3b0160")); // kRed);
  p->AddSample("DYJetsToLL_M50_aMCatNLO",     " Z/#gamma* #rightarrow e^{#pm}#mu^{#mp}", itBkg);
  p->AddSample("TTbar_Powheg", "t#bar{t}", itBkg, TColor::GetColor("#669966")); // TColor::GetColor("#8ADCFF"));
  p->AddSample("TW", "tW", itBkg, TColor::GetColor("#ff4800")); // TColor::GetColor("#ffc878"));
  p->AddSample("TbarW", "tW", itBkg);
  p->AddSample("TTbar_PowhegSemi","Non W/Z", itBkg, kGray);
  p->AddSample("WJetsToLNu_MLM","Non W/Z", itBkg, kGray);


  p->AddSample("MuonEG", "Data", itData);
  p->AddSample("SingleMuon", "Data", itData);
  p->AddSample("SingleElec", "Data", itData);

  p->AddSample("TTbar_Powheg_hdampUp", "t#bar{t}", itSys, 1,  "hdampUp"); 
  p->AddSample("TTbar_Powheg_hdampDown", "t#bar{t}", itSys, 1,  "hdampDown"); 




  p->AddSample("TTbar_Powheg_ueUp", "t#bar{t}", itSys, 1,  "ueUp"); 
  p->AddSample("TTbar_Powheg_ueDown", "t#bar{t}", itSys, 1,  "ueDown"); 
  p->AddSample("TTbar_Powheg_isrUp", "t#bar{t}", itSys, 1,  "isrUp"); 
  p->AddSample("TTbar_Powheg_isrDown", "t#bar{t}", itSys, 1,  "isrDown"); 
  p->AddSample("TTbar_Powheg_fsrUp", "t#bar{t}", itSys, 1,  "fsrUp"); 
  p->AddSample("TTbar_Powheg_fsrDown", "t#bar{t}", itSys, 1,  "fsrDown"); 

  p->AddSample("TW_noFullyHadr_isrUp","tW", itSys, 1,  "isrUp");
  p->AddSample("TW_noFullyHadr_isrDown","tW", itSys, 1,  "isrDown");
  p->AddSample("TW_noFullyHadr_fsrUp","tW", itSys, 1,  "fsrUp");
  p->AddSample("TW_noFullyHadr_fsrDown","tW", itSys, 1,  "fsrDown");

  p->AddSample("TW_noFullyHadr_MEscaleUp","tW", itSys, 1,  "tWMe_s_caleUp");
  p->AddSample("TW_noFullyHadr_MEscaleDown","tW", itSys, 1,  "tWMe_s_caleDown");

  p->AddSample("TbarW_noFullyHadr_isrUp","tW", itSys, 1,  "isrUp");
  p->AddSample("TbarW_noFullyHadr_isrDown","tW", itSys, 1,  "isrDown");
  p->AddSample("TbarW_noFullyHadr_fsrUp","tW", itSys, 1,  "fsrUp");
  p->AddSample("TbarW_noFullyHadr_fsrDown","tW", itSys, 1,  "fsrDown");
  p->AddSample("TbarW_noFullyHadr_MEscaleUp","tW", itSys, 1,  "tWMe_s_caleUp");
  p->AddSample("TbarW_noFullyHadr_MEscaleDown","tW", itSys, 1,  "tWMe_s_caleDown");

  p->AddSample("TTbar_Powheg", "t#bar{t}", itSys, 1, "ScaleUp"); 
  p->AddSample("TTbar_Powheg", "t#bar{t}", itSys, 1, "ScaleDown"); 
  p->AddSample("TTbar_Powheg", "t#bar{t}", itSys, 1, "pdfUp"); 
  p->AddSample("TTbar_Powheg", "t#bar{t}", itSys, 1, "pdfDown"); 


  p->SetRatioMin( 0.6 );
  p->SetRatioMax( 1.4 );
  

  p->SetPlotFolder("1j1t/");

  p->AddSystematic("stat,JES,Btag,Mistag,PU,ElecEff,MuonEff,Trig"); //,LepEff

  p->doYieldsInLeg=false;

  p->doSetLogy = false;
  p->doData = true;
  if (setLegendLeft) p->SetLegendPosition("UL");
  p->DrawStack("ElMu_0_log", 1);
  //cout << "done stack" << endl;
  // p->PrintSystematics();
  //p->PrintSystYields();

  delete p;

}


void runLooper(){
  Looper* p = new Looper(pathToTree, NameOfTree, "TMET", "TMET > 0", "ElMu", 30, 0, 300);
  Histo* h = p->GetHisto("TW");
  h->SetLineColor(kRed);
  cout << "nEntries = " << h->GetEntries() << endl;
  cout << "Yield = " << h->Integral()*35900 << endl;
  h->Draw();
}

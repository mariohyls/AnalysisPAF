#include "Looper.h"

std::vector<TString> GetAllVars(TString varstring){ 
  std::vector<TString> g;
  TString var; Int_t i;
  TString chain = varstring;
  while(chain.Contains("T")){
    i = 0;
    var = chain(chain.First('T'), chain.Sizeof());
    //    while(TString(var[i]).IsAlnum()) i++;
    while( var[i] != ' ') i++;
    var = var(0,i);
    g.push_back(var);
    chain.ReplaceAll(var + " ", " ");
  }
  return g;
}

TString Looper::CraftVar(TString varstring, TString sys){
  TString var = varstring;
  std::vector<TString> AllVars = GetAllVars(var);
  Int_t nvars = AllVars.size();
  for(Int_t i = 0; i < nvars; i++) 
    if(tree->GetBranchStatus(AllVars.at(i) + sys)){
      var.ReplaceAll(AllVars.at(i) + " ", AllVars.at(i)+sys+" ");
    }
  return var;
}

TString Looper::CraftFormula(TString cuts, TString chan, TString sys, TString options){
  TString schan = ("1");
  if     (chan == "ElMu")  schan = (Form("(TChannel == %i)", iElMu));
  else if(chan == "Muon")  schan = (Form("(TChannel == %i)", iMuon));
  else if(chan == "Elec")  schan = (Form("(TChannel == %i)", iElec));
  else if(chan == "2lss")  schan = (Form("(TChannel == %i)", i2lss));
  else if(chan == "3l")    schan = (Form("(TChannel == %i)", iTriLep));
  else if(chan == "4l")    schan = (Form("(TChannel == %i)", iFourLep));
  else if(chan == "SF" || chan == "sameF") schan = (Form("(TChannel != %i)", iElMu));
  else if(chan == "PromptLep") schan = Form("(TChannel == %i || TChannel == %i)", iTriLep, i2lss);
  else if(chan == "PromptTau") schan = Form("(TChannel == %i || TChannel == %i)", iSS1tau, iOS1tau);
  else if(chan == "SSTau") schan = Form("(TChannel == %i)", iSS1tau);
  else if(chan == "OSTau") schan = Form("(TChannel == %i)", iOS1tau);
  else if(chan == "All")   schan = ("1");
  else schan = chan;

  TString weight = TString("TWeight");
  if(tree->GetBranchStatus(weight + "_" + sys)){
    weight += "_" + sys; 
  }

  std::vector<TString> AllVars = GetAllVars((TString) cuts);
  Int_t nvars = AllVars.size();
  for(Int_t i = 0; i < nvars; i++) 
    if(tree->GetBranchStatus(AllVars.at(i) + sys)){
      cuts = ( ((TString) cuts).ReplaceAll(AllVars.at(i) + " ", AllVars.at(i)+sys + " "));
    }
  TString                                                  formula = TString("(") + cuts + TString(")*(") + schan + TString(")*") + weight;
  if((options.Contains("Fake") || options.Contains("fake"))){
    if(chan.Contains("Lep")) schan = Form("(TChannel == %i || TChannel == %i)", i2lss_fake, iTriLep_fake);
    //if(chan.Contains("Tau")) schan = Form("(TChannel == %i)", i1Tau_emufake);
    if(options.Contains("sub") || options.Contains("Sub"))  formula = TString("(") + cuts + TString(")*(") + schan + TString(")*") + weight;
    else formula = TString("(") + cuts + TString(")*(") + schan + TString(")");
  }
  if(options.Contains("isr") || options.Contains("ISR"))   formula = "TISRweight*(" + formula + ")";
  if(options.Contains("noWeight"))                         formula = TString("(") + cuts + TString(")*(") + schan + TString(")");
  return formula;
}

void Looper::SetFormulas(TString systematic){
  if(FormulasCuts) delete FormulasCuts;
  if(FormulasVars) delete FormulasVars;
  if(FormulasLHE)  delete FormulasLHE;
  stringcut = ""; stringvar = "";
  TString cu = ""; TString ch = ""; TString v = ""; 
  stringcut = CraftFormula(cut, chan, systematic, options);
  stringvar = CraftVar(var, systematic);

  if(stringvar.Contains("[") && stringvar.Contains("]")){
    TString number = TString(stringvar(stringvar.First("["), stringvar.First("]")) );
    stringvar.ReplaceAll(number, "");
    numberInstance = TString(number.ReplaceAll("[", "").ReplaceAll("]","")).Atoi(); 
  }
  //cout << "stringcuts = " << stringcut << endl;
  //cout << "stringvars = " << stringvar << endl;
  //cout << "numberInstance = " << numberInstance << endl;

  FormulasCuts = new TTreeFormula("Form_" + sampleName + "_" + systematic + "_cut", stringcut, tree);
  FormulasVars = new TTreeFormula("Form_" + sampleName + "_" + systematic + "_var", stringvar, tree);
  if(doSysScale || doSysPDF){
    FormulasLHE  = new TTreeFormula("Form_" + sampleName + "_" + systematic + "_LHE", "TLHEWeight", tree);
    FormulasLHE->GetNdata();
  }
}

void Looper::CreateHisto(TString sys){
  if(Hist) delete Hist;
  //Hist = new Histo(TH1F(sampleName+"_"+sys+"_"+var,sampleName+"_"+sys+"_"+var, nbins, bin0, binN));
  TString name = sampleName;
  if(sys != "0") name += "_" + sys;
  if(bin0 != binN) Hist = new Histo(TH1F(name,sampleName+"_"+sys+"_"+var, nbins, bin0, binN));
  else             Hist = new Histo(TH1F(name,sampleName+"_"+sys+"_"+var, nbins, vbins));
  if(doSysPDF || doSysScale){
    for(Int_t i = 0; i < nLHEweights; i++){
      if(bin0 != binN) hLHE[i] = new TH1F(name+"_"+Form("%i", i),sampleName+"_"+sys+"_"+var+"_"+Form("%i",i), nbins, bin0, binN);
      else             hLHE[i] = new TH1F(name+"_"+Form("%i", i),sampleName+"_"+sys+"_"+var+"_"+Form("%i",i), nbins, vbins);
    }
  }
}

void Looper::Loop(TString sys){
  //SetFormulas(sys); // Change systematic
  Int_t nEntries = tree->GetEntries();
  Float_t val = 0; Float_t weight = 0;
  Int_t nInstances = FormulasVars->GetNdata();
  Bool_t doAllInstances(false);
  if(options.Contains("AllInstances")){
    doAllInstances = true;
  }

  // Options for systematics
  HeppySampleName = sampleName; 
  if(options.Contains("HeppySampleName:")){
    TString hsn = options(options.First("HeppySampleName"), options.Sizeof()); 
    hsn.ReplaceAll("HeppySampleName:", "");
    if(hsn.Contains(",")) hsn = hsn(0, hsn.First(","));
    HeppySampleName = hsn;
  }
  //cout << HeppySampleName << endl;
  if(doSysPDF || doSysScale) hLHEweights = loadSumOfLHEweights(pathToHeppyTrees, HeppySampleName);

  // For fake or flips from data
  Float_t f; Int_t nfakes;
  if(options.Contains("Fake") || options.Contains("fake")){
    ForFLepPt    = GetFormula("LepPt",    "TFLep_Pt");
    ForFLepEta   = GetFormula("LepEta",   "TFLep_Eta");
    ForFLepPdgId = GetFormula("LepPdgId", "TFLep_pdgId");
    ForLepChar   = GetFormula("LepChar",  "TLep_Charge");
    FornSelTau   = GetFormula("nSelTau",  "TNTaus");
    FornSelLep   = GetFormula("nSelLep",  "TNSelLeps");
    FornFakeLep  = GetFormula("nFakeLep", "TNFakeableLeps");
  }

  Int_t counter = 0;
  for (Long64_t jentry=0; jentry<nEntries; jentry++) {
    tree->GetEntry(jentry);
    counter ++;
    if(numberInstance != 0) FormulasVars->GetNdata();
    weight  = FormulasCuts->EvalInstance();
    val     = FormulasVars->EvalInstance(numberInstance);

    // For ScaleME and PDF systematics
    if(doSysScale || doSysPDF){
      Float_t LHEweight = 1;
      for(Int_t i = 0; i < nLHEweights; i++){
        LHEweight = weight*getLHEweight(i);
        hLHE[i]->Fill(val, LHEweight);
      }
    }

    if(options.Contains("Fake") || options.Contains("fake")){
      f = 1; nfakes = 0; 
      if(options.Contains("Sub") || options.Contains("sub")) weight *= -1;
      ForFLepPt   ->GetNdata();
      ForFLepEta  ->GetNdata();
      ForFLepPdgId->GetNdata();
      ForLepChar  ->GetNdata();
      FornFakeLep->GetNdata();
      FornSelLep->GetNdata();
      FornSelTau->GetNdata();
      nFakeLeps = FornFakeLep->EvalInstance();
      nLeps     = FornSelLep->EvalInstance();
      nTaus     = FornSelTau->EvalInstance();
      if(nFakeLeps <= 0)           continue;
      for(Int_t i = 0; i < nFakeLeps ; i++){
        FLepPt    = ForFLepPt ->EvalInstance(i);
        FLepEta   = ForFLepEta->EvalInstance(i);
        FLepPdgId = ForFLepPdgId->EvalInstance(i);
        if(FLepPdgId == 11) f *= electronFakeRate(FLepPt, FLepEta);
        if(FLepPdgId == 13) f *=     muonFakeRate(FLepPt, FLepEta);
        if(f >= 0.99) continue;
        weight *= f/(1-f);
        //if(weight != 0) cout << "[" << counter << "] nFakes = " << nFakeLeps << ", weight = " << weight << endl;
      }
    }

    if(doAllInstances){
      nInstances = FormulasVars->GetNdata();
      for(Int_t g = 0; g < nInstances; g++){
        val = FormulasVars->EvalInstance(g);
        Hist->Fill(val, weight);
      }
    }
    else    Hist->Fill(val, weight);

  }

  Float_t nom = 0; Float_t var = 0; Float_t ext = 0; Float_t env = 0;
  if(doSysScale){ // Get envelope!!
    //cout << " Scale matrix element weights: \n";
    for(Int_t bin = 1; bin <= nbins; bin++){
      ext = 0; env = 0;
      nom = hLHE[0]->GetBinContent(bin); // weight 0 is not nominal???????????????? 
      for(Int_t w = 1; w < 9; w++){
        if(w==4 || w==6) continue; // Following numbering scheme in http://www.hep.uniovi.es/juanr/pdfWeights_feb17.txt (count from 0 instead of from 1)
        var = hLHE[w]->GetBinContent(bin);  
        if(sys.Contains("Down") || sys.Contains("down")){
          if(nom-var > ext){ ext = nom-var; env = var;}
        }
        else{
          if(var-nom > ext){ ext = var-nom; env = var;}
        }
        //cout << "   nom = " << nom << ", var = " << var << endl;
      }
      Hist->SetBinContent(bin, env);
    }
  }
  else if(doSysPDF){
    Float_t rms = 0; Float_t alpha_up = 0; Float_t alpha_dw = 0;
    //cout << " PDF weights: \n";
    for(Int_t bin = 1; bin <= nbins; bin++){
      nom = hLHE[0]->GetBinContent(bin);
      for(Int_t w = 9; w < 109; w++){
        var = hLHE[w]->GetBinContent(bin);  
        ext += (nom-var)*(nom-var);
        //cout << "   nom = " << nom << ", var = " << var << endl;
      }
      rms = TMath::Sqrt(ext/100);
      alpha_up = TMath::Abs(hLHE[109]->GetBinContent(bin) - nom);
      alpha_dw = TMath::Abs(hLHE[110]->GetBinContent(bin) - nom);
      env = TMath::Sqrt(rms*rms + ((alpha_up-alpha_dw)*0.75/2)*((alpha_up-alpha_dw)*0.75/2));
      //cout << env << " " << rms << " " << alpha_up << " " << alpha_dw << endl;
      if(sys.Contains("Up") || sys.Contains("up"))  Hist->SetBinContent(bin, nom + env);
      else                                          Hist->SetBinContent(bin, nom - env);
    }
  }

}

Float_t Looper::getLHEweight(Int_t i){
  Float_t weight = 0; Float_t norm = 0; 
  Int_t bin = 0; 
  if      (i<9 )  bin = i + 1002;   // 1002-1010: muRmuF
  else if (i<111) bin = i + 1993;   // 2002-2103: NNPDF
  else if (i<166) bin = i + 2891;   // 3002-3056: CT10
  else if (i<222) bin = i + 3836;   // 4000-4057: MMHT2014
  else if (i<249) bin = i + 4780;   // 5002-5028: muRmuF, hdamp 
  norm = hLHEweights->GetBinContent(1002);
  weight = FormulasLHE->EvalInstance(i)/hLHEweights->GetBinContent(bin)*norm; 
  // cout << "weight for " << i << " is " << weight << endl;
  return weight;
}

void Looper::loadTree(){
  TString prefix = "Tree_"; TString sufix = ".root";
	TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject(path + prefix + sampleName + sufix);
	if (!f || !f->IsOpen()) {
		f = new TFile(path + prefix + sampleName + sufix);
	}
	f->GetObject(treeName,tree);
}

Histo* Looper::GetHisto(TString sample, TString sys){
  SetSampleName(sample); 
  loadTree();
  doSysPDF = false; doSysScale = false;

  // For scale and PDF uncertainties
  // ----------------------------------------------------------------
  if(sys.Contains("Scale") || sys.Contains("scale") || sys.Contains("Q2") || sys.Contains("PDF") || sys.Contains("pdf")){ // Using LHE weights
    if(tree->GetBranchStatus("TLHEWeight")){
      if(sys.Contains("PDF") || sys.Contains("pdf")) doSysPDF = true;
      else doSysScale = true;
    }
  }

  CreateHisto(sys);
  SetFormulas(sys);
  Loop(sys);
  return Hist;
}

TH1D* loadSumOfLHEweights(TString pathToHeppyTrees, TString sampleName){
  vector<TString> files = GetAllFiles(pathToHeppyTrees, sampleName);
  Int_t nFiles = files.size();
  //cout << "Number of files = " << nFiles << endl;
  TFile* f; TH1D* hSumOfLHEweights; TH1D* htemp;

  f = TFile::Open(files.at(0));
  f->GetObject("CountLHE", hSumOfLHEweights);

  for(Int_t k = 1; k < nFiles; k++){
    f = TFile::Open(files.at(k));
    //cout << "Opening: " << files.at(k) << endl;
    f->GetObject("CountLHE", htemp);
    hSumOfLHEweights->Add(htemp);
  }
  hSumOfLHEweights->SetDirectory(0);
  return hSumOfLHEweights;
}



TTreeFormula* Looper::GetFormula(TString name, TString var){
  TTreeFormula *f = new TTreeFormula(name, var, tree);
  f->GetNdata();
  return f;
}

void Looper::loadHisto2D(TString Path_to_histo, TString histo_name){

}

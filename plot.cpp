//######################################
//
// Sensitivity studies: plot spectra
//
//######################################

#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TStyle.h"

#include "constants.h"

int main(){

  TCanvas *c1 = new TCanvas("c1","test",1400,1200);
  c1->Divide(2,2);

  TFile *f1 = new TFile("/home/hep/pfranchi/SuperNEMO/testMC/1/se82_0nubb.root");  
  TFile *f2 = new TFile("/home/hep/pfranchi/SuperNEMO/testMC/1/se82_2nubb.root");  
  TFile *f3 = new TFile("/home/hep/pfranchi/SuperNEMO/testMC/1/tl208.root");  
  TFile *f4 = new TFile("/home/hep/pfranchi/SuperNEMO/testMC/1/bi214.root");  

  TTree *t1 = (TTree*)f1->Get("Sensitivity");
  TTree *t2 = (TTree*)f2->Get("Sensitivity");
  TTree *t3 = (TTree*)f3->Get("Sensitivity");
  TTree *t4 = (TTree*)f4->Get("Sensitivity");

  TH1F* se0n_2e_energy = new TH1F("se0n_2e_energy","Se 0nu spectrum", 100, 0., 5.0);
  TH1F* se2n_2e_energy = new TH1F("se2n_2e_energy","Se 2nu spectrum", 100, 0., 5.0);
  TH1F* tl_2e_energy = new TH1F("tl_2e_energy","Tl208 spectrum", 100, 0., 5.0);
  TH1F* bi_2e_energy = new TH1F("bi_2e_energy","Bi214 spectrum", 100, 0., 5.0);
  se0n_2e_energy->GetXaxis()->SetTitle("E(e_{1}e_{2}) [MeV]");
  se0n_2e_energy->GetYaxis()->SetTitle("dN/dE");
  se2n_2e_energy->GetXaxis()->SetTitle("E(e_{1}e_{2}) [MeV]");
  se2n_2e_energy->GetYaxis()->SetTitle("dN/dE");
  tl_2e_energy->GetXaxis()->SetTitle("E(e_{1}e_{2}) [MeV]");
  tl_2e_energy->GetYaxis()->SetTitle("dN/dE");
  bi_2e_energy->GetXaxis()->SetTitle("E(e_{1}e_{2}) [MeV]");
  bi_2e_energy->GetYaxis()->SetTitle("dN/dE");

  Double_t se0n_reco_higher_electron_energy;
  Double_t se0n_reco_lower_electron_energy;
  Double_t se2n_reco_higher_electron_energy;
  Double_t se2n_reco_lower_electron_energy;
  Double_t tl_reco_higher_electron_energy;
  Double_t tl_reco_lower_electron_energy;
  Double_t bi_reco_higher_electron_energy;
  Double_t bi_reco_lower_electron_energy;

  t1->SetBranchAddress("reco.higher_electron_energy",&se0n_reco_higher_electron_energy);
  t1->SetBranchAddress("reco.lower_electron_energy",&se0n_reco_lower_electron_energy);
  t2->SetBranchAddress("reco.higher_electron_energy",&se2n_reco_higher_electron_energy);
  t2->SetBranchAddress("reco.lower_electron_energy",&se2n_reco_lower_electron_energy);
  t3->SetBranchAddress("reco.higher_electron_energy",&tl_reco_higher_electron_energy);
  t3->SetBranchAddress("reco.lower_electron_energy",&tl_reco_lower_electron_energy);
  t4->SetBranchAddress("reco.higher_electron_energy",&bi_reco_higher_electron_energy);
  t4->SetBranchAddress("reco.lower_electron_energy",&bi_reco_lower_electron_energy);

  // Read all entries and fill the spectrum histograms

  Long64_t nentries = t1->GetEntries();
  for (Long64_t i=0;i<nentries;i++) {
    t1->GetEntry(i);
    if ((se0n_reco_higher_electron_energy + se0n_reco_lower_electron_energy) > 0 )
      se0n_2e_energy->Fill(se0n_reco_higher_electron_energy + se0n_reco_lower_electron_energy);
  }

  nentries = t2->GetEntries();
  for (Long64_t i=0;i<nentries;i++) {
    t2->GetEntry(i);
    if ((se2n_reco_higher_electron_energy + se2n_reco_lower_electron_energy) > 0 )
      se2n_2e_energy->Fill(se2n_reco_higher_electron_energy + se2n_reco_lower_electron_energy);
  }

  nentries = t3->GetEntries();
  for (Long64_t i=0;i<nentries;i++) {
    t3->GetEntry(i);
    if ((tl_reco_higher_electron_energy + tl_reco_lower_electron_energy) > 0 )
      tl_2e_energy->Fill(tl_reco_higher_electron_energy + tl_reco_lower_electron_energy);
  }

  nentries = t4->GetEntries();
  for (Long64_t i=0;i<nentries;i++) {
    t4->GetEntry(i);
    if ((bi_reco_higher_electron_energy + bi_reco_lower_electron_energy) > 0 )
      bi_2e_energy->Fill(bi_reco_higher_electron_energy + bi_reco_lower_electron_energy);
  }


  // Plot spectra

  c1->cd(1);
  gPad->SetLogy();
  se0n_2e_energy->Draw();
  c1->cd(2);
  gPad->SetLogy();
  se2n_2e_energy->Draw();
  c1->cd(3);
  gPad->SetLogy();
  tl_2e_energy->Draw();
  c1->cd(4);
  gPad->SetLogy();
  bi_2e_energy->Draw();

  // Save plots

  c1->SaveAs("2e_energy.pdf");   
  system("xpdf 2e_energy.pdf"); 


  // Normalization:

  Double_t N_0n, N_2n, N_tl, N_bi;



}

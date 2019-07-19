//###########################
//
// Sensitivity studies
//  - Bi214
//
//########################### 

#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TF1.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TCut.h"
#include "TRandom3.h"
#include <iostream>

#include "constants.h"

TH1F* h_bi214_source_bulk_length_alpha = new TH1F("h_bi214_source_bulk_length_alpha","Bi214 foil bulk: alpha length", 100, 0., 500.);
TH1F* h_bi214_source_surface_length_alpha = new TH1F("h_bi214_source_surface_length_alpha","Bi214 foil surface: alpha length", 100, 0., 500.);
TH1F* h_bi214_tracker_surface_length_alpha = new TH1F("h_bi214_tracker_surface_length_alpha","Bi214 tracker: alpha length", 100, 0., 500.);

TH1F* h_bi214_length_alpha_exp = new TH1F("h_bi214_length_alpha_exp","Bi214: alpha length - pseudo experiment", 100, 0., 500.);


// define fit function for 3 histograms
Double_t fitfunc(Double_t *x, Double_t *par){

  Double_t xx = x[0];
  Int_t bin = h_bi214_source_bulk_length_alpha->GetXaxis()->FindBin(xx);
  Double_t source_bulk = h_bi214_source_bulk_length_alpha->GetBinContent(bin);
  Double_t source_surface = h_bi214_source_surface_length_alpha->GetBinContent(bin);
  Double_t tracker_surface = h_bi214_tracker_surface_length_alpha->GetBinContent(bin);
  
  Double_t rs = par[0]/h_bi214_source_bulk_length_alpha->Integral()*source_bulk+ \
    par[1]/h_bi214_source_surface_length_alpha->Integral()*source_surface+ \
    par[2]/h_bi214_tracker_surface_length_alpha->Integral()*tracker_surface;

  return rs;

}


void do_contribution(TFile *f, TH1F* h_length_alpha, Double_t &efficiency){

  TTree *t = (TTree*)f->Get("Sensitivity");

  // Variables:

  Double_t alpha_track_length;
  Bool_t topology_1e1alpha;
  Double_t first_vertex_x;
  Double_t second_vertex_x;
  Int_t vertices_on_foil;

  t->SetBranchAddress("reco.alpha_track_length",&alpha_track_length);
  t->SetBranchAddress("reco.topology_1e1alpha",&topology_1e1alpha);
  t->SetBranchAddress("reco.first_vertex_x",&first_vertex_x);
  t->SetBranchAddress("reco.second_vertex_x",&second_vertex_x);
  t->SetBranchAddress("reco.vertices_on_foil",&vertices_on_foil);

  h_length_alpha->GetXaxis()->SetTitle("alpha length [mm]");
  h_length_alpha->GetYaxis()->SetTitle("Events/bin");

  //TCut source_selection= "(alpha_track_length > 0) && (topology_1e1alpha == false)";
  //TCut tracker_selection= "";

  // Read all entries and fill the histograms:
  Long64_t nentries = t->GetEntries();
  for (Long64_t i=0;i<nentries;i++) {
    t->GetEntry(i);
    if ((alpha_track_length > 0) && (topology_1e1alpha == true) && (abs(first_vertex_x) < 0.125 && (abs(second_vertex_x) < 0.125)) && (vertices_on_foil==2))
      h_length_alpha->Fill(alpha_track_length);
  }

  // Selection efficiency:
  efficiency = h_length_alpha->GetEntries()/nentries;
  
}


int main(){

  // Load MC:

  /*  TFile *f1 = new TFile("/home/hep/pfranchi/SuperNEMO/MC/20190717-1545/bi214_source_bulk.root");  
  TFile *f2 = new TFile("/home/hep/pfranchi/SuperNEMO/MC/20190717-1545/bi214_source_surface.root");  
  TFile *f3 = new TFile("/home/hep/pfranchi/SuperNEMO/MC/20190717-1545/bi214_tracker_surface.root");  
  */
  TFile *f1 = new TFile("/home/hep/pfranchi/SuperNEMO/MC/20190718-1402/bi214_source_bulk.root");  
  TFile *f2 = new TFile("/home/hep/pfranchi/SuperNEMO/MC/20190718-1402/bi214_source_surface.root");  
  TFile *f3 = new TFile("/home/hep/pfranchi/SuperNEMO/MC/20190718-1402/bi214_tracker_surface.root");  
  
  // Histograms:

  // weere here...

  Double_t e_source_bulk;
  Double_t e_source_surface;
  Double_t e_tracker_surface;

  // Fill histograms:

  do_contribution(f1, h_bi214_source_bulk_length_alpha, e_source_bulk);
  do_contribution(f2, h_bi214_source_surface_length_alpha, e_source_surface);
  do_contribution(f3, h_bi214_tracker_surface_length_alpha, e_tracker_surface);

  std::cout << "Source bulk efficiency: " << e_source_bulk*100 << "%" << std::endl;
  std::cout << "Source surface efficiency: " << e_source_surface*100 << "%" << std::endl;
  std::cout << "Source tracker surface: " << e_tracker_surface*100 << "%" << std::endl << std::endl;

  // Plot histograms:

  TCanvas *c1 = new TCanvas("c1","Radon",1400,900);
  c1->Divide(2,2);

  c1->cd(1);
  //gPad->SetLogy();
  h_bi214_source_bulk_length_alpha->Draw();
  c1->cd(2);
  //gPad->SetLogy();
  h_bi214_source_surface_length_alpha->Draw();
  c1->cd(3);
  //gPad->SetLogy();
  h_bi214_tracker_surface_length_alpha->Draw();


  // Normalization:

  Double_t N_source_bulk, N_source_surface, N_tracker_surface, N_total;

  Double_t exp = 30*24*60*60; // exposure 60days

  N_source_bulk = e_source_bulk*source_bulk_activity*exp;
  N_source_surface = e_source_surface*source_surface_activity*exp;
  N_tracker_surface = e_tracker_surface*tracker_surface_activity*exp;
  N_total = N_source_bulk+N_source_surface+N_tracker_surface;

  std::cout << "Events bulk efficiency: " << N_source_bulk << std::endl;
  std::cout << "Events surface efficiency: " << N_source_surface << std::endl;
  std::cout << "Events tracker surface: " << N_tracker_surface << std::endl;
  std::cout << "  Total number of events: " << N_total << std::endl << std::endl;

  h_bi214_source_bulk_length_alpha->Scale(N_source_bulk/h_bi214_source_bulk_length_alpha->Integral());
  h_bi214_source_surface_length_alpha->Scale(N_source_surface/h_bi214_source_surface_length_alpha->Integral());
  h_bi214_tracker_surface_length_alpha->Scale(N_tracker_surface/h_bi214_tracker_surface_length_alpha->Integral());

  h_bi214_source_bulk_length_alpha->SetLineColor(kRed);
  h_bi214_source_surface_length_alpha->SetLineColor(kGreen+3);
  h_bi214_tracker_surface_length_alpha->SetLineColor(kBlue);

  h_bi214_source_bulk_length_alpha->SetFillStyle(3003);
  h_bi214_source_surface_length_alpha->SetFillStyle(3003);
  h_bi214_tracker_surface_length_alpha->SetFillStyle(3003);
  h_bi214_source_bulk_length_alpha->SetFillColorAlpha(kRed,.60);
  h_bi214_source_surface_length_alpha->SetFillColorAlpha(kGreen+3,.60);
  h_bi214_tracker_surface_length_alpha->SetFillColorAlpha(kBlue,.60);

  TH1F* h_bi214_length_alpha = new TH1F("h_bi214_length_alpha","Bi214: alpha length", 100, 0., 500.);
  h_bi214_length_alpha->Add(h_bi214_source_bulk_length_alpha);
  h_bi214_length_alpha->Add(h_bi214_source_surface_length_alpha);
  h_bi214_length_alpha->Add(h_bi214_tracker_surface_length_alpha);

  h_bi214_length_alpha->SetLineColor(kBlack);

  c1->cd(4);
  h_bi214_length_alpha->Draw();
  h_bi214_source_bulk_length_alpha->Draw("same");
  h_bi214_source_surface_length_alpha->Draw("sames");
  h_bi214_tracker_surface_length_alpha->Draw("sames");
  h_bi214_length_alpha->Draw("sames");

  // Save plots:

  c1->SaveAs("alpha_length.png");   
  //system("display alpha_length.png"); 

  // One pseudo-experiment:

  // randomize the number of events:
  TRandom3 *rand = new TRandom3();
  Int_t N_experiment;
  N_experiment= rand->TRandom3::Poisson(N_source_bulk+N_source_surface+N_tracker_surface);
  N_experiment= rand->TRandom3::Poisson(N_source_bulk+N_source_surface+N_tracker_surface);
  std::cout << "Experiment randomized event number: " << N_experiment << std::endl;

  TH1F* h_bi214_length_alpha_exp = new TH1F("h_bi214_length_alpha_exp","Bi214: alpha length - pseudo experiment", 100, 0., 500.);

  for ( Int_t bin = 0; bin < h_bi214_length_alpha->GetNbinsX(); bin++ )
    h_bi214_length_alpha_exp->SetBinContent(bin,h_bi214_length_alpha->GetBinContent(bin)/N_total*N_experiment);
  

  std::cout << "Experiment event number: " << h_bi214_length_alpha_exp->Integral() << std::endl;

  TCanvas *c2 = new TCanvas("c2","test",900,600);
  c2->cd();
  h_bi214_length_alpha_exp->Draw("E");

  // Fit:

  TF1 *func = new TF1("func",fitfunc,0,500,3);
  func->SetParameters(N_source_bulk,N_source_surface,N_tracker_surface);
  h_bi214_length_alpha_exp->Fit("func");

  c2->SaveAs("alpha_length_fit.png");
  system("display *.png");  

  // Multiple pseudo-experiments:


}

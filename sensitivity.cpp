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
#include "TGraph.h"
#include "TStyle.h"
#include "TCut.h"

#include "constants.h"

void do_contribution(TFile *f, TH1F* h_length_alpha){

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

  TCut source_selection= "(alpha_track_length > 0) && (topology_1e1alpha == false)";
  //TCut tracker_selection= "";

  // Read all entries and fill the histograms:

  Long64_t nentries = t->GetEntries();
  for (Long64_t i=0;i<nentries;i++) {
    t->GetEntry(i);
    if ((alpha_track_length > 0) && (topology_1e1alpha == true) && (abs(first_vertex_x) < 0.13 && (abs(second_vertex_x) < 0.13)) && (vertices_on_foil==2))
      h_length_alpha->Fill(alpha_track_length);
  }
  
}


int main(){

  // Load MC:

  TFile *f1 = new TFile("/home/hep/pfranchi/SuperNEMO/MC/20190717-1545/bi214_source_bulk.root");  
  TFile *f2 = new TFile("/home/hep/pfranchi/SuperNEMO/MC/20190717-1545/bi214_source_surface.root");  
  TFile *f3 = new TFile("/home/hep/pfranchi/SuperNEMO/MC/20190717-1545/bi214_tracker_surface.root");  

  // Histograms:

  TH1F* h_bi214_source_bulk_length_alpha = new TH1F("h_bi214_source_bulk_length_alpha","Bi214 foil bulk: alpha length", 100, 0., 500.);
  TH1F* h_bi214_source_surface_length_alpha = new TH1F("h_bi214_source_surface_length_alpha","Bi214 foil surface: alpha length", 100, 0., 500.);
  TH1F* h_bi214_tracker_surface_length_alpha = new TH1F("h_bi214_tracker_surface_length_alpha","Bi214 tracker: alpha length", 100, 0., 500.);

  // Fill histograms:

  do_contribution(f1, h_bi214_source_bulk_length_alpha);
  do_contribution(f2, h_bi214_source_surface_length_alpha);
  do_contribution(f3, h_bi214_tracker_surface_length_alpha);

  // Plot histograms:

  TCanvas *c1 = new TCanvas("c1","test",1900,400);
  c1->Divide(3,1);

  c1->cd(1);
  //gPad->SetLogy();
  h_bi214_source_bulk_length_alpha->Draw();
  c1->cd(2);
  //gPad->SetLogy();
  h_bi214_source_surface_length_alpha->Draw();
  c1->cd(3);
  //gPad->SetLogy();
  h_bi214_tracker_surface_length_alpha->Draw();
 
  // Save plots:

  c1->SaveAs("alpha_length.png");   
  system("display alpha_length.png"); 

  // Normalization:

  //Double_t N_0n, N_2n, N_tl, N_bi;



}

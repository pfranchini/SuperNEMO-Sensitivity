//###############################################
//
// Sensitivity studies
//  - Bi214
//
// P. Franchini - p.franchini@imperial.ac.uk
//
//###############################################

#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TCut.h"
#include "TRandom3.h"
#include "TError.h"
#include "TLegend.h"
#include "TString.h"
#include "TColor.h"
#include <iostream>

//////////////////////////////////////////////////////////////////////////////////////////
Int_t N_experiments = 10000;      // Number of pseudo experiments for each exposure
TString selection = "SOURCE";   // Selection: [SOURCE, TRACKER]
Int_t nbinsx = 40;              // Scan resolution in X
Int_t nbinsy = 40;              // Scan resolution in Y
//////////////////////////////////////////////////////////////////////////////////////////

Double_t N_source_bulk, N_source_surface, N_tracker_surface, N_total;
Double_t e_source_bulk;
Double_t e_source_surface;
Double_t e_tracker_surface;

TCanvas *c1 = new TCanvas("c1","Radon",1400,900);
TCanvas *c2 = new TCanvas("c2","Bi contributions",900,600);
TCanvas *c3 = new TCanvas("c3","Fitted number of events",1800,600);
TCanvas *c4 = new TCanvas("c4","Exposure plots",900,1000);

// PDFs for the various contributios
TH1F* h_bi214_source_bulk_length_alpha = new TH1F("h_bi214_source_bulk_length_alpha","Bi214 foil bulk: alpha length", 100, 0., 500.);
TH1F* h_bi214_source_surface_length_alpha = new TH1F("h_bi214_source_surface_length_alpha","Bi214 foil surface: alpha length", 100, 0., 500.);
TH1F* h_bi214_tracker_surface_length_alpha = new TH1F("h_bi214_tracker_surface_length_alpha","Bi214 tracker: alpha length", 100, 0., 500.);
TH1F* h_bi214_length_alpha = new TH1F("h_bi214_length_alpha","Bi214: alpha length", 100, 0., 500.);

// mock dataset
TH1F* h_bi214_length_alpha_exp = new TH1F("h_bi214_length_alpha_exp","Bi214: alpha length - pseudo experiment", 100, 0., 500.);

TRandom3 *rand1 = new TRandom3();
TRandom3 *rand2 = new TRandom3();
TRandom3 *rand3 = new TRandom3();

// plot for exposures
TGraph* bulk_events = new TGraph();
TGraph* surface_events = new TGraph();
TGraph* tracker_events = new TGraph();
TGraph* bulk_errors = new TGraph();
TGraph* surface_errors = new TGraph();
TGraph* tracker_errors = new TGraph();

// scan plot
TH2F *bulk_scan = new TH2F("bulk_scan","Bulk relative error [%]",nbinsx, 0., 10., nbinsy, 1., 1000.);
Double_t source_bulk_activity;
Double_t source_surface_activity = 3.2214e-03;
Double_t tracker_surface_activity = 38.0786e-03;

// define fit function for 3 histograms
Double_t fitfunc(Double_t *x, Double_t *par){

  Double_t xx = x[0];
  Int_t bin1 = h_bi214_source_bulk_length_alpha->GetXaxis()->FindBin(xx);
  Int_t bin2 = h_bi214_source_surface_length_alpha->GetXaxis()->FindBin(xx);
  Int_t bin3 = h_bi214_tracker_surface_length_alpha->GetXaxis()->FindBin(xx);
  Double_t source_bulk = h_bi214_source_bulk_length_alpha->GetBinContent(bin1);
  Double_t source_surface = h_bi214_source_surface_length_alpha->GetBinContent(bin2);
  Double_t tracker_surface = h_bi214_tracker_surface_length_alpha->GetBinContent(bin3);
  
  Double_t total = par[0]/h_bi214_source_bulk_length_alpha->Integral()*source_bulk+ \
                   par[1]/h_bi214_source_surface_length_alpha->Integral()*source_surface+ \
                   par[2]/h_bi214_tracker_surface_length_alpha->Integral()*tracker_surface;

  return total;

}

TF1 *func = new TF1("func",fitfunc,0,500,3);

void do_fit(){

  // Randomize single contributions
  Double_t N_source_bulk_rand = rand1->TRandom3::PoissonD(N_source_bulk);
  Double_t N_source_surface_rand = rand2->TRandom3::PoissonD(N_source_surface);
  Double_t N_tracker_surface_rand = rand3->TRandom3::PoissonD(N_tracker_surface);
  //std::cout << "Experiment randomized event number: " << N_source_bulk_rand+N_source_surface_rand+N_tracker_surface_rand << std::endl;
  
  // Create new pseudo-experiment histogram:
  for ( Int_t bin = 0; bin < h_bi214_length_alpha->GetNbinsX(); bin++ )
    //    h_bi214_length_alpha_exp->SetBinContent(bin,h_bi214_length_alpha->GetBinContent(bin)/N_total*N);
    h_bi214_length_alpha_exp->SetBinContent(bin, \
					    h_bi214_source_bulk_length_alpha->GetBinContent(bin)/N_source_bulk*N_source_bulk_rand + \
					    h_bi214_source_surface_length_alpha->GetBinContent(bin)/N_source_surface*N_source_surface_rand + \
					    h_bi214_tracker_surface_length_alpha->GetBinContent(bin)/N_tracker_surface*N_tracker_surface_rand );
  //std::cout << "Experiment event number: " << h_bi214_length_alpha_exp->Integral() << std::endl;
  
  // Fit:
  func->SetParameters(N_source_bulk,N_source_surface,N_tracker_surface);
  h_bi214_length_alpha_exp->Fit("func","q"); // "q" quite option
  
}

// Full pseudo-experiment for one single exposure [days]
void do_exposure(Int_t days){

  std::cout << "Exposure: " << days << " days"  << std::endl;

  // pseudo-experiments results
  
  TH1F* bulk    = new TH1F("bulk","Source bulk", 100, source_bulk_activity-0.4*(source_bulk_activity), source_bulk_activity+0.4*(source_bulk_activity));
  //TH1F* surface = new TH1F("surface","Source surface", 100, source_surface_activity-0.1*(source_surface_activity), source_surface_activity+0.1*(source_surface_activity));
  //TH1F* tracker = new TH1F("tracker","Tracker surface", 100, tracker_surface_activity-0.1*(tracker_surface_activity), tracker_surface_activity+0.1*(tracker_surface_activity));

  // Normalization based on exposure:
  Double_t exposure = days*24*60*60; // exposure in seconds

  N_source_bulk = e_source_bulk*source_bulk_activity*exposure;
  N_source_surface = e_source_surface*source_surface_activity*exposure;
  N_tracker_surface = e_tracker_surface*tracker_surface_activity*exposure;
  N_total = N_source_bulk+N_source_surface+N_tracker_surface;

  h_bi214_source_bulk_length_alpha->Scale(N_source_bulk/h_bi214_source_bulk_length_alpha->Integral());
  h_bi214_source_surface_length_alpha->Scale(N_source_surface/h_bi214_source_surface_length_alpha->Integral());
  h_bi214_tracker_surface_length_alpha->Scale(N_tracker_surface/h_bi214_tracker_surface_length_alpha->Integral());
  
  // Multiple pseudo-experiments for one exposure:
  
  for (Long64_t i=0;i<N_experiments;i++) {
    
    do_fit();

    bulk->Fill(func->GetParameter(0)/exposure/e_source_bulk);
    //surface->Fill(func->GetParameter(1)/exposure/e_source_surface);
    //tracker->Fill(func->GetParameter(2)/exposure/e_tracker_surface);
    
  }

  Int_t bulk_fit = bulk->Fit("gaus","wq");
  //  Int_t surface_fit = surface->Fit("gaus","q");
  //  Int_t tracker_fit = tracker->Fit("gaus","q");

  /* Fill TGraphs for each contribution
  if (bulk_fit==0) bulk_events->SetPoint(bulk_events->GetN()+1,days, bulk->GetFunction("gaus")->GetParameter(1)*exposure*e_source_bulk);
  if (surface_fit==0) surface_events->SetPoint(surface_events->GetN()+1,days, surface->GetFunction("gaus")->GetParameter(1)*exposure*e_source_surface);
  if (tracker_fit==0) tracker_events->SetPoint(tracker_events->GetN()+1,days, tracker->GetFunction("gaus")->GetParameter(1)*exposure*e_tracker_surface);
  if (bulk_fit==0) bulk_errors->SetPoint(bulk_errors->GetN()+1,days, bulk->GetFunction("gaus")->GetParameter(2)/bulk->GetFunction("gaus")->GetParameter(1)*100);
  if (surface_fit==0) surface_errors->SetPoint(surface_errors->GetN()+1,days, surface->GetFunction("gaus")->GetParameter(2)/surface->GetFunction("gaus")->GetParameter(1)*100);
  if (tracker_fit==0) tracker_errors->SetPoint(tracker_errors->GetN()+1,days, tracker->GetFunction("gaus")->GetParameter(2)/tracker->GetFunction("gaus")->GetParameter(1)*100);*/

  // Fill activity | time | error
  if (bulk_fit==0) {
    std::cout << source_bulk_activity*1e3 << "  " << days << "  " << bulk->GetFunction("gaus")->GetParameter(2)/bulk->GetFunction("gaus")->GetParameter(1)*100 << std::endl;
    if (( bulk->GetFunction("gaus")->GetParameter(1) >0 ) && ( bulk->GetFunction("gaus")->GetParameter(2)/bulk->GetFunction("gaus")->GetParameter(1)*100 < 100 )) {
      bulk_scan->Fill(source_bulk_activity*1e3, days, bulk->GetFunction("gaus")->GetParameter(2)/bulk->GetFunction("gaus")->GetParameter(1)*100);
    }
  }

  TFile *output = new TFile("bulk_scan_fits.root","UPDATE");
  output->cd();
  bulk->Write();
  output->Close();
  delete output;

  delete bulk;
  //delete surface;
  //delete tracker;

}

void do_contribution(TChain *t, TH1F* h_length_alpha, Double_t &efficiency){

  std::cout << h_length_alpha->GetTitle() << " entries: " << t->GetEntries() << std::endl;
  
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
  
  // Read all entries and fill the histograms:
  Long64_t nentries = t->GetEntries();
  for (Long64_t i=0;i<nentries;i++) {
    t->GetEntry(i);
    
    // Selections:
    if (selection == "SOURCE")
      // Source selection:
      if ((alpha_track_length > 0) && (topology_1e1alpha == true) && (abs(first_vertex_x) < 0.125 && (abs(second_vertex_x) < 0.125)) && (vertices_on_foil==2))
	h_length_alpha->Fill(alpha_track_length);
    
    if (selection == "TRACKER") 
      // Tracker selection:
      if ((alpha_track_length > 0) && (topology_1e1alpha == true) && (abs(first_vertex_x) > 0.125 && (abs(second_vertex_x) > 0.125)) && (vertices_on_foil==0))
	h_length_alpha->Fill(alpha_track_length);
    
    
  // Selection efficiency:
  efficiency = h_length_alpha->GetEntries()/nentries;
  
  }
}

////////////////////////////////////////////////////////////////////////

int main(){

  gErrorIgnoreLevel = kWarning; // prints only warnings
  gStyle->SetOptStat(0);

  // Load MC:

  std::cout << "Loading MC from files..." << std::endl;
  TChain *d1 = new TChain("Sensitivity");
  TChain *d2 = new TChain("Sensitivity");
  TChain *d3 = new TChain("Sensitivity");

  // can add multiple root files with *  
  d1->Add("/home/hep/pfranchi/SuperNEMO/MC/20190916-1323/bi214_source_internal_bulk.root");
  //d1->Add("/home/hep/pfranchi/SuperNEMO/MC/Lyon/damned_sn_reco_5/file_?.root");
  d2->Add("/home/hep/pfranchi/SuperNEMO/MC/20190718-1402/bi214_source_surface.root");
  d3->Add("/home/hep/pfranchi/SuperNEMO/MC/20190718-1402/bi214_tracker_surface.root");

  // Filling histograms, calculate efficiency and create PDFs:

  std::cout << "Fill histograms with " << selection << " selections..." << std::endl << std::endl;
  do_contribution(d1, h_bi214_source_bulk_length_alpha, e_source_bulk);
  do_contribution(d2, h_bi214_source_surface_length_alpha, e_source_surface);
  do_contribution(d3, h_bi214_tracker_surface_length_alpha, e_tracker_surface);
  
  std::cout << "Source bulk efficiency: " << e_source_bulk*100 << "%" << std::endl;
  std::cout << "Source surface efficiency: " << e_source_surface*100 << "%" << std::endl;
  std::cout << "Tracker surface efficiency: " << e_tracker_surface*100 << "%" << std::endl << std::endl;

  // kBird for ROOT 5                                                                                                                                                      
  Double_t red[9]   = { 0.2082, 0.0592, 0.0780, 0.0232, 0.1802, 0.5301, 0.8186, 0.9956, 0.9764};
  Double_t green[9] = { 0.1664, 0.3599, 0.5041, 0.6419, 0.7178, 0.7492, 0.7328, 0.7862, 0.9832};
  Double_t blue[9]  = { 0.5293, 0.8684, 0.8385, 0.7914, 0.6425, 0.4662, 0.3499, 0.1968, 0.0539};
  Double_t stops[9] = { 0.0000, 0.1250, 0.2500, 0.3750, 0.5000, 0.6250, 0.7500, 0.8750, 1.0000};
  Int_t nb=255;
  TColor::CreateGradientColorTable(9, stops, red, green, blue, nb);

  bulk_scan->GetXaxis()->SetTitle("Activity [mBq]");
  bulk_scan->GetYaxis()->SetTitle("Time [days]");

  c1->cd();
  //gPad->SetLogx();
  //gPad->SetLogy();

  TFile *output = new TFile("bulk_scan_fits.root","RECREATE");
  output->Close();
  delete output;

  // Multiple activities for the bulk (keep the other fixed)
  for ( source_bulk_activity=10e-03/bulk_scan->GetNbinsX()/2; source_bulk_activity<=10e-03; source_bulk_activity+=10e-03/bulk_scan->GetNbinsX() ) {

    std::cout << std::endl << "Bulk activity: " << source_bulk_activity  << std::endl;
    
    for (Long64_t i=10; i<=1000; i+=1000/bulk_scan->GetNbinsY())
      do_exposure(i);
    
  }
  
  bulk_scan->Draw("colz");
  c1->SaveAs("bulk_scan.png");
  c1->SaveAs("bulk_scan.pdf");
  system("pkill display");
  system("display bulk_scan.png &");

  TFile *output1 = new TFile("bulk_scan.root","UPDATE");
  output1->cd();
  bulk_scan->Write();
  output1->Close();
  
}
 



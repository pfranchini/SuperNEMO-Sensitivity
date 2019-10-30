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
#include "TF1.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TCut.h"
#include "TRandom3.h"
#include "TError.h"
#include "TLegend.h"
#include "TString.h"
#include <iostream>

#include "constants.h"

//////////////////////////////////////////////////////////////////////////////////////////
Bool_t do_plot = false;         // Creates example plots for one exposure
Int_t N_experiments = 10000;    // Number of pseudo experiments for each exposure
TString selection = "SOURCE";   // Selection: [SOURCE, TRACKER]
Int_t step=10;                  // Steps for the exposure [day]
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
    //h_bi214_length_alpha_exp->SetBinContent(bin,h_bi214_length_alpha->GetBinContent(bin)/N_total*N);
    h_bi214_length_alpha_exp->SetBinContent(bin, \
					    h_bi214_source_bulk_length_alpha->GetBinContent(bin)/N_source_bulk*N_source_bulk_rand + \
					    h_bi214_source_surface_length_alpha->GetBinContent(bin)/N_source_surface*N_source_surface_rand + \
					    h_bi214_tracker_surface_length_alpha->GetBinContent(bin)/N_tracker_surface*N_tracker_surface_rand );
  //std::cout << "Experiment event number: " << h_bi214_length_alpha_exp->Integral() << std::endl;
  
  // Fit:
  func->SetParameters(N_source_bulk,N_source_surface,N_tracker_surface);
  h_bi214_length_alpha_exp->Fit("func", "q"); // "q" quite option
  
}

// Full pseudo-experiment for one single exposure [days]
void do_exposure(Int_t days){

  std::cout << std::endl << "Exposure: " << days << " day(s)"  << std::endl;

  // pseudo-experiments results
  
  //TH1F* bulk    = new TH1F("bulk","Source bulk", 100, source_bulk_activity-0.1*(source_bulk_activity), source_bulk_activity+0.1*(source_bulk_activity));
  //TH1F* surface = new TH1F("surface","Source surface", 100, source_surface_activity-0.1*(source_surface_activity), source_surface_activity+0.1*(source_surface_activity));
  //TH1F* tracker = new TH1F("tracker","Tracker surface", 100, tracker_surface_activity-0.1*(tracker_surface_activity), tracker_surface_activity+0.1*(tracker_surface_activity));

  TH1F* bulk    = new TH1F("bulk","Source bulk", 100, source_bulk_activity-0.4*(source_bulk_activity), source_bulk_activity+0.4*(source_bulk_activity));
  TH1F* surface = new TH1F("surface","Source surface", 100, source_surface_activity-0.4*(source_surface_activity), source_surface_activity+0.4*(source_surface_activity));
  TH1F* tracker = new TH1F("tracker","Tracker surface", 100, tracker_surface_activity-0.4*(tracker_surface_activity), tracker_surface_activity+0.4*(tracker_surface_activity));

  //bulk->Reset();
  //surface->Reset();
  //tracker->Reset();

  // Normalization based on exposure:
  Double_t exposure = days*24*60*60; // exposure in seconds

  N_source_bulk = e_source_bulk*source_bulk_activity*exposure;
  N_source_surface = e_source_surface*source_surface_activity*exposure;
  N_tracker_surface = e_tracker_surface*tracker_surface_activity*exposure;
  N_total = N_source_bulk+N_source_surface+N_tracker_surface;

  h_bi214_source_bulk_length_alpha->Scale(N_source_bulk/h_bi214_source_bulk_length_alpha->Integral());
  h_bi214_source_surface_length_alpha->Scale(N_source_surface/h_bi214_source_surface_length_alpha->Integral());
  h_bi214_tracker_surface_length_alpha->Scale(N_tracker_surface/h_bi214_tracker_surface_length_alpha->Integral());
  
  if (do_plot){
    std::cout << "Events source bulk: " << N_source_bulk << std::endl;
    std::cout << "Events source surface: " << N_source_surface << std::endl;
    std::cout << "Events tracker surface: " << N_tracker_surface << std::endl;
    std::cout << "  Total number of events: " << N_total << std::endl << std::endl;
    
    h_bi214_source_bulk_length_alpha->SetLineColor(kRed);
    h_bi214_source_surface_length_alpha->SetLineColor(kGreen+3);
    h_bi214_tracker_surface_length_alpha->SetLineColor(kBlue);
    
    h_bi214_source_bulk_length_alpha->SetFillStyle(3003);
    h_bi214_source_surface_length_alpha->SetFillStyle(3003);
    h_bi214_tracker_surface_length_alpha->SetFillStyle(3003);
    h_bi214_source_bulk_length_alpha->SetFillColorAlpha(kRed,.60);
    h_bi214_source_surface_length_alpha->SetFillColorAlpha(kGreen+3,.60);
    h_bi214_tracker_surface_length_alpha->SetFillColorAlpha(kBlue,.60);
    
    h_bi214_length_alpha->Reset();
    h_bi214_length_alpha->Add(h_bi214_source_bulk_length_alpha);
    h_bi214_length_alpha->Add(h_bi214_source_surface_length_alpha);
    h_bi214_length_alpha->Add(h_bi214_tracker_surface_length_alpha);
    
    h_bi214_length_alpha->GetXaxis()->SetTitle("alpha length [mm]");
    h_bi214_length_alpha->GetYaxis()->SetTitle("Events/bin");
    h_bi214_length_alpha->SetLineColor(kBlack);
    
    c1->cd(4);
    h_bi214_length_alpha->Draw();
    h_bi214_source_bulk_length_alpha->Draw("same");
    h_bi214_source_surface_length_alpha->Draw("sames");
    h_bi214_tracker_surface_length_alpha->Draw("sames");
    h_bi214_length_alpha->Draw("sames");

    // Save plots:
    
    c1->SaveAs("alpha_length.png");   
    system("display alpha_length.png"); 
    
    // One pseudo-experiment:
    
    do_fit();
    
    // Plot contributions:
    
    c2->cd();
    
    h_bi214_source_bulk_length_alpha->Scale(func->GetParameter(0)/h_bi214_source_bulk_length_alpha->Integral());
    h_bi214_source_surface_length_alpha->Scale(func->GetParameter(1)/h_bi214_source_surface_length_alpha->Integral());
    h_bi214_tracker_surface_length_alpha->Scale(func->GetParameter(2)/h_bi214_tracker_surface_length_alpha->Integral());
    
    h_bi214_length_alpha_exp->SetLineWidth(3);
    h_bi214_length_alpha_exp->SetLineColor(kBlack);
    h_bi214_length_alpha_exp->GetXaxis()->SetTitle("alpha length [mm]");
    h_bi214_length_alpha_exp->GetYaxis()->SetTitle("Events/bin");
    
    h_bi214_length_alpha_exp->SetStats(false);
    h_bi214_source_bulk_length_alpha->SetStats(false);
    h_bi214_source_surface_length_alpha->SetStats(false);
    h_bi214_tracker_surface_length_alpha->SetStats(false);
    
    TLegend* legend = new TLegend(0.6,0.7,0.9,0.9);
    legend->AddEntry(h_bi214_source_bulk_length_alpha,"Bi214 source bulk","f");
    legend->AddEntry(h_bi214_source_surface_length_alpha,"Bi214 source surface","f");
    legend->AddEntry(h_bi214_tracker_surface_length_alpha,"Bi214 tracker surface","f");
    legend->AddEntry(h_bi214_length_alpha_exp,"Pseudo data","lep");

    h_bi214_length_alpha_exp->Draw("E");
    h_bi214_source_bulk_length_alpha->Draw("sames");
    h_bi214_source_surface_length_alpha->Draw("sames");
    h_bi214_tracker_surface_length_alpha->Draw("sames");
    legend->Draw();    

    c2->SaveAs("alpha_length_fit.png");
  }
  
  // Multiple pseudo-experiments for one exposure:
  
  std::cout << "Multiple pseudo-experiments..." << std::endl;

  for (Long64_t i=0;i<N_experiments;i++) {
    
    if (do_plot) std::cout << "\b\b\b\b\b" << i;
    
    do_fit();

    bulk->Fill(func->GetParameter(0)/exposure/e_source_bulk);
    surface->Fill(func->GetParameter(1)/exposure/e_source_surface);
    tracker->Fill(func->GetParameter(2)/exposure/e_tracker_surface);
    
  }

  Int_t bulk_fit = bulk->Fit("gaus","wq");
  Int_t surface_fit = surface->Fit("gaus","wq");
  Int_t tracker_fit = tracker->Fit("gaus","wq");

  // Fill TGraphs for each contribution
  if (bulk_fit==0) bulk_events->SetPoint(bulk_events->GetN()+1,days, bulk->GetFunction("gaus")->GetParameter(1)*exposure*e_source_bulk);
  if (surface_fit==0) surface_events->SetPoint(surface_events->GetN()+1,days, surface->GetFunction("gaus")->GetParameter(1)*exposure*e_source_surface);
  if (tracker_fit==0) tracker_events->SetPoint(tracker_events->GetN()+1,days, tracker->GetFunction("gaus")->GetParameter(1)*exposure*e_tracker_surface);
  if (bulk_fit==0) bulk_errors->SetPoint(bulk_errors->GetN()+1,days, bulk->GetFunction("gaus")->GetParameter(2)/bulk->GetFunction("gaus")->GetParameter(1)*100);
  if (surface_fit==0) surface_errors->SetPoint(surface_errors->GetN()+1,days, surface->GetFunction("gaus")->GetParameter(2)/surface->GetFunction("gaus")->GetParameter(1)*100);
  if (tracker_fit==0) tracker_errors->SetPoint(tracker_errors->GetN()+1,days, tracker->GetFunction("gaus")->GetParameter(2)/tracker->GetFunction("gaus")->GetParameter(1)*100);

  if (do_plot){

    // print results only for good fits (0)
    std::cout << std::endl << "Fit results: " << std::endl;
    if (bulk_fit==0)    std::cout << "  Source bulk events: " << bulk->GetFunction("gaus")->GetParameter(1)*exposure*e_source_bulk << " ± "<< \
			  bulk->GetFunction("gaus")->GetParameter(2)/sqrt(bulk->GetEntries())*exposure*e_source_bulk << std::endl;
    if (surface_fit==0) std::cout << "  Source surface events: " << surface->GetFunction("gaus")->GetParameter(1)*exposure*e_source_surface << " ± "<< \
			  surface->GetFunction("gaus")->GetParameter(2)/sqrt(surface->GetEntries())*exposure*e_source_surface << std::endl;
    if (tracker_fit==0) std::cout << "  Tracker surface events: " << tracker->GetFunction("gaus")->GetParameter(1)*exposure*e_tracker_surface << " ± "<< \
			  tracker->GetFunction("gaus")->GetParameter(2)/sqrt(tracker->GetEntries())*exposure*e_tracker_surface << std::endl;
    if (bulk_fit==0) std::cout << "  Source bulk error: " << bulk->GetFunction("gaus")->GetParameter(2)/bulk->GetFunction("gaus")->GetParameter(1)  << std::endl;
    if (surface_fit==0) std::cout << "  Source surface error: " << surface->GetFunction("gaus")->GetParameter(2)/surface->GetFunction("gaus")->GetParameter(1) << std::endl;
    if (tracker_fit==0) std::cout << "  Tracker surface error: " << tracker->GetFunction("gaus")->GetParameter(2)/tracker->GetFunction("gaus")->GetParameter(1) << std::endl;
    
    // plot results
    bulk->GetXaxis()->SetTitle("Activity [Bq]");
    surface->GetXaxis()->SetTitle("Activity [Bq]");
    tracker->GetXaxis()->SetTitle("Activity [Bq]");

    c3->cd(1);
    bulk->Draw();
    c3->cd(2);
    surface->Draw();
    c3->cd(3);
    tracker->Draw();
    
    c3->SaveAs("alpha_length_exp_fit.png");
    system("display alpha_length_exp_fit.png");

  }

  delete bulk;
  delete surface;
  delete tracker;

}

void do_contribution(TChain *t, TH1F* h_length_alpha, Double_t &efficiency){

  std::cout << h_length_alpha->GetTitle() << " entries: " << t->GetEntries() << std::endl;
  //  TTree *t = (TTree*)f->Get("Sensitivity");
  
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

  // Load MC:

  std::cout << "Loading MC from files..." << std::endl;
  TChain *d1 = new TChain("Sensitivity");
  TChain *d2 = new TChain("Sensitivity");
  TChain *d3 = new TChain("Sensitivity");

  // can add multiple root files with *  
  // Falaise 4.0.0
  d1->Add("/home/hep/pfranchi/SuperNEMO/MC/20191004-1238/bi214_source_internal_bulk.root");
  //d1->Add("/home/hep/pfranchi/SuperNEMO/MC/Lyon/damned_sn_reco_5/file_?.root");
  d2->Add("/home/hep/pfranchi/SuperNEMO/MC/20191004-1238/bi214_source_surface.root");
  d3->Add("/home/hep/pfranchi/SuperNEMO/MC/20191004-1238/bi214_tracker_surface.root");

  // Filling histograms, calculate efficiency and create PDFs:

  std::cout << "Fill histograms with " << selection << " selections..." << std::endl << std::endl;
  do_contribution(d1, h_bi214_source_bulk_length_alpha, e_source_bulk);
  do_contribution(d2, h_bi214_source_surface_length_alpha, e_source_surface);
  do_contribution(d3, h_bi214_tracker_surface_length_alpha, e_tracker_surface);
  
  std::cout << "Source bulk efficiency: " << e_source_bulk*100 << "%" << std::endl;
  std::cout << "Source surface efficiency: " << e_source_surface*100 << "%" << std::endl;
  std::cout << "Tracker surface efficiency: " << e_tracker_surface*100 << "%" << std::endl << std::endl;

  // Plot histograms:

  c1->Divide(2,2);
  c3->Divide(3,1);
  
  c1->cd(1);
  h_bi214_source_bulk_length_alpha->Draw();
  c1->cd(2);
  h_bi214_source_surface_length_alpha->Draw();
  c1->cd(3);
  h_bi214_tracker_surface_length_alpha->Draw();

  //if (do_plot) do_exposure(1); // 1 day
  //if (do_plot) do_exposure(7); // 7 days
  //if (do_plot) do_exposure(14); // 14 days
  //if (do_plot) do_exposure(30); // 30 days
  if (do_plot) do_exposure(60); // 60 days
  //if (do_plot) do_exposure(500); // 500 days

  // Multiple exposures:

  if (do_plot==false){  

    do_exposure(1); // 1 days
    do_exposure(7); // 7 days
    do_exposure(14); // 14 days
    do_exposure(30); // 30 days
    do_exposure(60); // 60 days
    
    for (Long64_t i=2; i<1010; i+=step) 
      do_exposure(i);
      
  }
  
  // Plot results:

  bulk_events->SetTitle("Number of events evolution");
  bulk_events->SetName("bulk_events");
  bulk_events->GetXaxis()->SetTitle("Exposure [days]");
  bulk_events->GetYaxis()->SetTitle("Number of expected events");
  bulk_events->SetMarkerStyle(20);
  bulk_events->SetMarkerColor(kRed);

  surface_events->SetTitle("Number of events evolution");
  surface_events->SetName("surface_events");
  surface_events->GetXaxis()->SetTitle("Exposure [days]");
  surface_events->GetYaxis()->SetTitle("Number of expected events");
  surface_events->SetMarkerStyle(20);
  surface_events->SetMarkerColor(kGreen+3);

  tracker_events->SetTitle("Number of events evolution");
  tracker_events->SetName("tracker_events");
  tracker_events->GetXaxis()->SetTitle("Exposure [days]");
  tracker_events->GetYaxis()->SetTitle("Number of expected events");
  tracker_events->SetMarkerStyle(20);
  tracker_events->SetMarkerColor(kBlue);

  bulk_errors->SetTitle("Relative errors evolution");
  bulk_errors->SetName("bulk_errors");
  bulk_errors->GetXaxis()->SetTitle("Exposure [days]");
  bulk_errors->GetYaxis()->SetTitle("Relative error [%]");
  bulk_errors->SetMarkerStyle(20);
  bulk_errors->SetMarkerColor(kRed);

  surface_errors->SetName("surface_errors");
  surface_errors->SetMarkerStyle(20);
  surface_errors->SetMarkerColor(kGreen+3);
  tracker_errors->SetName("tracker_errors");
  tracker_errors->SetMarkerStyle(20);
  tracker_errors->SetMarkerColor(kBlue);

  c4->Divide(1,2);
  
  c4->cd(1);  
  gPad->SetGrid();
  gPad->SetLogx();
  gPad->SetLogy();

  tracker_events->Draw("ap");
  bulk_events->Draw("p SAME");
  surface_events->Draw("p SAME");
  gPad->Modified(); gPad->Update();

  c4->cd(2);  
  gPad->SetGrid();
  gPad->SetLogx();
  bulk_errors->Draw("ap");
  surface_errors->Draw("p SAME");
  tracker_errors->Draw("p SAME");
  gPad->Modified(); gPad->Update();

  c4->SaveAs("exposures.png");   
  c4->SaveAs("exposures.pdf");   
  system("display exposures.png &"); 

  // Save some results on a root file
  TFile *output1 = new TFile("sensitivity.root","RECREATE");
  output1->cd();

  bulk_events->Write();
  surface_events->Write();
  tracker_events->Write();
  bulk_errors->Write();
  surface_errors->Write();
  tracker_errors->Write();

  output1->Close();

  
}
 



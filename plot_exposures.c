// Plot exposure results for manual fiddling:                                                                                                                              

{
  TFile *_file0 = TFile::Open("sensitivity.root");
  
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

  TCanvas *c4 = new TCanvas("c4","Exposure plots",900,1000);
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


}

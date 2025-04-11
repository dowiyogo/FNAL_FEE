// Description :  MACRO that prints a plot for the Noise measurements and fits them

// Usage : root MACRO_NoisePlotAndFit.cpp

// Customize the source file name and directory
TString fsource_directory = "/home/esteban/work/4d-tracking/data/lecroy-noise-rootformat/";
TString fsource_name      = "noise_amplitude_ntuple_3.root";

// Customize the output plot name and directory
TString output_directory = "/home/esteban/work/4d-tracking/plots/";
TString output_name      = "noise_amplitude_ntuple_3_FIT";

void MACRO_NoisePlotAndFit(){
  // PAD CUSTOM
  gStyle->SetOptStat(1110);
  gStyle->SetOptFit(111);
  gStyle->SetPadRightMargin(0.07);
  gStyle->SetPadTopMargin(0.05);

  TFile* fsource = new TFile(fsource_directory+fsource_name,"READ");

  TTree* ntuple_data = (TTree*) fsource->Get("ntuple_data");

  TH1F* histo = new TH1F("histo","",150,-0.001,0.001);

  ntuple_data->Draw("ampl>>histo","","E1");
  
  // AXIS CUSTOM
  histo->GetXaxis()->SetMaxDigits(2);
  histo->GetXaxis()->SetTitle("Amplitude[V]");
  // PAD CUSTOM
  gPad->SetGrid(1);
  //  gPad->SetLogy(1);
  histo->Draw("E1 SAME");

  // FIT
  TF1* func = new TF1("func","gaus",-0.001,0.001);
  func->SetLineColor(kRed);
  histo->Fit(func,"R");

  gPad->Print(output_directory+output_name+".pdf");
  gPad->Print(output_directory+output_name+".png");
}

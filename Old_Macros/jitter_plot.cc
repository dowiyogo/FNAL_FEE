   void jitter_plot()
{
    double jitter[5]     = {14.57,28.44,41.90,63.74,85.30};
    double jittererr[5] = { 0 , 0 , 0 , 0 , 0 };
    double SNR[5]        = {78.61,29.76,20.69,10.83,7.62};
    double SNRerr[5]     = { 0 , 0 , 0 , 0 , 0};
    TGraphErrors* g = new TGraphErrors(5,SNR,jitter,SNRerr,jittererr);

    g->SetMarkerStyle(8);
    g->SetMarkerColor(kBlue);
    g->SetLineColor(kBlue);

    g->Draw("AP");

    g->SetTitle("Resolution");
    g->GetXaxis()->SetTitle("Amp (mV)");
    g->GetYaxis()->SetTitle("Jitter (ps)");

    g->GetXaxis()->CenterTitle();
    g->GetYaxis()->CenterTitle();
    
    // Definir la función f(x) = a/x + b
    TF1* fitFunc = new TF1("fitFunc", "[0]/x + [1]", 5, 85);
    fitFunc->SetParameters(100, 0); // valores iniciales para a y b

    g->Fit(fitFunc, "R"); // "R" para restringir al rango definido en TF1

    fitFunc->SetLineColor(kRed);
    fitFunc->Draw("same");


    gPad->Print("/media/rene/Data/LGAD/plots/jitter-plot.pdf");
    gPad->Print("/media/rene/Data/LGAD/plots/jitter-plot.png");
    
/*
36 dB jitter 4.37591e-12   5.17856e-14  amp 7.92358e+02  RMS 2.10058e+00 risetime_80_20 2.94516e-10
40 dB jitter 7.24580e-12   1.03762e-13  amp 3.41789e+02  RMS 3.16146e+00 risetime_80_20
46 dB jitter 8.71976e-12   1.09741e-13  amp 2.12840e+02  RMS 3.19138e+00 risetime_80_20 
50 dB jitter 1.18503e-11   1.52358e-13  amp 1.56492e+02  RMS 1.01525e+00 risetime_80_20 
56 dB jitter 1.95018e-11   2.34880e-13  amp 7.61450e+01  RMS 1.42378e-01 risetime_80_20 
60 dB jitter 2.96304e-11   4.38063e-13  amp 5.53118e+01  RMS 1.41717e+00 risetime_80_20 
66 dB jitter 7.42528e-11   2.41172e-12  amp 2.44132e+01  RMS 5.39306e-01 risetime_80_20 

Double_t x[n]  = {14.57,28.44,41.90,63.74,85.30};//,81.84};
Double_t y[n]  = {78.61,29.76,20.69,10.83,7.62};//,5.27};
   

pulso+ lo demas
4.75295e-12 

7.24580e-12

Double_t Rise[6] = { }
Double_t Err_Rise[6] = { }
Double_t Amp[6] = {}
Double_t Err_Amp[6] = {}
Double_t Jitter[6] = {}
Double_t Res[6];
Double_t SNR[6];

for (i=1,i++, i < 6)
{
   SNR[i] = Amp[i] / 
   Res[i] = Rise[i]/SNR[i]
}


// error propagado division:  sqrt((sigma_X / X)^2+ (sigma_Y / Y)^2 +...)

Risetime ps:
341.13 
361.68
376.18

358.88
341.19
342.95
Noise RMS:

1.71 +- 0.89
2.04 +- 0.83
1.98 +- 1.03

2.54 +- 0.88
2.57 +- 0.93
4.83 +- 1.04

Amplitudes mV
24.91 +- 3.10
58.01 +- 4.33
82.97 +- 3.16

161.90 +- 14.9
219.21 +- 9.65
395.29 +- 3.95

*/
}

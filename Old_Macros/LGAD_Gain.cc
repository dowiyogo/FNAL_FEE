{
   auto c1 = new TCanvas("c1","Ch8 Gain",200,10,700,500);
   //c1->SetFillColor(42);
   c1->SetGrid();
   //c1->GetFrame()->SetFillColor(21);
   c1->GetFrame()->SetBorderSize(12);
   const Int_t n = 4;
   Double_t input[n]  = {5.18926,2.42101,1.69885,9.42003e-01};
   Double_t output[n]  = {3.24017e+02,2.08359e+02,1.48785e+02,7.52674e+01};
   Double_t ein[n] = {2.67243e-01,3.28376e-01,2.38383e-01,2.17144e-01};
   Double_t eout[n] = {3.39200,3.88844,4.00785,4.45948};
   auto gr = new TGraphErrors(n,input,output,ein,eout);
   //gr->SetTitle("Channel 8 Out vs Input Amplitudes");
   gr->SetMarkerColor(4);
   gr->SetMarkerStyle(21);
   gr->GetXaxis()->SetTitle("Input Pulse Amplitude [mV]");
   gr->GetYaxis()->SetTitle("Output Pulse Amplitude [mV]");
   gr->Draw("ALP");
   /*
1) 40 dB amplitude charge injector (input)
NO.   NAME      VALUE            ERROR          SIZE      DERIVATIVE
1  Constant     1.54439e+02   2.73237e+00   9.78129e-03   2.25031e-04
2  Mean         5.18926e+00   3.88254e-03   1.70726e-05  -3.87398e-02
3  Sigma        2.67243e-01   2.84753e-03   1.25696e-05   1.40691e-01
output
1  Constant     3.03283e+02   5.59361e+00   1.46061e-02   2.60005e-05
2  Mean         3.24017e+02   4.98836e-02   1.66329e-04   7.80703e-04
3  Sigma        3.39200e+00   4.06713e-02   1.04595e-05   3.24482e-02
2) 46 dB amplitude charge injector (input)
1  Constant     3.42260e+02   4.32918e+00   2.17492e-02  -3.35014e-05
2  Mean         2.42101e+00   3.51846e-03   2.17982e-05  -4.56793e-02
3  Sigma        3.28376e-01   2.70831e-03   1.39382e-05  -5.55996e-02
output
1  Constant     2.29206e+02   4.40771e+00   2.01600e-02   1.46639e-06
2  Mean         2.08359e+02   5.68775e-02   3.42163e-04  -7.13384e-04
3  Sigma        3.88844e+00   4.94705e-02   1.84337e-05  -1.62262e-02
3) 50 dB amplitude charge injector (input)
1  Constant     1.59461e+02   2.80833e+00   1.04412e-02  -8.44082e-06
2  Mean         1.69885e+00   3.45841e-03   1.57566e-05  -1.80302e-02
3  Sigma        2.38383e-01   2.50369e-03   1.28718e-05   3.60208e-03
output
1  Constant     2.16011e+02   3.97203e+00   1.58462e-02  -1.61781e-04
2  Mean         1.48785e+02   5.78822e-02   2.94298e-04  -8.55822e-03
3  Sigma        4.00785e+00   4.63133e-02   1.48376e-05   1.35269e-02
4) 56 dB amplitude charge injector (input)
1  Constant     1.45387e+02   2.73477e+00   1.03621e-02   4.88142e-05
2  Mean         9.42003e-01   3.62160e-03   1.72056e-05   2.25198e-02
3  Sigma        2.17144e-01   3.14001e-03   1.90312e-05  -6.73114e-02
output
1  Constant     2.01267e+02   3.67051e+00   1.06695e-02   5.54023e-05
2  Mean         7.52674e+01   6.46253e-02   2.39257e-04  -5.19141e-03
3  Sigma        4.45948e+00   5.15990e-02   1.12128e-05   4.04965e-02

*/
}
  input = [5.18926,2.42101,1.69885,9.42003e-01];
  output = [3.24017e+02,2.08359e+02,1.48785e+02,7.52674e+01];
  ein = [2.67243e-01,3.28376e-01,2.38383e-01,2.17144e-01];
  eout = [3.39200,3.88844,4.00785,4.45948];
  errorgain = gainCh8.*sqrt((eout./output).^2+(ein./input).^2) ;
  errordB = 20/log(10)*(errorgain./gainCh8);
  gainCh8=output./input;
  gainCh8dB= 20.*log10(gainCh8);


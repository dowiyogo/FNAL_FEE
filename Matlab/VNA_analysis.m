clear;
Gali_20mA = sparameters('GALI-S66+_Unit1_20mA_Plus25.s2p')
optGali = detectImportOptions('GALI-S66+_Unit1_20mA_Plus25.dat');
VNAGali = readmatrix('GALI-S66+_Unit1_20mA_Plus25.dat',optGali);
Gali_gain= rfparam(Gali_20mA,2,1);
%Gali_lin = 10.^(abs(Gali_gain)./20);
Gali_Freq=Gali_20mA.Frequencies;
Gali_Freq3G = Gali_Freq(Gali_Freq <= 3e9);
Gali_gain3G = Gali_gain(1:length(Gali_Freq3G));
VNAGali3G = VNAGali(1:length(Gali_Freq3G),:);
VNACh1Gali = zeros(size(VNAGali3G));
VNACh1Gali(:,1)= VNAGali3G(:,1);
VNACh8Gali = zeros(size(VNAGali3G));
VNACh8Gali(:,1)= VNAGali3G(:,1);
Ch1 = sparameters('CH1_-15dbm_100KHz_3GHz.s2p')
optCh1 = detectImportOptions('CH1_-15dbm_100KHz_3GHz.dat');
VNACh1 = readmatrix('CH1_-15dbm_100KHz_3GHz.dat',optCh1);
VNACh1(:,1)= VNACh1(:,1)*1e6;
%Ch1_lin = VNACh1(:,4);
Ch1_gain= rfparam(Ch1,2,1);
Ch8 = sparameters('CH8_-15dbm_100KHz_3GHz.s2p')
optCh8 = detectImportOptions('CH8_-15dbm_100KHz_3GHz.dat');
VNACh8 = readmatrix('CH8_-15dbm_100KHz_3GHz.dat',optCh8);
VNACh8(:,1)= VNACh8(:,1)*1e6;
%Ch8_lin = VNACh8(:,4);
Ch8_gain= rfparam(Ch8,2,1);
VNAFreq = Ch1.Frequencies;
for i = 2:9
    VNACh1Gali(:,i) = interp1(VNACh1(:,1),VNACh1(:,i),VNAGali3G(:,1));
    VNACh8Gali(:,i) = interp1(VNACh8(:,1),VNACh8(:,i),VNAGali3G(:,1));
end
Ch1Data = zeros(2,2,length(Gali_Freq3G));
Ch8Data = zeros(2,2,length(Gali_Freq3G));
GaliData = zeros(2,2,length(Gali_Freq3G));

for j = 1 : length(Gali_Freq3G)
    Ch1Data(1,1,j) = VNACh1Gali(j,2).*exp(1i.*VNACh1Gali(j,3)*pi./180);
    Ch8Data(1,1,j) = VNACh8Gali(j,2).*exp(1i.*VNACh8Gali(j,3)*pi./180);
    GaliData(1,1,j) = 10.^(VNAGali3G(j,2)./20).*exp(1i.*VNAGali3G(j,3)*pi./180);
    Ch1Data(2,1,j) = VNACh1Gali(j,4).*exp(1i.*VNACh1Gali(j,5)*pi./180);
    Ch8Data(2,1,j) = VNACh8Gali(j,4).*exp(1i.*VNACh8Gali(j,5)*pi./180);
    GaliData(2,1,j) = 10.^(VNAGali3G(j,4)./20).*exp(1i.*VNAGali3G(j,5)*pi./180);
    Ch1Data(1,2,j) = VNACh1Gali(j,6).*exp(1i.*VNACh1Gali(j,7)*pi./180);
    Ch8Data(1,2,j) = VNACh8Gali(j,6).*exp(1i.*VNACh8Gali(j,7)*pi./180);
    GaliData(1,2,j) = 10.^(VNAGali3G(j,6)./20).*exp(1i.*VNAGali3G(j,7)*pi./180);
    Ch1Data(2,2,j) = VNACh1Gali(j,8).*exp(1i.*VNACh1Gali(j,9)*pi./180);
    Ch8Data(2,2,j) = VNACh8Gali(j,9).*exp(1i.*VNACh8Gali(j,9)*pi./180);
    GaliData(2,2,j) = 10.^(VNAGali3G(j,8)./20).*exp(1i.*VNAGali3G(j,9)*pi./180);
end
SCh1Gali = sparameters(Ch1Data,Gali_Freq3G)
SCh8Gali = sparameters(Ch8Data,Gali_Freq3G)
SGali3G = sparameters(GaliData,Gali_Freq3G)

TCh1DataInv = zeros(2,2,length(Gali_Freq3G));
TCh8DataInv = zeros(2,2,length(Gali_Freq3G));
TGaliDataInv = zeros(2,2,length(Gali_Freq3G));
TCh1Dataimp = zeros(2,2,length(Gali_Freq3G));
TCh8Dataimp = zeros(2,2,length(Gali_Freq3G));
TGaliSQDataInv = zeros(2,2,length(Gali_Freq3G));

TCh1Gali = tparameters(SCh1Gali)
TCh8Gali = tparameters(SCh8Gali)
TGali3G = tparameters(SGali3G)
SGaliSQ = cascadesparams(SGali3G,SGali3G)
TGaliSQ = tparameters(SGaliSQ)

for j = 1 : length(Gali_Freq3G)
    TCh1DataInv(:,:,j) = inv(TCh1Gali.Parameters(:,:,j));
    TCh8DataInv(:,:,j) = inv(TCh8Gali.Parameters(:,:,j));
    TGaliDataInv(:,:,j) = inv(TGali3G.Parameters(:,:,j));
    TGaliSQDataInv(:,:,j) = inv(TGaliSQ.Parameters(:,:,j));
end

for j = 1 : length(Gali_Freq3G)
    TCh1Dataimp(:,:,j) = TCh1DataInv(:,:,j)*TGaliSQDataInv(:,:,j);
    TCh8Dataimp(:,:,j) = TCh8DataInv(:,:,j)*TGaliSQDataInv(:,:,j);
end

TCh1imp = tparameters(TCh1Dataimp,Gali_Freq3G)
TCh8imp = tparameters(TCh8Dataimp,Gali_Freq3G)

ZCh1imp = zparameters(TCh1imp)
ZCh8imp = zparameters(TCh8imp)

Z11 = rfparam(ZCh1imp,1,1);

%TGaliInv = tparameters(TGaliDataInv,Gali_Freq3G)
%TCh1Inv = tparameters(TCh1DataInv,Gali_Freq3G)
%TCh8Inv = tparameters(TCh8DataInv,Gali_Freq3G)

%TCH1imp = cascadesparams(TCh1Gali,TGaliInv,TGaliInv)
%TCH8imp = cascadesparams(TCh8Gali,TGaliInv,TGaliInv)

%SCh1imp = sparameters(TCh1imp,Gali_Freq3G)
%SCh8imp = sparameters(TCh8imp,Gali_Freq3G)
%rfplot(SCh1imp)
figure;
semilogy(Gali_Freq3G,abs(Gali_gain3G),VNAFreq,abs(Ch1_gain),VNAFreq,abs(Ch8_gain));
legend("Gali","Ch1","Ch8")
cir = circuit('entrada');
add(cir,[1 2],resistor(51));
add(cir,[2 3],resistor(10e3));
add(cir,[3 4],capacitor(100e-9));
add(cir,[4 1],resistor(16.9));
%add(cir,[4 5],capacitor(0.1e-6));
%add(cir,[5 1],resistor(50))
%setports(cir,[2 1],[5 1]);
setports(cir,[2 1],[4 1]);
cir_S = sparameters(cir,Gali_Freq3G)
cir_Z = zparameters(cir_S)
cir_Z11 = rfparam(cir_Z,1,1);
cir_gain= rfparam(cir_S,2,1);
%semilogy(Gali_Freq3G,abs(Gali_gain3G),VNAFreq,abs(Ch1_gain),VNAFreq,abs(Ch8_gain),VNAFreq,abs(cir_gain))
%legend("Gali","Ch1","Ch8","Circuito Entrada")
%loglog(Gali_Freq3G,abs(cir_gain));
S_total = cascadesparams(cir_S,SGali3G,SGali3G)
res_16 = circuit('resistencia');
add(res_16,[1 2],resistor(16.9));
setports(res_16,[2 1],[2 1]);
cir_reS = sparameters(res_16,Gali_Freq3G)
%S_total = cascadesparams(cir_S,SGali3G,SGali3G)
%S_total = cascadesparams(cir_reS,SGali3G,SGali3G)
%rfplot(S_total)








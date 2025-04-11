% Att_10dB_s2p
% Fermi_Att_10dB_in&out_s2p
% Inj_s2p

clear;

AttS2P = sparameters('Att_10dB_s2p.s2p');
BoardS2P = sparameters('Fermi_Att_10dB_in&out_s2p.s2p');
InjS2P = sparameters ('Inj_s2p.s2p');

Freq = AttS2P.Frequencies;

%optAtt = detectImportOptions('Att_10dB_s2p.dat');
%AttDat = readmatrix('Att_10dB_s2p.dat',optAtt);
%AttDat(:,1) = AttDat(:,1)*1e6;

%optInj = detectImportOptions('Inj_s2p.dat');
%InjDat = readmatrix('Inj_s2p.dat',optInj);
%InjDat(:,1) = InjDat(:,1)*1e6;

%optBoard = detectImportOptions('Fermi_Att_10dB_in&out_s2p.dat');
%BoardDat = readmatrix('Fermi_Att_10dB_in&out_s2p.dat',optBoard);
%BoardDat(:,1) = BoardDat(:,1)*1e6;

%AttComp = zeros(2,2,length(Freq));
%BoardComp = zeros(2,2,length(Freq));
%InjComp = zeros(2,2,length(Freq));

%for j = 1 : length(Freq)
%    AttComp(1,1,j) = AttDat(j,2).*exp(1i.*AttDat(j,3)*pi./180);
%    BoardComp(1,1,j) = BoardDat(j,2).*exp(1i.*BoardDat(j,3)*pi./180);
%    InjComp(1,1,j) = 10.^(InjDat(j,2)./20).*exp(1i.*InjDat(j,3)*pi./180);
%    AttComp(2,1,j) = AttDat(j,4).*exp(1i.*AttDat(j,5)*pi./180);
%    BoardComp(2,1,j) = BoardDat(j,4).*exp(1i.*BoardDat(j,5)*pi./180);
%    InjComp(2,1,j) = 10.^(InjDat(j,4)./20).*exp(1i.*InjDat(j,5)*pi./180);
%    AttComp(1,2,j) = AttDat(j,6).*exp(1i.*AttDat(j,7)*pi./180);
%    BoardComp(1,2,j) = BoardDat(j,6).*exp(1i.*BoardDat(j,7)*pi./180);
%    InjComp(1,2,j) = 10.^(InjDat(j,6)./20).*exp(1i.*InjDat(j,7)*pi./180);
%    AttComp(2,2,j) = AttDat(j,8).*exp(1i.*AttDat(j,9)*pi./180);
%    BoardComp(2,2,j) = BoardDat(j,9).*exp(1i.*BoardDat(j,9)*pi./180);
%    InjComp(2,2,j) = 10.^(InjDat(j,8)./20).*exp(1i.*InjDat(j,9)*pi./180);
%end

TAtt = tparameters(AttS2P);
TBoard = tparameters(BoardS2P);
TInj = tparameters(InjS2P);

TAttInv = zeros(2,2,length(Freq));
TBoardInv = zeros(2,2,length(Freq));
TInjInv = zeros(2,2,length(Freq));

for j = 1 : length(Freq)
    TAttInv(:,:,j) = inv(TAtt.Parameters(:,:,j));
    TBoardInv(:,:,j) = inv(TBoard.Parameters(:,:,j));
    TInjInv(:,:,j) = inv(TInj.Parameters(:,:,j));
end
TFermiData = zeros(2,2,length(Freq));
for j = 1 : length(Freq)
    TFermiData(:,:,j) = TInjInv(:,:,j)*TAttInv(:,:,j)*TBoard.Parameters(:,:,j)*TAttInv(:,:,j);
end

TFermi = tparameters(TFermiData,Freq);
SFermi = sparameters(TFermi)
rfplot(SFermi)
figure
rfplot(SFermi,2,1)
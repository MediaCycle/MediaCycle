%RUNYIN(wavFile)
%    Sets default parameters and calls YIN for f0 estimation
%    Performs post-filtering (calls filterf0.m) for correction 
%      of octave errors and smoothing the noisy parts

% IYTE-Audio Pro. Lab. SOFTWARE LIBRARY
% Date: <September/2008>
% Authors: Baris Bozkurt
% Email :  bozkurt@tcts.fpms.ac.be
% Copyright (c) Izmir Institute Of Technology, Audio Processing Lab. 
% All rights reserved.

function runYin(wavFile)

% YIN(NAME,P) uses parameters stored in P:
%   P.minf0:    Hz - minimum expected F0 (default: 30 Hz)
%   P.maxf0:    Hz - maximum expected F0 (default: SR/(4*dsratio))
%   P.thresh:   threshold (default: 0.1)
%   P.relfag:   if ~0, thresh is relative to min of difference function (default: 1)
%   P.hop:      s - interval between estimates (default: 32/SR)
%   P.range:    samples - range of samples ([start stop]) to process
%   P.bufsize:  samples - size of computation buffer (default: 10000)
%	P.sr:		Hz - sampling rate (usually taken from file header)
%	P.wsize:	samples - integration window size (defaut: SR/minf0)
%	P.lpf:		Hz - intial low-pass filtering (default: SR/4)
%	P.shift		0: shift symmetric, 1: shift right, -1: shift left (default: 0)


[y,FS]=wavread(wavFile);

P = struct('minf0',30,'maxf0',4000);
P.thresh=0.1;%default value
P.relfag=1;%default value
%P.hop=round(FS*0.005);% hop/shift set to 5 msec
P.hop=round(FS*0.01);% hop/shift set to 10 msec
P.range=[1 length(y)];%default value
P.bufsize=10000;%default value
P.sr=FS;%default value
P.wsize=FS/P.minf0;%default value
P.lpf=FS/4;%default value
P.shift=0;%default value

f0_HzIni=yin(wavFile,P);

%find Nan entries
nanInd=find(~isfinite(f0_HzIni));
f0_HzIni(nanInd)=8.17579891564371;%corresponds to zero cents

%figure;plot(f0_HzIni,'.k');%plot(f0_Hz,'.');

octavFiltFlag=0;minChunkSize=10;
[f0_Hz]=filterf0(f0_HzIni,octavFiltFlag,minChunkSize,P.hop,wavFile);
%hold;plot(f0_Hz,'-k');%plot(f0_Hz,'o');
%hold;zoom xon
%legend('YIN','modifiedYIN');xlabel('frame no(n)');ylabel('f0(Hz)');

outFile=strrep(wavFile,'.wav','.yin.txt');
fidOut=fopen(outFile,'w+t');
for k=1:length(f0_Hz)
    if(~isinf(f0_Hz(k)) && isfinite(f0_Hz(k)))
        fprintf(fidOut,'%d %5.2f\n',k,hz2cent(f0_Hz(k)));
    end
end
fclose(fidOut);

f0_cent=hz2cent(f0_Hz);
datFile=strrep(wavFile,'.wav','.f0.mat');
save(datFile,'f0_cent');
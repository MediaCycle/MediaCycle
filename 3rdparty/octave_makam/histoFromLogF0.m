%HISTOFROMLOGF0(pitchFile)
%    Computes histogram and saves in .hist.mat file
%    Histogram edges are set such that the region [meanF0-2octaves meanF0+2octaves]
%    is divided with 1/3Holderian comma resolution 
%
%    To plot the histogram with Hz unit: 
%    load *.hist.mat; plot(2.^edges,n_all);

% IYTE-Audio Pro. Lab. SOFTWARE LIBRARY
% Date: <September/2008>
% Authors: Baris Bozkurt
% Email :  bozkurt@tcts.fpms.ac.be
% Copyright (c) Izmir Institute Of Technology, Audio Processing Lab. 
% All rights reserved.

function histoFromLogF0(pitchFile)

histFile=strrep(pitchFile,'.yin.txt','.hist.mat');
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%IF FILE EXISTS, EXIT THE FUNCTION WITHOUT RUNNING
fidHist=fopen(histFile,'r');
if(fidHist~=-1)%if file exists exit the function
    fclose(fidHist);
    load(histFile);
    return;
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%perdeler=struct('tonicCand',[],'f',[],'fIndex',[]);
perdeler=struct('tonicHz',[],'tonicInd',[]);

%reading estimated f0 values -------------------
f0track=[];
fidPitch=fopen(pitchFile,'r+t');
if(fidPitch ~= -1)%if file opened without problem
    while 1
        textline = fgetl(fidPitch);
        if ~ischar(textline), break, end
        numline=str2num(textline);
        f0track=[f0track; numline(2)];    
    end
    f0track=cent2hz(f0track);
    fclose(fidPitch);
else
    disp(sprintf('could not open file:%s\n',pitchFile));
end
maxf0=max(f0track);
[f0track]=removeZerosAtEnds(f0track);
numF0Frames=length(f0track);

numOctaves=4;
[zeroInd]=find(f0track<8);f0track(zeroInd)=8.17579891564371;
logF0=log2(f0track);
maxLogF0=max(logF0);

if(maxLogF0>5)
    minLogF0=maxLogF0-numOctaves;
else
    disp('maxLogF0<5, exit');return;
end
stepNo=numOctaves*53*3;
st=(maxLogF0-minLogF0)/(stepNo-1);
edges=minLogF0:st:maxLogF0;

%%%find the tonic
[perdeler,n_all]=findTonic(perdeler,logF0,numF0Frames,minLogF0,edges);
n=n_all;%n will be modified during peak finding

save(histFile,'n_all','edges','perdeler');
disp(['done: ' histFile]);

%%%%%%%%%%%%%%%%%%%%%%%%%%
function [p]=ispeak(n,ind)

if(n(ind-1)<n(ind) && n(ind+1)<n(ind))
    p=1;
else
    p=0;
end

function [f0track]=removeZerosAtEnds(f0track)

leftInd=1;
while(leftInd<length(f0track))
    if(f0track(leftInd)>9)%in cents
        break;
    end
    leftInd=leftInd+1;
end
f0track=f0track(leftInd:end);

rightInd=length(f0track);
while(rightInd>10)
    if(f0track(rightInd)>9)%in cents
        break;
    end
    rightInd=rightInd-1;
end
f0track=f0track(1:rightInd);

%%FINDING THE TONIC---------------------------------------------
function [perdeler,n_all]=findTonic(perdeler,logF0,numF0Frames,minLogF0,edges)

comma=2^(1/53);
comma2=comma*comma;
%finding the tonic as last most frequent note ------------------
lastPartF0=1500;
if(lastPartF0>numF0Frames)
    lastPartF0=round(numF0Frames/2);
end
%calculating histograms
f0last=logF0(end-lastPartF0+1:end);
%remove zero-entries inside
zInd=find(f0last>minLogF0);
f0last=f0last(zInd);
lastPartF0=min(round(lastPartF0/5),length(f0last)-1);
f0last=f0last(end-lastPartF0+1:end);
n=histc(f0last,edges);
%n(1:minF0)=0;
[val,ind]=max(n);
if(ind<length(edges))
    tonicCand1=2^((edges(ind)+edges(ind+1))/2);
else
    tonicCand1=2^edges(ind);
end

% finding the tonic as most frequent note in the whole file--------
%calculating histograms
n_all=histc(logF0,edges);
[val,ind2]=max(n_all);
tonicCand2=2^((edges(ind2)+edges(ind2+1))/2);
%perdeler.tonicCand=[tonicCand1 tonicCand2];

% decision of tonic
[val,indTonic]=max([tonicCand1 tonicCand2]);
if(indTonic==1)
    tonicRatio=tonicCand1/tonicCand2;
else
    tonicRatio=tonicCand2/tonicCand1;
end
if(tonicRatio<comma2)%oran iki komadan küçükse geneli al
    perdeler.tonicHz=tonicCand2;
    perdeler.tonicInd(1)=ind2;
else
    perdeler.tonicHz=tonicCand1;
    perdeler.tonicInd(1)=ind;
end
%%FINDING THE TONIC---------------------------------------------


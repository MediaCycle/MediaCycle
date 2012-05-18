%[f0track]=FILTERF0(f0track,octavFiltFlag,minChunkSize,windowShiftSample,wavFile)
%    Post-filtering to: 
%           - remove sudden jumps in the f0 curve
%           - assign zero cents frequency to noisy f0 regions
%           - filter out outliers (too high frequencies)
%    Setting 'octavFiltFlag', octave error correction filter is enabled.
%    windowShiftSample: number of samples (hop size) in YIN analysis
%    Wave file is utilized in filters using signal energy information

% IYTE-Audio Pro. Lab. SOFTWARE LIBRARY
% Date: <September/2008>
% Authors: Baris Bozkurt
% Email :  bozkurt@tcts.fpms.ac.be
% Copyright (c) Izmir Institute Of Technology, Audio Processing Lab. 
% All rights reserved.


function [f0track]=filterf0(f0track,octavFiltFlag,minChunkSize,windowShiftSample,wavFile)

%-----------------------------------
[f0track]=correctOctErrByChnk(f0track);
%-----------------------------------
%removing extreme values-----------
%get histogram
maxf0=max(f0track);
histEdges=0:100:maxf0;
n=histc(f0track,histEdges);
%find ranges where no pitch estimation exists
zeroInd=find(n==0);
if(length(zeroInd)>2)
    k=1;
    while((zeroInd(k)~=zeroInd(k+1)-1) && k<(length(zeroInd)-1))
        k=k+1;
    end
    if(k<length(zeroInd)-1)
        if(sum(n(1:zeroInd(k)))>(sum(n)*0.9))%higher values are outliers
            maxf0=histEdges(zeroInd(k)+1);
        end
    end
end
meanf0=mean(f0track);
stdf0=std(f0track);
candMaxf0=max(meanf0*4,meanf0+2*stdf0);
maxf0=min(maxf0,candMaxf0);
%find indices of estimates exceeding maxval
maxInd=find(f0track>maxf0);
f0track(maxInd)=8.17579891564371;
%find indices of estimates lower than minval
minf0=mean(f0track)/4;
minInd=find(f0track<minf0);
f0track(minInd)=8.17579891564371;

%-----------------------------------
%correct jumps/discontinuities within continuous curves
[f0track]=correctJumps(f0track);
%the same operation on the inverse
f0track=f0track(end:-1:1);
[f0track]=correctJumps(f0track);
f0track=f0track(end:-1:1);
%-----------------------------------
%assigns zero cents frequency to noisy f0 regions
for m=1:3
    for k=2:length(f0track)-2%3 seperate points, assign 0 to mid
        if(~areClose(f0track(k-1),f0track(k)) && ~areClose(f0track(k),f0track(k+1)))
            f0track(k)=8.17579891564371;
        end
    end
    for k=3:length(f0track)-3%4 points, 2 in mid are similar but different from the boundaries, assign 0 to mid two
        if(~areClose(f0track(k-2),f0track(k)) && ~areClose(f0track(k-1),f0track(k)) && ~areClose(f0track(k+1),f0track(k+2)) && ~areClose(f0track(k+1),f0track(k+3)))
            f0track(k:k+1)=8.17579891564371;
        end
    end
end
%-----------------------------------
%filter out noise like variations
for k=2:length(f0track)-2
    if(~areClose(f0track(k-1),f0track(k)) && ~areClose(f0track(k),f0track(k+1)) && ~areClose(f0track(k+1),f0track(k+2)))
        if(~areClose(f0track(k-1),f0track(k+1)) && ~areClose(f0track(k),f0track(k+2)))
            if(~areClose(f0track(k-1),f0track(k+2)))
                f0track(k:k+1)=8.17579891564371;
            end
        end        
    end
end
%-----------------------------------
%filter that corrects octave errors
if(octavFiltFlag)
    [f0track]=correctOctaveErr(f0track);
    %the same operation on the inverse
    f0track=f0track(end:-1:1);
    [f0track]=correctOctaveErr(f0track);
    f0track=f0track(end:-1:1);
end
%-----------------------------------
[f0track]=correctOctErrByChnk(f0track);
%-----------------------------------
%filter short chunks with low energy
[f0Chunks,chunkLengths]=decomposeIntoChunks(f0track);
%find longest chunk
[val,maxInd]=max(chunkLengths);
%compute average signal amplitude in the longest chunk
wavStartInd=f0Chunks(maxInd).startIndex*windowShiftSample;
wavStopInd=(f0Chunks(maxInd+1).startIndex-1)*windowShiftSample;
[chunkSig]=wavread(wavFile,[wavStartInd wavStopInd]);
aveAmpLongest=sum(abs(chunkSig))/length(chunkSig);
aveAmpLimit=aveAmpLongest/6;

%filtering
for k=1:length(f0Chunks)
    if(max(f0Chunks(k).f0)>8.2)%no need to run this filter for zero-frequency regions
        if(length(f0Chunks(k).f0)<minChunkSize)
            f0Chunks(k).f0=f0Chunks(k).f0*0+8.17579891564371;
        elseif(length(f0Chunks(k).f0)<minChunkSize*3 && k<length(f0Chunks))%filtering short chunks with low signal energy
            %compute average signal amplitude
            wavStartInd=f0Chunks(k).startIndex*windowShiftSample;
            wavStopInd=(f0Chunks(k+1).startIndex-1)*windowShiftSample;
            [chunkSig]=wavread(wavFile,[wavStartInd wavStopInd]);
            aveAmp=sum(abs(chunkSig))/length(chunkSig);
            if(aveAmp<aveAmpLimit)
                f0Chunks(k).f0=f0Chunks(k).f0*0+8.17579891564371;
            end
        end
    end
end
[f0track]=recomposeChunks(f0Chunks);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [f0track]=correctJumps(f0track)
%each time, it assures that three boundaries on both sides are in fact continuous

for k=5:length(f0track)-6
    if(areClose(f0track(k-4),f0track(k-3)) && areClose(f0track(k-3),f0track(k-2)) && areClose(f0track(k-1),f0track(k-2)))
        %quadriple point        
        if(areClose(f0track(k+4),f0track(k+5)) && areClose(f0track(k+5),f0track(k+6)))
            if(~areClose(f0track(k),f0track(k-1)) && ~areClose(f0track(k),f0track(k+4)))
                f0track(k)=f0track(k-1);
            end
            if(~areClose(f0track(k+3),f0track(k-1)) && ~areClose(f0track(k+3),f0track(k+4)))
                f0track(k+3)=f0track(k+4);
            end          
        end
        %triple point
        if(areClose(f0track(k+3),f0track(k+4)) && areClose(f0track(k+4),f0track(k+5)))
            if(~areClose(f0track(k),f0track(k-1)) && ~areClose(f0track(k),f0track(k+3)))
                f0track(k)=f0track(k-1);
            end
            if(~areClose(f0track(k+2),f0track(k-1)) && ~areClose(f0track(k+2),f0track(k+3)))
                f0track(k+2)=f0track(k+3);
            end        
        end
        %double point
        if(areClose(f0track(k+2),f0track(k+3)) && areClose(f0track(k+3),f0track(k+4)))
            if(~areClose(f0track(k),f0track(k-1)) && ~areClose(f0track(k),f0track(k+2)))
                f0track(k)=f0track(k-1);
            end
            if(~areClose(f0track(k+1),f0track(k-1)) && ~areClose(f0track(k+1),f0track(k+2)))
                f0track(k+1)=f0track(k+2);
            end
        end
        %single point
        if(areClose(f0track(k+1),f0track(k+2)) && areClose(f0track(k+2),f0track(k+3)))
            if(~areClose(f0track(k),f0track(k-1)) && ~areClose(f0track(k),f0track(k+1)))
                f0track(k)=f0track(k-1);
            end
        end
    end
end

function [f0track]=correctOctaveErr(f0track)

midf0=(median(f0track)+mean(f0track))/2;
for k=5:length(f0track)-2
    %if previous values are continuous
    if(areClose(f0track(k-1),f0track(k-2)) && areClose(f0track(k-2),f0track(k-3)) && areClose(f0track(k-3),f0track(k-4)))
        if(f0track(k)>(midf0*1.8)) 
            if(areClose(f0track(k-1),f0track(k)/2))
                f0track(k)=f0track(k)/2;
            elseif(areClose(f0track(k-1),f0track(k)/4))
                f0track(k)=f0track(k)/4;
            end
        elseif(f0track(k)<(midf0/1.8))
            if(areClose(f0track(k-1),f0track(k)*2))
                f0track(k)=f0track(k)*2;
            elseif(areClose(f0track(k-1),f0track(k)*4))
                f0track(k)=f0track(k)*4;
            end
        end
    end
end


function [f0Chunks,chunkLengths]=decomposeIntoChunks(f0track)

f0Chunks=struct('f0',[],'startIndex',[]);
f0Chunks(1).startIndex=1;
ind=1;chunkLengths=[];
for k=1:length(f0track)-1
    f0Chunks(ind).f0=[f0Chunks(ind).f0 f0track(k)];
    f0interval=f0track(k+1)/f0track(k);
    if(f0interval<0.80 || f0interval>1.2)
        ind=ind+1;
        f0Chunks(ind).f0=[];
        f0Chunks(ind).startIndex=k+1;%keeps start index for each chunk within the whole f0track
        chunkLengths=[chunkLengths length(f0Chunks(ind-1).f0)];
    end
end
f0Chunks(ind).f0=[f0Chunks(ind).f0 f0track(end)];

function [newf0track]=recomposeChunks(f0Chunks)
%reconstruction
newf0track=[];
for k=1:length(f0Chunks)
    newf0track=[newf0track f0Chunks(k).f0];
end

function [f0track]=correctOctErrByChnk(f0track)
%-----------------------------------
%correct octave errors by chunks
%decompose into chunks
[f0Chunks,chunkLengths]=decomposeIntoChunks(f0track);
%correct octave errors in chunks
for k=2:length(f0Chunks)-1
    if(length(f0Chunks(k).f0)<max(length(f0Chunks(k-1).f0),length(f0Chunks(k+1).f0)))
        if(areClose(f0Chunks(k).f0(1)/2,f0Chunks(k-1).f0(end)) && f0Chunks(k).f0(end)/1.5>f0Chunks(k+1).f0(1))
            f0Chunks(k).f0=f0Chunks(k).f0/2;
        elseif(areClose(f0Chunks(k).f0(end)/2,f0Chunks(k+1).f0(1)) && f0Chunks(k).f0(1)/1.5>f0Chunks(k-1).f0(end))
            f0Chunks(k).f0=f0Chunks(k).f0/2;
            %----------------------------------
        elseif(areClose(f0Chunks(k).f0(1)*2,f0Chunks(k-1).f0(end)) && f0Chunks(k).f0(end)*1.5<f0Chunks(k+1).f0(1))
            f0Chunks(k).f0=f0Chunks(k).f0*2;
        elseif(f0Chunks(k).f0(1)*1.5<f0Chunks(k-1).f0(end) && areClose(f0Chunks(k).f0(end)*2,f0Chunks(k+1).f0(1)))
            f0Chunks(k).f0=f0Chunks(k).f0*2;
        end
    end
end
[f0track]=recomposeChunks(f0Chunks);




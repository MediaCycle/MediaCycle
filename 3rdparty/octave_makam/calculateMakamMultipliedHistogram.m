function multHisto = calculateMakamMultipliedHistogram(wavFile,makamTemplatesFile)
%makamDetector(wavFile,makamTemplatesFile)
%   Operations performed:
%       1-f0 extraction (if .yin.txt file does not exist)
%       2-Histogram computation(if .hist.mat file does not exist)
%       3-Makam-Tonic detection
%           Jointly performed by;
%           comparing with all existing makam histogram templates to the
%           recording's pitch histogram
%   Baris Bozkurt, Nov. 2011

%Parameter initializations
offset=5;%used in tonic re-computation, peak detection
noteWidthInCommas=4;%width of gaussian mixtures
makamTonicInd=700;%location(index) of the tonic in synthetic histogram templates
%--------------------------------------------------------

%Check if wav file exists
fidWav=fopen(wavFile,'r');
if(fidWav==-1)%if file does not exist, run yin
    disp(['File could not be read:' wavFile]);return;
else
    fclose(fidWav);
end

%1-Perform f0 extraction if .yin.txt file does not exist
pitchFile=strrep(wavFile,'.wav','.yin.txt');
fidPitch=fopen(pitchFile,'r+t');
if(fidPitch==-1)%if file does not exist, run yin
    runYin(wavFile);
else
    fclose(fidPitch);
end
%--------------------------------------------------------
%2-Perform histogram computation if .hist.mat does not exist
histoFromLogF0(pitchFile);histFile=strrep(pitchFile,'.yin.txt','.hist.mat');
load(histFile);songHisto=n_all/max(n_all);
%--------------------------------------------------------
%3-Makam-Tonic detection
%Load histogram templates file
if(~isempty(strfind(makamTemplatesFile,'.txt')))%if text file
    [makamTemplates]=readScaleTables(makamTemplatesFile);
    [makamTemplates,makamTonicInd]=createAllTheoTempls(makamTemplates,makamTonicInd,noteWidthInCommas);
    if(isempty(makamTemplates)),return;end
elseif(~isempty(strfind(makamTemplatesFile,'.mat')))
    load(makamTemplatesFile);
else
    disp('Format of Template file is not recognized:');disp(makamTemplatesFile);
    return;
end
% makamName='';makamIndex=[];
allMakamStrings = cell(length(makamTemplates),1);
[allMakamStrings{1:length(makamTemplates)}] = deal(makamTemplates.name);
% varargout{1} = allMakamStrings;
%--------------------------------------------------------
%template matching
allCorrValues=zeros(1,length(makamTemplates));
tonicIndex=allCorrValues;

for k=1:length(makamTemplates)
    template=makamTemplates(k).template;
    [tonicInd,corrVal]=multiplyTemplates(songHisto,template,makamTonicInd);
    allCorrValues(k)=corrVal;
    tonicIndex(k)=tonicInd;
end

[corrVal,makamIndex]=max(allCorrValues);
tonicInd=tonicIndex(makamIndex);




makamName=makamTemplates(makamIndex).name;
disp([makamName ' bulundu']);

template=makamTemplates(makamIndex).template;
[tonicInd]=reComputeTonic(songHisto,tonicInd,offset);

if(tonicInd>makamTonicInd)
    songHisto_cut=songHisto(tonicInd-makamTonicInd:end);
else
    songHisto_cut=[zeros(1,makamTonicInd-tonicInd) songHisto'];
    
end

if(length(songHisto_cut)<length(template))
    multHisto = [songHisto_cut zeros(1,length(template)-length(songHisto_cut))];
elseif(length(songHisto_cut) > length(template))
    multHisto = songHisto_cut(1:length(template));
else
    multHisto = songHisto_cut;
end
multHisto = multHisto .* template';

templateBegin = find(template,1,'first');
multHisto = multHisto(templateBegin:end);
multHisto = multHisto / max(multHisto);
multHisto = multHisto';



%-----------------------------------------------------------------------
%CALLED FUNCTIONS
%-----------------------------------------------------------------------
function [makamTemplates,makamTonicInd]=createAllTheoTempls(makamTemplates,makamTonicInd,noteWidthInCommas)

gridInCommas=3;
noteWidth=round(noteWidthInCommas*gridInCommas);
noteWidthHalf=round(noteWidth/2);
wind=gausswin(2*noteWidthHalf+1);

for k=1:length(makamTemplates)
    sequenceIncommas=makamTemplates(k).intervals;
    %creating the template
    templ=zeros(1,53*gridInCommas+makamTonicInd+100);
    %durak
    templ(makamTonicInd-noteWidthHalf:makamTonicInd+noteWidthHalf)=wind'*2;
    %other notes
    for m=1:length(sequenceIncommas)-1
        noteLoc=round(sequenceIncommas(m)*gridInCommas+makamTonicInd);
        templ(noteLoc-noteWidthHalf:noteLoc+noteWidthHalf)=templ(noteLoc-noteWidthHalf:noteLoc+noteWidthHalf)+wind';
    end
    %last note
    noteLoc=round(sequenceIncommas(end)*gridInCommas+makamTonicInd);
    templ(noteLoc-noteWidthHalf:noteLoc+noteWidthHalf)=templ(noteLoc-noteWidthHalf:noteLoc+noteWidthHalf)+wind';
    %storing the template
    makamTemplates(k).template=templ'/max(templ);
end

function [makamTemplates]=readScaleTables(tableFile)

makamTemplates=struct('system',[],'name',[],'intervals',[],'numTotalNotes',[]);
cnt=0;
fidTxt=fopen(tableFile,'r+t');
if(fidTxt==-1)
    disp(['Could not read:' tableFile]);makamTemplates='';
else
    systemName = fscanf(fidTxt,'%s',1);
    numOfNotesInOct = fscanf(fidTxt,'%d',1);
    makamTemplates(1).system=systemName;
    makamTemplates(1).numTotalNotes=numOfNotesInOct;
    while 1
        makamName = fscanf(fidTxt,'%s',1);
        if isempty(makamName), break, end
        cnt=cnt+1;
        makamTemplates(cnt).name=makamName;
        textline = fgetl(fidTxt);
        makamTemplates(cnt).intervals=str2num(textline);
    end
    fclose(fidTxt);
end

function [tonicInd,corrVal]=multiplyTemplates(songHisto,template,firstPeak)

distArray=zeros(1,length(template));
songHisto=[songHisto;zeros(length(template)-length(songHisto),1)];

for k=1:length(template)
    distArray(k)=sum(abs(songHisto.*template));
    songHisto=[songHisto(end);songHisto(1:end-1)];
end
[corrVal,ind]=max(distArray);%corrVal=1-corrVal;


tonicInd=firstPeak-ind;

%recompute tonic from a region around the estimate
function [tonicInd]=reComputeTonic(songHisto,tonicInd,offset)

for k=1:5
    portionNn=songHisto(tonicInd-offset:tonicInd+offset);
    indP=centerofgravity(portionNn);
    tonicInd=round(tonicInd+indP-(offset+1));
end

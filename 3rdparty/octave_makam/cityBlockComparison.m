%City Block(L1-norm): sum(abs(differences))
function [tonicInd,corrVal]=cityBlockComparison(songHisto,template,firstPeak)

distArray=zeros(1,length(template));
songHisto=[songHisto;zeros(length(template)-length(songHisto),1)];
for k=1:length(template)
    distArray(k)=sum(abs(songHisto-template));
    songHisto=[songHisto(end);songHisto(1:end-1)];
end
[corrVal,ind]=min(distArray);corrVal=1-corrVal;
tonicInd=firstPeak-ind;


%recompute tonic from a region around the estimate
function [tonicInd]=reComputeTonic(songHisto,tonicInd,offset)

for k=1:5
    portionNn=songHisto(tonicInd-offset:tonicInd+offset);
    indP=centerofgravity(portionNn);
    tonicInd=round(tonicInd+indP-(offset+1));
end

end

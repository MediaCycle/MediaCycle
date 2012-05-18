%[c]=centerofgravity(s)
function [c]=centerofgravity(s)
sizes=size(s);
if(sizes(1)<sizes(2))
    s=s';
end

squareS=s.*s;
sumSquare=sum(squareS);
x=1:length(s);
if(sumSquare==0)%signal with all-zero entries
    c=length(s)/2;
else
    c=sum(squareS.*x')/sumSquare;
end

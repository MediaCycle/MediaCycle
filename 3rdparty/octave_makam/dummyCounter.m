function vector = dummyCounter(filenamesCell)
%takes in a cell and returns a vector of integers of the same length as
%cell, increasing one by one.
vector = zeros(length(filenamesCell),1);

for i = 1:length(filenamesCell)
 	filename = filenamesCell{i};
	vector(i) = i;
end
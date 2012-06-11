function [allMakamStrings classifiedMakamStrings] = calculateMakamStrings(filenames,makamTemplatesFile)

% makamTemplatesFile = '/Users/Work/Development/Projects/MATLAB_Workspace/MakamCycle/mFiles_backup/makamlarArel_10makam.txt';
classifiedMakamStrings = cell(length(filenames),1);

for i = 1:length(filenames)
	wavFile = filenames{i};
	[classifiedMakamStrings{i} allMakamStrings] = makamDetector(wavFile,makamTemplatesFile);

end


end

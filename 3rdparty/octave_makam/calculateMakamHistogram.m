function makamHistogram = calculateMakamHistogram(filePath)

%Parameter initializations
offset=5;%used in tonic re-computation, peak detection
noteWidthInCommas=4;%width of gaussian mixtures
makamTonicInd=700;%location(index) of the tonic in synthetic histogram templates

%--------------------------------------------------------
wavFile = filePath;
%Check if wav file exists
disp(['calculateMakamHistogram: Opening wav file:' wavFile]);
fidWav=fopen(wavFile,'r');
if(fidWav==-1)%if file does not exist, run yin
    disp(['File could not be read:' wavFile]);return;
else
    fclose(fidWav);
end

%1-Perform f0 extraction if .yin.txt file does not exist
%if(strcmpi(getExtension(filePath),'.ogg'))
%	pitchFile=strrep(wavFile,'.ogg','.yin.txt');
%if((strcmpi(getExtension(filePath),'.wav')))
pitchFile=strrep(wavFile,'.wav','.yin.txt');
disp(['calculateMakamHistogram: Check if pitch file exists:' pitchFile]);
%end
fidPitch=fopen(pitchFile,'r+t');
if(fidPitch==-1)%if file does not exist, run yin
	if((strcmpi(getExtension(filePath),'.wav')))
		disp(['calculateMakamHistogram: Running YIN to create pitch file:' pitchFile]);
    		runYin(wavFile);
	else
		error('File needs to be in .wav format for YIN to work')
	end
else
    fclose(fidPitch);
end
%--------------------------------------------------------
%2-Perform histogram computation if .hist.mat does not exist
histFile=strrep(pitchFile,'.yin.txt','.hist.mat');
disp(['calculateMakamHistogram: Perform histogram computation:' histFile]);
histoFromLogF0(pitchFile);
load(histFile);songHisto=n_all/max(n_all);
%--------------------------------------------------------

makamHistogram = songHisto;
end

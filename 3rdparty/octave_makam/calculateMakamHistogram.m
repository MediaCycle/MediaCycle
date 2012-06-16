function makamHistogram = calculateMakamHistogram(filePath)

%Parameter initializations
offset=5;%used in tonic re-computation, peak detection
noteWidthInCommas=4;%width of gaussian mixtures
makamTonicInd=700;%location(index) of the tonic in synthetic histogram templates

%--------------------------------------------------------
wavFile = filePath;
%Check if wav file exists
fidWav=fopen(wavFile,'r');
if(fidWav==-1)%if file does not exist, run yin
    disp(['File could not be read:' wavFile]);return;
else
    fclose(fidWav);
end

%1-Perform f0 extraction if .yin.txt file does not exist
if(strcmpi(getExtension(filePath),'ogg'))
	pitchFile=strrep(wavFile,'.ogg','.yin.txt');
elseif((strcmpi(getExtension(filePath),'wav')))
pitchFile=strrep(wavFile,'.wav','.yin.txt');
end
fidPitch=fopen(pitchFile,'r+t');
if(fidPitch==-1)%if file does not exist, run yin
	if((strcmpi(getExtension(filePath),'wav')))
    runYin(wavFile);
	else
		error('File needs to be in .wav format for YIN to work')
	end
else
    fclose(fidPitch);
end
%--------------------------------------------------------
%2-Perform histogram computation if .hist.mat does not exist
histoFromLogF0(pitchFile);histFile=strrep(pitchFile,'.yin.txt','.hist.mat');
load(histFile);songHisto=n_all/max(n_all);
%--------------------------------------------------------

makamHistogram = songHisto;
end

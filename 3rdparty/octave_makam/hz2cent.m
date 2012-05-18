%[cents]=HZ2CENT(f0_Hz)
%   Convertion with ref: f(C-1)=8.17579891564371 Hz

% IYTE-Audio Pro. Lab. SOFTWARE LIBRARY
% Date: <September/2008>
% Authors: Baris Bozkurt
% Email :  bozkurt@tcts.fpms.ac.be
% Copyright (c) Izmir Institute Of Technology, Audio Processing Lab. 
% All rights reserved.
        
function [cents]=hz2cent(f0_Hz)

c1=8.17579891564371;%hz
ind=find(f0_Hz<30);
f0_Hz(ind)=c1;
cents=log2(f0_Hz/c1)*1200;
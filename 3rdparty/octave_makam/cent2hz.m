%[hz]=CENT2HZ(cents)
%   Convertion with ref: f(C-1)=8.17579891564371 Hz
        
% IYTE-Audio Pro. Lab. SOFTWARE LIBRARY
% Date: <September/2008>
% Authors: Baris Bozkurt
% Email :  bozkurt@tcts.fpms.ac.be
% Copyright (c) Izmir Institute Of Technology, Audio Processing Lab. 
% All rights reserved.

function [hz]=cent2hz(cents)
c1=8.17579891564371;%hz 
hz=c1*2.^(cents/1200);
ind=find(hz<20);
hz(ind)=0;
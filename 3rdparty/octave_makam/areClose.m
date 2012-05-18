function [decisionFlag]=areClose(num1,num2)
d=abs(num1-num2);
av=(num1+num2)/2;

decisionFlag=d/av<0.2;
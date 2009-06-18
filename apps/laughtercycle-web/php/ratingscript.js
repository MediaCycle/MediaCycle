/*
Author: Addam M. Driver
Date: 10/31/2006
*/

var sMax;	// Isthe maximum number of stars
var holder; // Is the holding pattern for clicked state
var preSet; // Is the PreSet value onces a selection has been made
var rated;

// Rollover for image Stars //
function rating(num){
	sMax = 0;	// Isthe maximum number of stars
	for(n=0; n<num.parentNode.childNodes.length; n++){
		if(num.parentNode.childNodes[n].nodeName == "A"){
			sMax++;	
		}
	}
	
	if(!rated){
		s = num.id.replace("_", ''); // Get the selected star
		a = 0;
		for(i=1; i<=sMax; i++){		
			if(i<=s){
				document.getElementById("_"+i).className = "on";
				//document.getElementById("rateStatus").innerHTML = num.title;	
				holder = a+1;
				a++;
			}else{
				document.getElementById("_"+i).className = "";
			}
		}
		document.getElementById("noteId").value = s;
	}
}

// For when you roll out of the the whole thing //
function rateOff(){
	if(!rated){
		if(!preSet){	
			for(i=1; i<=sMax; i++){		
				document.getElementById("_"+i).className = "";
			}
		}else{
			rating(preSet);
		}
	}
}

// When you actually rate something //
function rateIt(me,val){
	if(!rated){
		preSet = me;
		rated=1;
		sendRate(val);
		rating(me);
	}
}

// Send the rating information somewhere using Ajax or something like that.
function sendRate(val){
	document.getElementById("noteId").value = val;
}



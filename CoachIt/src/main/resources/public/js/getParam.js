$(document).ready(function() {
	function GetURLParameter(sParam){
	    var sPageURL = window.location.search.substring(1);
	    var sURLVariables = sPageURL.split('&');
	    for (var i = 0; i < sURLVariables.length; i++) {
	        var sParameterName = sURLVariables[i].split('=');
	        if (sParameterName[0] == sParam) {
	            return sParameterName[1].replace("+"," ");
	        }      
	    } 
	    return " ";
	}
	
	document.getElementById("title").innerHTML = GetURLParameter('title');
	document.getElementById("date").innerHTML = GetURLParameter('date');
	document.getElementById("loc").innerHTML = GetURLParameter('loc');
	document.getElementById("type").innerHTML = GetURLParameter('type');
	document.getElementById("deadline").innerHTML = GetURLParameter('deadline');
	document.getElementById("description").innerHTML = GetURLParameter('description');
});
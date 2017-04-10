var eventDate = "March 15, 2017"
var eventName = "Winter Classics"

$(document).ready(function() {
	if(localStorage.getItem('event') != "Winter Classics"){
    // page is now ready, initialize the calendar...
    $('#calendar').fullCalendar({
        // put your options and callbacks here
        header: {
			left: 'prev,next today',
			center: 'title',
			right: 'listDay,listWeek,month'
		},
	defaultView: 'listWeek',
	navLinks: true, // can click day/week names to navigate views
	editable: true,
	eventLimit: true,
		events: getEvents()
    })
	}
});
function CreateEvent() {
}

function Login() {
	var email = document.form["login"]["email"].value
	var pwd = document.form["login"]["pwd"].value
	return(pwd!="" && email!="") 
}

function getEvents() {
	var events = "{}"
	
	$.ajax({
	    url : "events",
	    type : "get",
	    async: false,
	    success : function(data, status){
			events = data;
	    
	    }
	});
	
	return JSON.parse(events)
}

function MakeEvent(){
	$('.notList').append("<li> " + eventName +  + " date: " + " <div class='btn-group navbar-right '> <br> <button class='btn btn-primary'>Inform Players</button> <button class='btn btn-danger '>Decline</button> </div></li>")
}

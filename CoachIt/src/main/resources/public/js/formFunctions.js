$(document).ready(function() {
    if($('#calendar').length !=0) {
      $('#calendar').fullCalendar({
		  header: {
			  left: 'title',
			  center: '',
			  right: 'prev,next'
		  },
		  defaultView: 'listWeek',
		  //navLinks: true, // can click day/week names to navigate views
		  editable: true,
		  eventLimit: true,
		  events: getEvents()
      })
      $(".fc-button-group").show()
      $(".fc-button").show()
    }
    
    var clubContainer = $('.club')
    if(clubContainer != null){
	var header = clubContainer.find(".navbar-brand")
	header.text(getClub())
	players = getPlayers()
	for(player in players) {
	  var item = "<li>" + players[player].name + "</li>"
	  clubContainer.find(".playerList").append(item)
	}
    }
    var clubContainer = $('.clubs')
    if(clubContainer != null){
	var header = clubContainer.find(".navbar-brand")
	clubs = getClubs()
	for(club in clubs) {
	  var item = "<li>" + clubs[club].name + "  "+ clubs[club].status + "</li>"
	  clubContainer.find(".clubList").append(item)
	}
    }
    var clubContainer = $('.players')
    if(clubContainer != null){
	var header = clubContainer.find(".navbar-brand")
	players = getEventPlayers()
	for(player in players) {
	  var item = "<li>" + players[player].name + "</li>"
	  clubContainer.find(".playerList").append(item)
	}
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
	var events = '[{"title":"winter clasics", "start":"2017-04-10", "url":"event/2"},{"title":"word1", "start":"2017-01-31", "url":"event/10"} ]'
	
	$.ajax({
	    url : "events",
	    type : "get",
	    async: false,
	    success : function(data, status){
			events = data;
	    
	    }
	});
	events = JSON.parse(events)
	return events
}

function getClub() {
	var clubName = 'My Club'
	
	$.ajax({
	    url : "clubName",
	    type : "get",
	    async: false,
	    success : function(data, status){
			clubName = data;
	    
	    }
	});
	return clubName
}



function getPlayers() {
	var players = '[{"name":"Mark","email":"Mark@gmail.com"},{"name":"Jason","email":"Jason@hotmail.com"}]'
	
	$.ajax({
	    url : "players",
	    type : "get",
	    async: false,
	    success : function(data, status){
			players = data;
	    
	    }
	});
	players = JSON.parse(players)
	return players
}

function getEventPlayers() {
	var players = '[]'
	
	$.ajax({
	    url : "playersEvent",
	    type : "get",
	    async: false,
	    success : function(data, status){
			players = data;
	    
	    }
	});
	players = JSON.parse(players)
	return players
}

function getClubs() {
	var clubs = '[]'
	
	$.ajax({
	    url : "clubs",
	    type : "get",
	    async: false,
	    success : function(data, status){
			clubs = data;
	    
	    }
	});
	clubs = JSON.parse(clubs)
	return clubs
}


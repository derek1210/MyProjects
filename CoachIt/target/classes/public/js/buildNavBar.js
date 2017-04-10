$(document).ready(function() {

	var nav_string = '<nav class=\x22navbar navbar-inverse\x22>';
	nav_string += '<div class=\x22container-fluid\x22>';
	nav_string += '<div class=\x22navbar-header\x22>';
	nav_string += '<a class=\x22navbar-brand\x22 href=\x22index.html\x22>CoachIt</a>';
	nav_string += '</div>';
	nav_string += '<ul class=\x22nav navbar-nav\x22>';
	nav_string += '<li><a href=\x22#\x22>Calendar</a></li>';
	nav_string += '<li><a href=\x22profile.html\x22>My Team</a></li>';
	nav_string += '<li><a href=\x22newEvent.html\x22>New Event</a></li>';
	nav_string += '</ul> </div> </nav>';

	function make_Var(validHTML){
		var fragment = document.createDocumentFragment();
		var navbar = document.createElement("nav");
		navbar.innerHTML = validHTML;
		while (navbar.firstChild) {
			fragment.appendChild(navbar.firstChild);
		}
		return fragment;
	}
	var frag = make_Var(nav_string);
	document.body.insertBefore(frag, document.body.childNodes[0]);
});

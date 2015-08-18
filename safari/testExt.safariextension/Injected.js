var theBody = document.body;
// create a para and insert it at the top of the body
var element = document.createElement("p");
element.id = "status";
element.style.cssText = "float:right; color:red";
element.textContent = "Waiting...";
theBody.insertBefore(element, theBody.lastChild);

function replyToMessage(aMessageEvent) {
	if (aMessageEvent.name === "hey") {
		document.getElementById("status").textContent = "Message received.";
		safari.self.tab.dispatchMessage("gotIt", "Message acknowledged.");
	}
}
// register for message events
safari.self.addEventListener("message", replyToMessage, false);
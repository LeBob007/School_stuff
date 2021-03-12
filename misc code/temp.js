var port = chrome.runtime.connect();

window.addEventListener("message", (event) => {
    if (event.source != window)
        return;

    if (event.data.type && (event.data.type == "FROM_PAGE")) {
        console.log("Content script received: " + event.data.text);
        port.postMessage(event.data.text);
    }
}, false);

const titles = document.getElementsByTagName("title")[0];
const metas = document.getElementsByTagName("meta");
var title = titles.innerHTML;
for (let i = 0; i < metas.length; i++) {
    const meta = metas[i];
    if (meta.name.toLowerCase() == "description"){
		var description = meta.content;
//		var bookmark = {title: title.innerHTML, description: meta.content};
	}
}

//port.postMessage({title: title, description: description});

chrome.runtime.sendMessage({title: title, description: description});
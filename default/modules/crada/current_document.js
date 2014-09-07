jQuery(function($) {

$(document).ready(function () {
	create_dialogs();

	
	$("#load_button").click(click_load_button);
	$("#save_button").click(click_save_button);
	$("#select_document_button").click(click_select_document_button);
});

function create_dialogs() {
	$("#load_dialog").dialog({
		title:"Load Document",
		position:['middle',100],
		autoOpen: false,
		height:'auto',
		width:[300]
	});	
}

function click_load_button () {
	$("#load_dialog").dialog("open");	
	// Need to call into database to pull back the documents, then versions
	ajax_caller("get_all_documents", null, load_document_into_select);
	
}

function load_document_into_select(data) {
	var documents = data.documents;

	$("#document_select").empty();
	for (i=0;i<documents.length; i++) {
		$("#document_select").append($("<OPTION>" + documents[i].title + "</OPTION>"));
	}
}

function click_save_button () {
	alert("Clicked Save Button");
}

function click_select_document_button() {
	alert("Clicked Select Document Button");
}

});

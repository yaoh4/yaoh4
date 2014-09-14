jQuery(function($) {

$(document).ready(function () {
	create_dialogs();

	
	$("#load_button").click(click_load_button);
	$("#save_button").click(click_save_button);
	$("#select_document_button").click(click_select_document_button);

	var querystring = getQueryString();
	if (querystring["action"] == 'Load') {
		document_id = querystring["document_id"];
		version = querystring["version"];
//		alert("Preloading: " + document_id + ":" + version);
		ajax_caller("get_full_document", {'document_id':document_id, 'version':version}, get_document_elements_callback);
	}


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
		$("#document_select").append($("<OPTION value='" + documents[i].document_id + "'>" + documents[i].name + "</OPTION>"));
	}
}

function click_save_button () {
	alert("Clicked Save Button");
}

function click_select_document_button() {
//	alert("Clicked Select Document Button: " + $("#document_select").val());
	var document_id = $("#document_select").val();
	var version = 0;
	
	ajax_caller("get_full_document", {'document_id':document_id, 'version':version}, get_document_elements_callback);
	
}

function get_document_elements_callback(data) {
//	alert (JSON.stringify(data, null, 2));

	$("#current_document_content").empty().append($("<H1>").append(data.title)).append("<hr />");
	
	current_section = "";
	for (var i=0; i<data.clauses.length; i++) {
		if (!(data.clauses[i].text == 'silent') ) {
			if (data.clauses[i].section != current_section) {
				$("#current_document_content").append("<br /><br />").append($("<H2>").append(data.clauses[i].section)).append("<hr />");
				current_section = data.clauses[i].section;
			}

			$("#current_document_content").append($("<P>").append(data.clauses[i].text));	
		} 
	}

}
//// Helper to get the query string to see if need to autoload document

function getQueryString() {
  var result = {}, queryString = location.search.slice(1),
      re = /([^&=]+)=([^&]*)/g, m;

  while (m = re.exec(queryString)) {
    result[decodeURIComponent(m[1])] = decodeURIComponent(m[2]);
  }

  return result;
}




});

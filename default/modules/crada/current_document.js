jQuery(function($) {
var documentSections;
$(document).ready(function () {
	create_dialogs();

	
	$("#load_button").click(click_load_button);
	$("#save_button").click(click_save_button);
	$("#select_document_button").click(click_select_document_button);
	$("#annotation_options").change(change_annotation_options);

	var querystring = getQueryString();
	if (querystring["action"] == 'Load') {
		document_id = querystring["document_id"];
		version = querystring["version"];
//		alert("Preloading: " + document_id + ":" + version);
		ajax_caller("get_full_document", {'document_id':document_id, 'version':version}, get_document_elements_callback);
	}

//Testing annotator
//  	$('#annotator').annotator().anotator('setupPlugins');

});

function change_annotation_options() {
	alert("change_annotation_options");

}

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
	
	var current_section = "";
	var confidential_annotation;
	var public_annotation;
	var annotationLabel;
	for (var i=0; i<data.clauses.length; i++) {
		if (!(data.clauses[i].text == 'silent') ) {
			if (data.clauses[i].section != current_section) {
				$("#current_document_content").append("<br /><br />");
				$("#current_document_content").append($("<h2>").append(data.clauses[i].section));
				current_section = data.clauses[i].section;
			}

			$("#current_document_content").append($("<a href='#'>").append("<p><sup>"+(i+1)+".</sup> "+data.clauses[i].text+"</p>").addClass('clause').attr( "id", "clause-" + i));

			if( JSON.stringify(data.clauses[i].confidential_annotation).length > 2) {
				annotationLabel = "<b>Comment [Conf"+(i+1)+"]:  </b>";
				$("#current_annotation_content").append($("<p>").append(annotationLabel+data.clauses[i].confidential_annotation).addClass('annotation').addClass('confidential_annotation'));	
			}
			if( JSON.stringify(data.clauses[i].public_annotation).length > 2) {
				annotationLabel = "<b>Comment [Pub"+(i+1)+"]:  </b>";
				$("#current_annotation_content").append($("<p>").append(annotationLabel+data.clauses[i].public_annotation).addClass('annotation').addClass('public_annotation'));
			}
		} 
	}
	documentSections = data;
	console.dir(documentSections);
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

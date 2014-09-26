jQuery(function($) {
var documentSections;
var current_document_id = 2;
var current_version = 0;
$(document).ready(function () {
	create_dialogs();

	$("#current_annotation_content").tooltip({show: {delay: 350}}); /*Adding jQuery tooltip for annotations*/
	$("#load_button").click(click_load_button);
	$("#save_button").click(click_save_button);
	$("#change_answer_button").click(click_change_answer_button);
	$("#back_button_answer").click(click_back_button_answer);

	$('#document_select').click(changed_document_id);
	$("#select_document_button").click(click_select_document_button);
	$("#annotation_options").change(change_annotation_options);
 	$("#current_document_content").on( "click", "p", function(event) {
            editClause(event);
	});
 	$("#current_annotation_content").on( "click", "p", function(event) {
            editAnnotation(event);
	});
	
	$("#change_answer_container").on( "change", "select", function(event) {
		changedAnswer(event);
	});	
	var querystring = getQueryString();
	if (querystring["action"] == 'Load') {
		setCookie("Drupal.visitor.document.id", querystring["document_id"], 365);
		setCookie("Drupal.visitor.document.version", querystring["version"], 365);
	}

	ajax_caller("get_full_document", {'document_id':getCookie("Drupal.visitor.document.id"), 'version':getCookie("Drupal.visitor.document.version")}, get_document_elements_callback);
	set_footer();
});

function set_footer(){
	$("#document_footer").empty().append(
		$('<div>')
		.append("Document Id: " + getCookie("Drupal.visitor.document.id"))
	);
	$("#document_footer").append(
		$('<div>')
		.append("Version: " + getCookie("Drupal.visitor.document.version"))
	);
}

function changedAnswer(e) {
	var ref = e.target.id;
	var question_id = $("#"+ref).attr('question_id');
	var answer_id = $("#"+ref).val();

	alert("You changed the answer for "+ref+"\nThe new value selected is "+answer_id+"\nquestion_id = "+question_id);
	ajax_caller('set_answer_retrieve_new_element', {'document_id':current_document_id, 'question_id':question_id, 'answer_id':answer_id}, set_answer_retrieve_new_element_callback);
}

function set_answer_retrieve_new_element_callback(data) {
//	alert(JSON.stringify(data));
//	alert("set_answer_retrieve_new_element_callback completed. Send clause to madlib to add demographic info. " + current_document_id);
	//Add madlib to clause with demographic answers.
//	alert("about to call addMadLib");
	console.log('data.demographic_answers');
	console.log(data.demographic_answers);
	console.log('JSON.parse(data.demographic_answers)');
	console.dir(JSON.parse(data.demographic_answers));
//	alert("about to call addMadLib");
	//
	//  Perform addMadLib and send back to server 
	//

	var new_mad_lib = addMadLib(data.element.document_element_text, JSON.parse(data.demographic_answers));
	data.element.document_element_text = new_mad_lib;

	ajax_caller('set_answer', {'user':getCookie('Drupal.visitor.user.name'), 'data':JSON.stringify(data)}, set_answer_callback, "POST");

}
function set_answer_callback(data) {
	//alert(JSON.stringify(data));
	//alert("set_answer completed.  Redireccting to latest document for document_id"+current_document_id);
	location.href = "load_document?action=Load&document_id=" + current_document_id + "&version="+current_version;

}

function editClause(e) {
	var ref = e.target.id;
	$("#"+ref).attr("contenteditable", "true").addClass('edit-section');
	var data_clause = $("#"+ref).attr("data-clause");
	var content;
	content = '<textarea rows="5" cols="70" class="data_clause_textarea">';
	content += data_clause;
	content += '</textarea>';
	$(content).dialog({
		resizable: false,
		height: 430,
		width: 700,
		modal: true,
		title: $("#"+ref).attr("dialog-title"),
		buttons: {
			"Cancel": function() {
				$( this ).dialog( "close" );
			},
			Save: function() {
				$( this ).dialog( "close" );
			}
		}
	});		
}

function editAnnotation(e) {
	var ref = e.target.id;

	var data_annotate = $("#"+ref).attr("data-annotate");
	var content;

	content = '<textarea rows="5" cols="70" class="data_clause_textarea">';
	content += data_annotate;
	content += '</textarea>';
	$(content).dialog({
		resizable: false,
		height:430,
		width:700,
		modal: true,
		title: $("#"+ref).attr("dialog-title"),
		buttons: {
			"Cancel": function() {
				$( this ).dialog( "close" );
			},
			Save: function() {
				$( this ).dialog( "close" );
			}
		}
	});		
}

function change_annotation_options() {
	var annotationOption = $( "#annotation_options" ).val();
	if( annotationOption == 'off') {
		$('#current_annotation_content').hide();
		$('#current_document_content').animate({width:'960px'});
		$('.annotation_footnote').hide();		
	} else {
		$('#current_document_content').animate({width: '700px'}, "normal", function(){
			$('#current_annotation_content').show();
			$('.annotation_footnote').show();		
		});
		switch(annotationOption) {
		    case "both":
		        $('.confidential_annotation').show();
		        $('.public_annotation').show();
		        break;
		    case "confidential":
		        $('.confidential_annotation').show();
		        $('.public_annotation').hide();
		        break;
		    case "public":
		        $('.confidential_annotation').hide();
		        $('.public_annotation').show();
		        break;
		}		
	}
}

function create_dialogs() {
	$("#load_dialog").dialog({
		title:"Load Document",
		position:['middle',100],
		autoOpen: false,
		height:'auto',
		width:[300],
		modal: true
	});	
}

function click_load_button () {
	$("#load_dialog").dialog("open");	
	// Need to call into database to pull back the documents, then versions
	ajax_caller("get_all_documents_info", null, load_document_info_into_select);
	
}

function changed_document_id() {
	var document_id = $("#document_select").val();
	//setCookie("Drupal.visitor.document.id", document_id, 365);
	//set the load_latest to 1
	setCookie("Drupal.visitor.document.loadLatest", 1, 365);
	ajax_caller("get_all_documents_info", null,	load_document_version_into_select);
}

function load_document_info_into_select(data) {
	var documents = data.documents;
	var document_id = parseInt(getCookie("Drupal.visitor.document.id"));

	console.log("load_document_info_into_select");
	console.dir(data);

	//populate select
	$("#document_select").empty();
	$.each( documents, function( document_id, doc ) {
		$("#document_select").append($("<OPTION value='" + doc.document_id + "'>" + doc.name + "</OPTION>"));
	});

	if(isNaN(document_id)) {
		document_id = $("#document_select").val();
		setCookie("Drupal.visitor.document.id", document_id, 365);
	};

	//select the current document
	$("#document_select").val(document_id).prop('selected', true);

	load_document_version_into_select(data);
}

function load_document_version_into_select(data) {
	var documents = data.documents;
	var version_id = parseInt(getCookie("Drupal.visitor.document.version"));
	var document_id = $("#document_select").val();
	console.log("document_id = "+document_id);

	//populate select
	$("#document_version").empty();
	$.each( documents[document_id].versions, function( key, version_id ) {
		$("#document_version").append($("<OPTION value='" + version_id + "'>" + version_id + "</OPTION>"));
	});

	if(isNaN(version_id)) {
		version_id = $("#document_version").val();
		setCookie("Drupal.visitor.document.version", version_id, 365);
	};
	if(parseInt(getCookie("Drupal.visitor.document.loadLatest")) == 1) {
		version_id = documents[document_id].versions[documents[document_id].versions.length-1];
		setCookie("Drupal.visitor.document.loadLatest", 0, 365);
	}
	//set the current version
	$("#document_version").val(version_id).prop('selected', true);;
}

function click_back_button_answer() {
	$('#current_document_container').show();
	$('#change_answer_container').hide();
}

function click_change_answer_button() {
	$('#current_document_container').hide();
	$('#change_answer_container').show();
	//var document_id = $("#document_select").val();

	ajax_caller("get_answers", {'document_id':current_document_id}, load_change_answer);
}

function load_change_answer(data) {
	change_answer_questions

/*
	$('#change_answer_questions').empty().append(
		$("<p>")
			.append("Changing a document answer below will immediately replace the appropriate clauses into the current document.")
			.addClass("change-answer-intro")
	);
*/
	$('#change_answer_questions').empty().append(
		$("<form>")
			.attr('id', 'change-answer')
	);

	var questions = data.questions;
	var previous_section = "";
	$.each(questions , function( key, value ) {
		console.dir(value);
		if(previous_section != value.section){
			//section_change
			$('#change-answer').append(
				$("<h2>")
					.append(value.section)
					.addClass('current_question')
			);
			previous_section = value.section;
			$('#change-answer').append(
				$("<hr>")
			);
			previous_section = value.section;
		};

		$('#change-answer').append($('<div>').attr('id', key).addClass('form-group'));
		//Add question LABEL
		$('#'+key).append(
			$("<label>")
				.append(value.question_text)
				.addClass('question-label')
				.attr('for', 'question-'+value.question_id)
			);
		//Add question SELECT
		$('#'+key).append(
			$("<select>")
				.append("Answer")
				.attr('id', 'question-'+value.question_id)
				.attr('name', 'question-'+value.question_id)
				.attr('question_id', value.question_id)
		);
		//Add question OPTIONS
		for(i=0;value.answers.length>i;i++) {
			$('#question-'+value.question_id).append(
				$('<option>')
					.append(value.answers[i])
					.attr('value', i)
			);
		}
		//Select the correct answer
		if(value.answer == null){
			$('#'+key).append(
				$("<span>")
					.append("answer is null")
					.css('color', 'red')
			);

		} else {
			$('#'+key).append(
				$("<span>")
					.append(value.answer)

			);
			//Select correct answer
			$("#question-"+value.question_id).val(value.answer).prop('selected', true);

		}	
		$('#'+key).append(
			$("<div>")
				.addClass('both')
		);

	});

}

function click_save_button () {
	alert("Clicked Save Button");
}

function click_select_document_button() {
//	alert("Clicked Select Document Button: " + $("#document_select").val());
	document_id = $("#document_select").val();
	version_id = $("#document_version").val();
	//Set cookies
	setCookie("Drupal.visitor.document.id", document_id, 365);
	setCookie("Drupal.visitor.document.version", version_id, 365);

	set_footer();
	
	ajax_caller("get_full_document", {'document_id':document_id, 'version':version_id}, get_document_elements_callback);
	$("#load_dialog").dialog( "close" );
	
}

function get_document_elements_callback(data) {
//	alert (JSON.stringify(data, null, 2));

	$("#current_document_content").empty().append($("<h1>").append(data.title)).append("<hr />");
	
	var current_section = "";
	var confidential_annotation;
	var public_annotation;
	var annotation_footnote;
	var section_reference;
	var section_number = 0;
	var data_clause;
	for (var i=0; i<data.clauses.length; i++) {
		if (!(data.clauses[i].text == 'silent') ) {
			if (data.clauses[i].section != current_section) {
				$("#current_document_content").append("<br /><br />");
				$("#current_document_content").append($("<h2>").append(data.clauses[i].section));
				current_section = data.clauses[i].section;
				section_number++;
			}
			section_reference = section_number+"."+(i+1);
			//data_clause = ​typeof data.clauses[i].text;
			//alert(data_clause);
			$("#current_document_content")
				.append(
					$("<p>")
						.append("<sup class='annotation_footnote'>["+section_reference+"]</sup> "+data.clauses[i].text)
						.addClass('contract_clause')
						.attr("id", "clause-" + i)
						.attr("dialog-title", "Section ["+section_reference+"]")
						.attr("title", "Click to edit section")
						.attr("data-clause", data.clauses[i].text)
					);

			//addAnnotationDiv(data.clauses[i].confidential_annotation, section_reference, 'Confid');
			addAnnotationDiv("test", section_reference, "Confid");
			//addAnnotationDiv(data.clauses[i].public_annotation, section_reference, 'Pub');
			if( JSON.stringify(data.clauses[i].confidential_annotation).length > 2) {
				var annotate = data.clauses[i].confidential_annotation;
				annotate = $("<div/>").html(annotate).text();
				data.clauses[i].confidential_annotation_short = trimAnnoation(JSON.stringify(data.clauses[i].confidential_annotation));
				annotation_footnote = "Comment [Conf"+section_reference+"]: ";
				//addAnnotationDiv(data.clauses[i].confidential_annotation, 'Confid', "current_annotation_content");
				$("#current_annotation_content")
					.append(
						$("<p class='conatract_clause'>")
							.append( "<b>" + annotation_footnote + "</b> " + data.clauses[i].confidential_annotation_short)
							.addClass('annotation')
							.addClass('confidential_annotation')
							.attr("id", "annotate-"+i+"annotateNeedsIndex")
							.attr("data-annotate", annotate)
							.attr("title", data.clauses[i].confidential_annotation)
							.attr("dialog-title", annotation_footnote)
					);	
			}
			if( JSON.stringify(data.clauses[i].public_annotation).length > 2) {
				var annotate = data.clauses[i].public_annotation;
				annotate = $("<div/>").html(annotate).text();
				data.clauses[i].public_annotation_short = trimAnnoation(JSON.stringify(data.clauses[i].public_annotation));
				annotation_footnote = "Comment [Pub"+(i+1)+"]:";
				$("#current_annotation_content")
					.append(
						$("<p>")
							.append("<b>" + annotation_footnote + "</b> " + data.clauses[i].public_annotation_short)
							.addClass('annotation')
							.addClass('public_annotation')
							.attr("id", "annotate-"+i+"annotateNeedsIndex")
							.attr("data-annotate", annotate)
							.attr("title", data.clauses[i].public_annotation)
							.attr("dialog-title", annotation_footnote)

						);
			}
		} 
	}
	documentSections = data;
	console.info("documentSections")
	console.dir(documentSections);
	$( ".annotation" ).tooltip({ track: true });

}

function addAnnotationDiv(annotation_data, section_reference, annotation_type) {
	var annotations;
	//split the annotation data on excel return
	//annotations = JSON.stringify(annotation_data);
	//console.log("section_reference: ");
	//console.dir(annotation_data);
}

function trimAnnoation(annotation) {
	//If length is greater than 60 then find next previous space and trim
	var max_size = 60;
	if(annotation.length > max_size) {
		//find the next previous space
	    for (i = max_size; i >= 0; i--) {
	        if (annotation[i] == " ") {break;}
	    }
	    annotation =  annotation.substr(0, i) + " ...";

	}

	return annotation;
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

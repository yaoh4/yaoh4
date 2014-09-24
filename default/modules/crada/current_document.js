jQuery(function($) {
var documentSections;
$(document).ready(function () {
	create_dialogs();

	$("#current_annotation_content").tooltip({show: {delay: 350}}); /*Adding jQuery tooltip for annotations*/
	$("#load_button").click(click_load_button);
	$("#save_button").click(click_save_button);
	$("#change_answer_button").click(click_change_answer_button);
	$("#back_button_answer").click(click_back_button_answer);

	$("#select_document_button").click(click_select_document_button);
	$("#annotation_options").change(change_annotation_options);
 	$("#current_document_content").on( "click", "p", function(event) {
            editClause(event);
	});
 	$("#current_annotation_content").on( "click", "p", function(event) {
            editAnnotation(event);
	});
	var querystring = getQueryString();
	if (querystring["action"] == 'Load') {
		document_id = querystring["document_id"];
		version = querystring["version"];
		ajax_caller("get_full_document", {'document_id':document_id, 'version':version}, get_document_elements_callback);
	}

});

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
function click_back_button_answer() {
	$('#current_document_container').show();
	$('#change_answer_container').hide();
}
function click_change_answer_button() {
	$('#current_document_container').hide();
	$('#change_answer_container').show();
	var document_id = $("#document_select").val();

	ajax_caller("get_answers", {'document_id':document_id}, load_change_answer);
}
function load_change_answer(data) {

	$('#change_answer_container').empty().append("<div style='height:25px;'></div>");
	$('#change_answer_container').append(
		$("<form>")
			.attr('id', 'change-answer')
	);
	var questions = data.questions;
	var previous_section = "";
	$.each(questions , function( key, value ) {
		console.dir(value);

		//$('#change_answer_container').append("<div> "+ "  question_id:" + value.question_id+
		//	 " section: "+value.section+" questin_text:   "+value.question_text+"</div>" );
//					.append(value.question_text)
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
				);
		//Add question OPTIONS
		for(i=0;value.answers.length>i;i++)
			$('#question-'+value.question_id).append(
				$('<option>')
						.append(value.answers[i])
						.attr('value', i)
				);
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
			$("#question"+value.question_id).val(value.answer).css('color', 'red');

		}	
		$('#'+key).append(
			$("<div>")
				.addClass('both')
			);

	});
	//alert(JSON.stringify(data));
	$('#change_answer_container').append(
			$("<div>")
				.append(JSON.stringify(data))
			);

}

function click_save_button () {
	alert("Clicked Save Button");
}

function click_select_document_button() {
//	alert("Clicked Select Document Button: " + $("#document_select").val());
	var document_id = $("#document_select").val();
	var version = 0;
	
	ajax_caller("get_full_document", {'document_id':document_id, 'version':version}, get_document_elements_callback);
	$("#load_dialog").dialog( "close" );
	
}

function get_document_elements_callback(data) {
//	alert (JSON.stringify(data, null, 2));

	$("#current_document_content").empty().append($("<H1>").append(data.title)).append("<hr />");
	
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
	console.dir(annotation_data);
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

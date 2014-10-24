jQuery(function($) {
var documentSections;
var clause_editor = [];
var my_editor;
$(document).ready(function () {
	create_dialogs();

	$("#current_annotation_content").tooltip({show: {delay: 350}}); /*Adding jQuery tooltip for annotations*/
	$("#load_button").click(click_load_button);
	$("#archive_version_button").click(click_archive_version_button);
	$("#download-pdf").click(function() {
		downloadDocument("PDF");
	});
	$("#download-word").click(function() {
		downloadDocument("Word");
	});
	$("#change_answer_button").click(click_change_answer_button);
	$("#back_button_answer").click(click_back_button_answer);

	$('#document_select').click(changed_document_id);
	$("#select_document_button").click(click_select_document_button);
	$("#annotation_options").change(user_changed_annotation_option);
	
 	$("#current_document_content").on( "focus", "p.clause", function(event) {
    	editClause(event);
	});
 	$("#current_document_content").on( "blur", "p.clause", function(event) {
 		updateClauseParagraph();
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
	if(getCookie('Drupal.visitor.document.id') != "") {
		ajax_caller("get_full_document", {'document_id':getCookie("Drupal.visitor.document.id"), 'version':getCookie("Drupal.visitor.document.version")}, get_document_elements_callback);
	} else {
		// Open up the Load document Dialog
		click_load_button();
	}

	set_footer();
});
function downloadDocument(document_type) {
//	console.dir(e);
//	e.preventDefault();  //stop the browser from following
    location.href = "download_document?document_id=" + getCookie('Drupal.visitor.document.id')
    	+"&version="+getCookie('Drupal.visitor.document.version')
    	+"&user="+getCookie('Drupal.visitor.user.name')
    	+"&document_type="+document_type;
}

function updateClauseParagraph() {
	// Check if dirty
	var data_changed = my_editor.checkDirty();
	//If dirty then add the changed-answer class to <p>
	if(data_changed) {
		$('#'+my_editor.name).removeClass('answer-changed');
		$('#'+my_editor.name).animate({
				backgroundColor: "#FAFAD2",
				borderColor: "#A0981D"
			}, 1000, function() {
    			// Animation complete.
				$('#'+my_editor.name).addClass('clause-changed')
 			}
  		);

	}
	//remove editor
	my_editor.destroy();
}
/*
function change_annotation_selection() {

	annotation_option = getCookie("Drupal.visitor.annotation.option");
	if( annotation_option != "") {
		console.log('annotation_option is set');
		console.log(annotation_option);
		$( "#annotation_options" ).val(annotation_option);
		change_annotation_options('fast');		

	} else {
		console.log('annotation_option is NOT set');
		console.log(annotation_option);
	}

}
*/

function set_footer(){
	$("#document_footer").empty().append(
		$('<div>')
		.append("Document Id: " + getCookie("Drupal.visitor.document.id"))
	);
	$("#document_footer").append(
		$('<div>')
		.append("Version: v" + getCookie("Drupal.visitor.document.version"))
	);
}

function changedAnswer(e) {
	var ref = e.target.id;
	var question_id = $("#"+ref).attr('question_id');
	var answer_id = $("#"+ref).val();

	//alert("You changed the answer for "+ref+"\nThe new value selected is "+answer_id+"\nquestion_id = "+question_id);
	ajax_caller('set_answer_retrieve_new_element', {'document_id':getCookie("Drupal.visitor.document.id"), 'question_id':question_id, 'answer_id':answer_id}, set_answer_retrieve_new_element_callback);
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
	//don't user cookies here, use data.document_id and data.user instead
	ajax_caller('set_answer', {'user':getCookie('Drupal.visitor.user.name'), 'data':JSON.stringify(data)}, set_answer_callback, "POST");

}

function set_answer_callback(data) {
	console.log("set answer callback");
	console.dir(data);
	alert(JSON.stringify(data));
	//alert("set_answer completed.  Redireccting to latest document for document_id"+current_document_id);
	location.href = "load_document?action=Load&document_id=" + getCookie('Drupal.visitor.document.id') + "&version="+getCookie('Drupal.visitor.document.version');

}

function editClause(e) {
var	toolbar = [
			{ 'name': 'basicstyles', 'items' : [ 'Bold', 'Italic', 'Underline', 'Strike', '-', 'RemoveFormat' ]   },
			{ 'name': 'undo', 'items' :['Undo','Redo']}
		];
	// Taking out links for now.
	// toolbar links (not working)...	{ 'name': 'links', 'items' : ['Link','Unlink']},

	console.dir(e);
	var ref = e.target.id;

	my_editor = CKEDITOR.inline(
					ref, 
					{toolbar:toolbar, uiColor: '#d3ebf9', title:'Click to edit clause'}
					);
	my_editor.on( 'change', function( evt ) {
	    // getData() returns CKEditor's HTML content.
	    //console.log( 'Total bytes: ' + evt.editor.getData().length );
	    saveClause(evt);
	});

	//console.log(e.editor.getData());
	/*
	var ref = e.target.id;

	var editable = $("#"+ref).attr("contenteditable");
	if(editable) {
		console.log('save data');
		console.log(e.editor.getData())
	} else {
		console.error("Should not be here");
		alert('p is not editable...  return');
	}
	*/
}

function saveClause(e) {
	var data = [];
	console.dir(e);
	console.log(e.editor.getData());
	console.log(e.editor.name);
	var document_element_id = e.editor.name.substring(7) ;
	console.log(document_element_id);
	var data = {document_id: getCookie('Drupal.visitor.document.id'),
				document_element_id: e.editor.name.substring(7),
				column_text: e.editor.getData(),
				update_column: "document_element_text",
				answer_changed: 0,
				updated_by: getCookie('Drupal.visitor.user.name')
			};
	//alert(JSON.stringify(data));
	//var data['document_id'] =
	ajax_caller("save_element", data, save_clause_callback);

	/*
	var ref = e.target.id;
	var editable = $("#"+ref).attr("contenteditable");
	if(editable) {
		console.log('Editable blur.  Determine if there is a change');
	}
	*/
}
function save_clause_callback() {
	console.log('Clause saved');
}

function editClauseOld(e) {
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

function user_changed_annotation_option() {
	change_annotation_options('slow');	
}

function change_annotation_options(speed) {
	console.log('speed');
	console.log(typeof speed);
	console.log(speed);

	var annotationOption = $( "#annotation_options" ).val();
	if( annotationOption == 'off') {
		$('#current_annotation_content').hide();
		if(speed == 'slow')
			$('#current_document_content').animate({width:'960px'});
		else
			$('#current_document_content').css('width','960px');

		$('.annotation_footnote').hide();		

	} else {
		if(speed == 'slow') {
			$('#current_document_content').animate({width: '700px'}, "normal", function(){
				$('#current_annotation_content').show();
				$('.annotation_footnote').show();		
			});
		} else {
			$('#current_document_content').css('width', '700px');
			$('#current_annotation_content').show();
			$('.annotation_footnote').show();		
		}
		console.log('Selected '+annotationOption);
		switch(annotationOption) {
		    case "both":
		        $('.confidential_annotation').show();
		        $('.public_annotation').show();
		        console.log('both should be showing');
		        break;
		    case "confidential":
		        $('.confidential_annotation').show();
		        $('.public_annotation').hide();
		        console.log('confidential should be showing');
		        break;
		    case "public":
		        $('.confidential_annotation').hide();
		        $('.public_annotation').show();
		        console.log('public should be showing');
		        break;
		}		
	}
	setCookie("Drupal.visitor.annotation.option", annotationOption, 365);

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
	var version_name;
	var version_index = 0;
	$("#document_version").empty();
	$.each( documents[document_id].versions, function( key, version_id ) {
		version_index++;
		version_name = "v"+version_id;
		if(version_index == documents[document_id].versions.length)
			version_name += " (current)";
		$("#document_version").append($("<OPTION value='" + version_id + "'>" + version_name + "</OPTION>"));
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
//getCookie('Drupal.visitor.document.id')
	ajax_caller("get_answers", {'document_id':getCookie('Drupal.visitor.document.id')}, load_change_answer);
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


function click_archive_version_button () {
	//alert("Clicked freeze version Button");
	var content;
	content = '<p>Archive and lock version '+getCookie('Drupal.visitor.document.version')+"?</p>";
	$(content).dialog({
		resizable: false,
		height: 175,
		width: 300,
		modal: true,
		title: "Archive Current Version",
		buttons: {
			"Yes": function() {
				$( this ).dialog( "close" );
				ajax_caller("archive_version", {
						document_id:getCookie('Drupal.visitor.document.id'), 
						version:getCookie('Drupal.visitor.document.version'),
						updated_by:getCookie('Drupal.visitor.user.name')
					}, lock_version_callback);
			},
			No: function() {
				$( this ).dialog( "close" );
			}
		}
	});		

}
function lock_version_callback(data) {
	setCookie("Drupal.visitor.document.version", data.version, 365);
	location.href = "load_document?action=Load&document_id=" + getCookie('Drupal.visitor.document.id') + "&version="+getCookie('Drupal.visitor.document.version');
}

function create_word_file_callback(data) {
	if(data.status == "Error") {
		alert("Error: "+data.message);
	}

//	alert("how did it go? Opening file now....");
	drupal_goto("word/"+data.filename);
	console.log("Look for word document on ther server under the word directory.");
	console.dir(data);
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
function set_toolbar_buttons(editable) {

	// Remove buttons if not editable
	if(editable) {
		$('#change_answer_button').show();
		//$('#save_button').show();
		$('#archive_version_button').show();
	} else {
		$('#change_answer_button').hide();
		//$('#save_button').hide();
		$('#archive_version_button').hide();
	}

}

function get_document_elements_callback(data) {
//	alert (JSON.stringify(data, null, 2));
	console.log("Get the highlighting right");
	console.log("Lock document if uneditable");
	console.log("Determine if this is the current. Editable document");
	console.log("Here is the data");
	console.log("get full document");
	set_toolbar_buttons(data.editable);


	console.dir(data);
	$("#current_document_content").empty().append($('<div>', {'id':'title_bar'}));
	if(data.editable) {
		$("#current_document_content").removeClass("document-locked");
	} else {
		$("#current_document_content").addClass("document-locked");
		$("#title_bar").append('<i class="fa fa-lock fa-2x fa-lock-style" title="Archived document is not editable"></i>');
	}
	//$("#current_document_content").append($('<span', {'class':'document-title'}).append(data.title));
	$("#title_bar").append(
			$('<span>').addClass('document-title').append(data.title)
		);
	$("#title_bar").append(
			$('<span>').addClass('document-version').append('(version '+data.version+')')
		);
//	$("#current_document_content").append($("<h1>").append(data.title));
	$("#current_document_content").append("<hr />");
	//$("#current_document_content").append('<i class="fa fa-camera-retro fa-3x"></i> fa-3x');
	//displayCurrentDocument(data);
	
	var current_section = "";
	var confidential_annotation;
	var public_annotation;
	var annotation_footnote;
	var section_reference;
	var section_number = 0;
	var clause_number = 0;
	var data_clause;
	var element_id = 'current_document_content';
	for (var i=0; i<data.clauses.length; i++) {
		if (!(data.clauses[i].text == 'silent') ) {
			if (data.clauses[i].section != current_section) {
				// Add new section
				//$("#current_document_content").append("<br /><br />");
				//$("#current_document_content").append($("<h2>").append(data.clauses[i].section));
				current_section = data.clauses[i].section;
				section_number++;
				clause_number = 0;  //Reset clause number for each new section
				//alert(section_number);
				displaySectionHeader(section_number, current_section, element_id);
			}
			clause_number++;
			section_reference = section_number+"."+(i+1);
			//data_clause = ​typeof data.clauses[i].text;
			//alert(data_clause);
			
			displayClauseParagraph(section_number, clause_number, data.clauses[i], i, "accordion-content-"+section_number, data.editable);

			/*
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
			*/
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

	change_annotation_selection();
	//$( ".clause-paragraph" ).accordion( "option", "active", 2 );

	$(".accordion").accordion({collapsible: true, 
							heightStyle: "content",
							icons: { "header": "ui-icon-triangle-1-w", "activeHeader": "ui-icon-triangle-1-s" } 
					});
	$(".accordion").removeClass("ui-state-active");
	$(".ui-accordion-content").removeClass("ui-cornner-bottom");

	var options = $( ".accordion" ).accordion( "option" );
	console.dir(options);

	//var widget = $( ".accordion" ).accordion( "widget" );

	//$( ".accordion" ).accordion("option", "icons", null);

}

function displayClauseParagraph(section_number, minor_number, clause, index, element_id, editable) {

	$("#"+element_id).append(
		$('<div/>', {'class': 'clause-paragraph'}).append(
		    $('<div/>', {'class': 'minor-version'}).append(
		        section_number+"-"+minor_number
		    )
		)
		.append(
		    $('<div/>', {'class': 'clause-container'}).append(
		    	$('<p/>', {'class':'clause'}).append(
		        	clause.text
		        )
		        .attr('id', 'clause-'+index)
		    )
		)
	);

	console.log("document_version = "+parseInt(clause.document_verison));
	if(parseInt(clause.document_version) > 0) {
		$('#clause-'+index).addClass('clause-changed');
	}
	console.log("clause.answer_changed = "+parseInt(clause.answer_changed));
	if(parseInt(clause.answer_changed) == 1) {
		$('#clause-'+index).addClass('answer-changed');
	}
	
	if(editable) {
		$('#clause-'+index).attr('contenteditable', 'true');
	} else {
		$('#clause-'+index).removeClass('clause').addClass('clause-locked');
	}
}

function displaySectionHeader(section_number, current_section, element_id) {
	console.log("section_number = " + section_number);
	console.log("current_section = " + current_section);
	console.log("element_id = " + element_id);

	$("#"+element_id).append(
		$('<h3/>', {'class': 'accordion'}).append(
		    $('<div/>', {'class': 'clause-paragraph'}).append(
		        $('<span/>', {'class': 'major-version'}).append(
		            section_number
		        )
		    )
		    .append(
		        $('<span/>', {'class': 'section-name'}).append(
		            current_section
		        )
		    )
		).attr('id','accordion-'+section_number)
	);
/*
	$("#accordion-"+section_number).append(
	    $('<div/>', {'class': 'clear'})
	);
*/
	$("#accordion-"+section_number).append($('<div/>', {'id':'accordion-content-'+section_number}));

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

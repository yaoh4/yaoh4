jQuery(function($) {
var documentSections;
var clause_editor = [];
var my_editor;
var conf_total = 0;
var pub_total = 0;

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
	$("#change_permission_button").click(click_change_permission_button);
	$("#back_button_answer").click(click_back_button_answer);

	$('#document_select').change(changed_document_id);
	$("#select_document_button").click(click_select_document_button);
	$("#annotation_options").change(user_changed_annotation_option);

 	$("#current_document_content").on( "focus", "p.clause", function(event) {
  	editClause(event);
	});
	$("#current_document_content").on( "blur", "p.clause", function(event) {
		updateClauseParagraph();
	});
	$("#current_document_content").on( "click", "a", function(event) {
		event.preventDefault();
		scrollToAnchor('#'+this.id);
	});

	$("#current_annotation_content").on( "click", "p", function(event) {
		if($('#'+event.target.id).attr("annotate-editable") == "true") {
				editAnnotation(event);
		}
	});

	$("#second_page").on( "change", "select.change-answer-select", function(event) {
		changedAnswer(event);
	});

	$("#second_page").on( "change", "select.permission-select", function(event) {
		changeDocumentPermissions(event);
	});

	$("#second_page").on( "change", "select.current-owner-select", function(event) {
		changeDocumentOwner(event);
	});

	var querystring = getQueryString();
	if (querystring["action"] == 'Load') {
		setCookie("Drupal.visitor.document.id", querystring["document_id"], 365);
		setCookie("Drupal.visitor.document.version", querystring["version"], 365);
	}
	if(getCookie('Drupal.visitor.document.id') != "") {
		change_annotation_selection();
		ajax_caller("get_full_document", {'document_id':getCookie("Drupal.visitor.document.id"), 'version':getCookie("Drupal.visitor.document.version")}, get_document_elements_callback);
	} else {

		$("#current_document_container").hide();
		$("#document_footer").hide();
		click_load_button();
	}
	change_annotation_options('fast');
	set_footer();

});

function changeDocumentPermissions(event) {
	var ref = event.target.id;
	var question_id = $("#"+ref).attr('question_id');
	var answer_id = $("#"+ref).val();

//	alert("You changed the answer for "+ref+"\nThe new value selected is "+answer_id+"\nquestion_id = "+question_id);
	ajax_caller('set_document_permissions',
			{'document_id':getCookie("Drupal.visitor.document.id"),
			'question_id':question_id,
			'answer_id':answer_id},
			generic_callback);
}

function changeDocumentOwner(event) {

	var ref = event.target.id;
	var user_id = $("#"+ref).val();
	var selected_user = $("#"+ref+" option:selected").text();

	var content;
	content = "<div id='change-document-owner'><span id='change-document-owner-message'>";
	content += "<i class='fa fa-exclamation-triangle icon-warn'></i>";
	content += "<b>Warning:</b>  ";
	content += "You are about to transfer ownership of this document and you may not be able to edit this document after confirmation.";
	content += "<br><br>Are you sure you want to assign <b>"+selected_user+"</b> as the new <b>Document Owner</b>?";
	content += "<br></span>";
	content += "</div>";

	$(function() {
		$(content).dialog({
			resizable: false,
			width:450,
			height:250,
			modal: true,
			title: "Change Document Owner",
			buttons: {
				"Change Document Owner": function() {
					$(this).dialog( "close" );
					load_current_document();
					ajax_caller('set_document_owner', {'document_id':getCookie("Drupal.visitor.document.id"),'user_id':user_id}, generic_callback);
				},
				Cancel: function() {
					//alert('Reselect Original without kicking off on change.');
					$(this).dialog( "close" );
				}
			}
		});
	});

}

function downloadDocument(document_type) {
//	console.dir(e);
//	e.preventDefault();  //stop the browser from following

    var link = "download_document?document_id=" + getCookie('Drupal.visitor.document.id')
    	+"&version="+getCookie('Drupal.visitor.document.version')
    	+"&user="+getCookie('Drupal.visitor.user.name')
    	+"&document_type="+document_type;

    if(document_type == "PDF") {
			window.open(link);
    } else {
			location.href = link;
    }

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

function change_annotation_selection() {

	annotation_option = getCookie("Drupal.visitor.annotation.option");
	if( annotation_option != "") {
		//console.log('annotation_option is set');
		//console.log(annotation_option);
		$( "#annotation_options" ).val(annotation_option);
		change_annotation_options('fast');

	} else {
//		console.log('annotation_option is NOT set');
//		console.log(annotation_option);
	}

}


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
  //
  //  Disable all other change answer drop downs and the back button.
  //  This will help avoid confusion when change answer rest call is occuring.
  //
  $("#change-answer").find('select').attr('disabled', 'disabled');
  $("#back_button_answer").attr('disabled', 'disabled');

  $("#"+ref).parent().find('span').fadeIn();
	ajax_caller('set_new_answer',
			{'document_id':getCookie("Drupal.visitor.document.id"),
			'question_id':question_id, 'answer_id':answer_id},
			load_current_document);
}

function load_current_document() {
	location.href = "load_document?action=Load&document_id=" + getCookie('Drupal.visitor.document.id') + "&version=current";
}

function set_answer_retrieve_new_element_callback(data) {
//	alert(JSON.stringify(data));
//	alert("set_answer_retrieve_new_element_callback completed. Send clause to madlib to add demographic info. " + current_document_id);
	//Add madlib to clause with demographic answers.
	alert("about to call addMadLib");
//	console.log('data.demographic_answers');
//	console.log(data.demographic_answers);

	//console.log('JSON.parse(data.demographic_answers)');
	//console.dir(JSON.parse(data.demographic_answers));
	//alert("about to call addMadLib");
	//
	//  Perform addMadLib and send back to server
	//

	var new_mad_lib = addMadLib(data.element.document_element_text, JSON.parse(data.demographic_answers));
	data.element.document_element_text = new_mad_lib;
	//don't user cookies here, use data.document_id and data.user instead
	ajax_caller('set_answer', {'user':getCookie('Drupal.visitor.user.name'), 'data':JSON.stringify(data)}, set_answer_callback, "POST");

}

function set_answer_callback(data) {
//	console.log("set answer callback");
//	console.dir(data);
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

	//console.dir(e);
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

  console.log('Hello, save clause');
  console.log(e.editor.getData());
	//console.dir(e);
	//console.log(e.editor.getData());
	//console.log(e.editor.name);
	var document_element_id = e.editor.name.substring(7) ;
	//console.log(document_element_id);
  //Make sure text always has at least one character.  Otherwise the clause goes away.
  //We don't have a method to delete a clause.
  //
  var current_clause = e.editor.getData();
  if(current_clause.length == 0) {
    current_clause = "&nbsp;";
  }
	var data = {document_id: getCookie('Drupal.visitor.document.id'),
				document_element_id: e.editor.name.substring(7),
				column_text: encodeURIComponent(current_clause),
				update_column: "document_element_text",
				answer_changed: 0,
				updated_by: getCookie('Drupal.visitor.user.name')
			};
	//alert(JSON.stringify(data));
	ajax_caller("save_element", data, check_ajax);

	/*
	var ref = e.target.id;
	var editable = $("#"+ref).attr("contenteditable");
	if(editable) {
		console.log('Editable blur.  Determine if there is a change');
	}
	*/
}

function check_ajax(data){
  if(data.status == "Error") {
    console.warn("crada ajax error: "+data.message);
    alert(data.message);
  }
}
function save_clause_callback() {
	console.log('Clause saved');
}

function editAnnotation(e) {
	var ref = e.target.id;
	var content;

	var data_annotate = $("#"+ref).attr("data-annotate");
	//console.log("data_annotate");
	//console.log(data_annotate);

	//
	//Make a unique number for TEXTAREA.  The second save is causing problems because id is not unique.
	//
	var unique = Math.floor(Math.random() * (99999 - 10000 + 1));
	content = '<textarea id="annotate-textarea-'+unique+'" rows="5" cols="70" class="edit_annotation_textarea">';
	content += data_annotate;
	content += '</textarea>';

	$(content).dialog({
		resizable: false,
		height:430,
		width:550,
		modal: true,
		title: $("#"+ref).attr("dialog-title"),
		buttons:{
			Save: function() {
				$( this ).dialog( "close" );
				var textarea = $('#annotate-textarea-'+unique).val();
				//textarea = $(textarea).text();
				console.log("TEXTAREA");
				console.log(textarea);
				var header = $("#"+ref).text();
				header = header.substr(0, header.search(']') + 1);
				var newText = '<b>'+header+'</b> '+ trimAnnotation(textarea);
				$("#"+ref).html(newText);
				$("#"+ref).attr("data-annotate", textarea);
				$("#"+ref).attr("title", textarea);
				updateAnnotateData(ref, textarea);
			},

			Cancel: function() {
				$( this ).dialog( "close" );
			}
		}
	});
}

function updateAnnotateData(ref, data) {
	//console.log("updateAnnotateData Record in database ....");
	//console.log("REF --- DATA");
	//console.log(ref);
	//console.log(data);
}

function user_changed_annotation_option() {
	change_annotation_options('slow');
}

function change_annotation_options(speed) {
	//console.log('speed');
	//console.log(typeof speed);
	//console.log(speed);

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
		//console.log('Selected '+annotationOption);
		switch(annotationOption) {
		    case "both":
		        $('.confidential_annotation').show();
		        $('.public_annotation').show();
		        //console.log('both should be showing');
		        break;
		    case "confidential":
		        $('.confidential_annotation').show();
		        $('.public_annotation').hide();
		        //console.log('confidential should be showing');
		        break;
		    case "public":
		        $('.confidential_annotation').hide();
		        $('.public_annotation').show();
		        //console.log('public should be showing');
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
		width:[350],
		modal: true
	});


}

function click_load_button () {
	///
	// Determine if documents are available for this user
	//
	ajax_caller("get_document_count", null, get_document_count_callback);

}

function get_document_count_callback(data) {
	//console.log("get_document_count_callback");
	//console.dir(data);

	if(data.count> 0) {
		$("#load_dialog").dialog("open");
		ajax_caller("get_all_documents_info", null, load_document_info_into_select);
	} else {
		//User has no documents
		// Redirect to instructions
		$("#current_document_container").empty();
		$("#current_document_container")
			.append($("<h2>")
				.append("No Documents are Accessable")

			).append($("<hr>"));
		$("#current_document_container")
			.append($("<p>")
				.append("No documents are currently active or available for your roles.")
			);
		$("#current_document_container")
			.append($("<p>")
				.append("Please contact an administrator to ensure you have the correct roles.")
			);
		$("#current_document_container").show();

	}

}

function changed_document_id() {

	var document_id = $("#document_select").val();
  var max_version = $("#document_select option:selected").attr('max_version');
  //alert("changed.. max_version: "+max_version);
	//setCookie("Drupal.visitor.document.id", document_id, 365);
	//set the load_latest to 1
  load_document_versions_into_select(max_version, 'current');

	//setCookie("Drupal.visitor.document.loadLatest", 1, 365);
//	ajax_caller("get_all_documents_info", null,	load_document_version_into_select);
}

function load_document_info_into_select(data) {
	var documents = data.documents;
	var document_id = parseInt(getCookie("Drupal.visitor.document.id"));
	var max_version;

	//console.log("load_document_info_into_select");
	//console.dir(data);

	if(isNaN(document_id)) {
		document_id = $("#document_select").val();
		//setCookie("Drupal.visitor.document.id", document_id, 365);
	};

	//populate select
	$("#document_select").empty();
	$.each(documents, function( key, doc ) {

		$("#document_select")
			.append($("<OPTION>")
				.attr("value", doc.document_id)
				.append(doc.title)
				.attr('max_version', doc.version)
			);
		if(doc.document_id == document_id) {
			max_version = doc.version;
		}

	});

	//select the current document
	$("#document_select").val(document_id).prop('selected', true);

	load_document_versions_into_select(max_version, parseInt(getCookie("Drupal.visitor.document.version")));
}

function load_document_versions_into_select(max_version, selected) {

	if(isNaN(selected)) {
		//document_id = $("#document_select").val();
		selected = max_version;
		//setCookie("Drupal.visitor.document.version", selected, 365);
	};

	$("#document_version").empty();
	// Put in the versions.
	//console.log("CHANGED DOCUMENT");
	//console.log("max_version "+max_version);
	//console.log("selected "+selected);
	for (i = 0; i <= max_version; i++) {

		version = "v" + i;

		if(i == max_version) {
			version += " (current)";
		}

		$("#document_version").
			append($("<OPTION>")
				.attr("value", i)
				.append(version)
			);
	}

	$("#document_version").val(selected).prop('selected', true);
	stop_spinner('load_dialog_spinner','load_dialog_select');

}

function load_document_version_into_select_old(data) {
	var documents = data.documents;
	var version_id = parseInt(getCookie("Drupal.visitor.document.version"));
	var document_id = $("#document_select").val();
	//console.log("document_id = "+document_id);

	//populate select
	var version_name;
	var version_index = 0;
	$("#document_version").empty();
	$.each(documents[document_id].versions, function( key, version_id ) {
		version_index++;
		version_name = "v"+version_id;
		if(version_index == documents[document_id].versions.length)
			version_name += " (current)";
		$("#document_version").append($("<OPTION value='" + version_id + "'>" + version_name + "</OPTION>"));
	});

	if(isNaN(version_id)) {
		version_id = $("#document_version").val();
		//setCookie("Drupal.visitor.document.version", version_id, 365);
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
	$('#second_page_container').hide();
}

function add_spinner(id) {
	$('#'+id).empty().append('<div></div><p class="second-page-intro"><i class="fa fa-spinner fa-spin fa-3x"></i></p>');
}

function show_second_page() {
	$('#current_document_container').hide();
	$('#second_page_container').show();
}

function click_change_permission_button() {
	add_spinner("second_page");
	show_second_page();
	ajax_caller("get_document_permissions", {'document_id':getCookie('Drupal.visitor.document.id')}, load_change_permission);
}

function load_change_permission(data) {
	var instructions = '<p class="second-page-intro">Change document access permissions or document owner.</p>';
	$('#second_page').empty().append(instructions);

	$('#second_page').append(
		$("<form>")
			.attr('id', 'change-permission')
	);
	$('#change-permission').append(
		$("<h2>")
			.append('Document Role Access Permissions')
			.addClass('current_question')
	);
	$('#change-permission').append(
		$("<hr>")
	);

	var permission_answers = ["", "read", "write"];
	//
	// Add header
	//
	$('#change-permission').append(
		$('<div>')
			.attr('id', "question-header")
	);
	$('#question-header').append(
		$("<div>")
				.append("Role")
				.addClass('question-label')
				.addClass('question-header')
				.attr('style', 'width:200px;')
	);
	$('#question-header').append(
		$("<div>")
				.append("Access")
				.addClass('question-label')
				.addClass('question-header')
				.attr('style', 'text-align: left;')
	);

	$('#question-header').append(
		$("<div>")
			.addClass('both')
	);
	//
	// Add all groups with selectable access
	//
	$.each(data , function( key, value ) {
		//console.dir(value);

		$('#change-permission').append($('<div>').attr('id', key).addClass('form-group'));
		//Add question LABEL
		$('#'+key).append(
			$("<label>")
				.append(value.name)
				.addClass('question-label')
				.attr('style', 'width:200px;')
				.attr('for', 'permission-'+value.rid)
			);
		//Add question SELECT
		$('#'+key).append(
			$("<select>")
				.attr('id', 'permission-'+value.rid)
				.attr('name', 'permission-'+value.rid)
				.attr('class', 'permission-select')
				.attr('question_id', value.rid)
		);
		//Add question OPTIONS
		for(i=0;permission_answers.length>i;i++) {
			$('#permission-'+value.rid).append(
				$('<option>')
					.append(permission_answers[i])
					.attr('value', permission_answers[i])
			);
		}
		//
		//Display answer
		//
		/*

		if(value.access == null){
			$('#'+key).append(
				$("<span>")
					.append("answer is null")
					.css('color', 'red')
			);
		} else {
			$('#'+key).append(
				$("<span>")
					.append(value.access)
			);
			//Select correct answer
			$("#permission-"+value.rid).val(value.access).prop('selected', true);

		}
		*/
		if(value.access != null) {
			$("#permission-"+value.rid).val(value.access).prop('selected', true);
		}

		$('#'+key).append(
			$("<div>")
				.addClass('both')
		);

	});

	// Add Document Owner Section
	ajax_caller("get_document_owners", {'document_id':getCookie('Drupal.visitor.document.id')}, load_change_owner);

}

function load_change_owner(data) {

	$('#change-permission').append(
		$("<h2>")
			.append('Document Owner')
			.addClass('current_question')
	);

	$('#change-permission').append(
		$("<hr>")
	);

	var key = "document_owner";

	console.log("CHANGE_OWNER data");
	console.dir(data);

	$('#change-permission')
		.append($('<div>')
		.attr('id', key)
		.addClass('form-group')
		.attr('style', 'padding-top:25px;padding-bottom:150px;')

	);
	//Add question LABEL
	$('#'+key).append(
		$("<label>")
			.append("Current Document Owner")
			.addClass('question-label')
			.attr('style', 'width:200px;')
			.attr('for', 'current-owner')
		);
	//Add question SELECT
	$('#'+key).append(
		$("<select>")
			.attr('id', 'current-owner-select')
			.attr('name', 'current-owner')
			.attr('class', 'current-owner-select')
			.attr('original_owner', data.current_document_owner_uid)
	);
	//Add question OPTIONS

	for(i=0;data.users.length>i;i++) {
		$('#current-owner-select').append(
			$('<option>')
				.append(data.users[i].name+' ('+data.users[i].mail+')'
					)
				.attr('value', data.users[i].uid)
		);
	}
	//select current owner
	if(data.current_document_owner_uid != null) {
		$('#current-owner-select').val(data.current_document_owner_uid).prop('selected', true);
	}

	$('#'+key).append(
		$("<div>")
			.addClass('both')
	);

}

function click_change_answer_button() {
	add_spinner("second_page");
	show_second_page();
	ajax_caller("get_answers", {'document_id':getCookie('Drupal.visitor.document.id')}, load_change_answer);
}

function load_change_answer(data) {
	//change_answer_questions
	console.log("load_change_answer");
	console.dir(data);

/*
	$('#change_answer_questions').empty().append(
		$("<p>")
			.append("Changing a document answer below will immediately replace the appropriate clauses into the current document.")
			.addClass("change-answer-intro")
	);
*/
	var instructions = '<p class="second-page-intro">Changing a document answer below will immediately replace the appropriate clauses into the current document.</p>';
	$('#second_page').empty().append(instructions);
	$('#second_page').append(
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
				.addClass('change-answer-select')
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
		if(value.source_answer == null){
			$('#'+key).append(
				$("<span>")
					.append("answer is null")
					.css('color', 'red')
			);

		} else {
			$('#'+key).append(
				$("<span>")
          .css('display','none')
					.append('<i class="fa fa-spinner fa-spin fa-2x" style="font-size:1.5em;margin-left:15px;margin-right: 10px;"></i><b>Changing Answer</b>')

			);
			//Select correct answer
			$("#question-"+value.question_id).val(value.source_answer).prop('selected', true);

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
	//console.log("Look for word document on ther server under the word directory.");
	//console.dir(data);
}

function click_select_document_button() {
//	alert("Clicked Select Document Button: " + $("#document_select").val());

	document_id = $("#document_select").val();
	version_id = $("#document_version").val();
	//Set cookies
	setCookie("Drupal.visitor.document.id", document_id, 365);
	setCookie("Drupal.visitor.document.version", version_id, 365);

	set_footer();
	change_annotation_selection();
	ajax_caller("get_full_document", {'document_id':document_id, 'version':version_id}, get_document_elements_callback);
	$("#load_dialog").dialog( "close" );
	$("#current_document_container").show();
	$("#document_footer").show();

}

function set_toolbar_buttons(editable) {
	// Remove buttons if not editable
	if(editable) {
		$('#change_answer_button').show();
		$('#change_permission_button').show();
		$('#archive_version_button').show();
	} else {
		$('#change_answer_button').hide();
		$('#change_permission_button').hide();
		$('#archive_version_button').hide();
	}
}

function get_document_elements_callback(data) {

	setCookie("Drupal.visitor.document.version", data.version, 365);
	console.log('data');
	console.dir(data);
	set_toolbar_buttons(data.editable);
	//Hide set Permisson
	if(parseInt(data.document.set_permission) == 0) {
		$('#change_permission_button').hide();
	}

  if(data.access == 'none') {
    // No document access.
    $("#current_document_content")
      .empty()
      .append(
          $('<div>')
            .append('You are not authorized to access this page.')

    );
    return;
  }
	//Clear document content
	$("#current_document_content").empty().append($('<div>').attr('id', 'title_bar'));
	if(data.editable) {
		$("#current_document_content").removeClass("document-locked");
	} else {
		$("#current_document_content").addClass("document-locked");
		$("#title_bar").append('<i class="fa fa-lock fa-2x fa-lock-style" title="Archived document is not editable"></i>');
	}
	//Append to title_bar
	$("#title_bar").append(
			$('<span>').addClass('document-title').append(data.document.title)
		);
	$("#title_bar").append(
			$('<span>').addClass('document-version').append('(version '+data.version+')')
		);
	//Add <hr>
	$("#current_document_content").append("<hr />");
	// Add document sections
	var current_section = "";
	var confidential_annotation;
	var public_annotation;
	var annotation_footnote;
	var section_reference;
	var section_number = 0;
	var clause_number = 0;
	var data_clause;
	var element_id = 'current_document_content';
	var conf_total = 0;
	var pub_total = 0;
	var annotation_positions = [];
	var annotation_detail = [];
	var annotate = [];
	var survivable_clauses = [];

	//Walk through each clause
	for (var i=0; i<data.clauses.length; i++) {
		if (!(data.clauses[i].text == 'silent')) {
			if (data.clauses[i].section != current_section) {
				// Add new section
				current_section = data.clauses[i].section;
				section_number++;
				clause_number = 0;  //Reset clause number for each new section
				displaySectionHeader(section_number, current_section, element_id);
			}
			clause_number++;
			section_reference = section_number+"-"+clause_number;
			//if(section_number == 5) { // For testing only
			var position = displayClauseParagraph(section_number, clause_number, data.clauses[i], i, "accordion-content-"+section_number, data.editable);
			var annotate_detail = addAnnotationDiv(data.clauses[i], section_reference, position, data.editable, "clause-"+i);
			if(annotate_detail.length > 0) {
				annotate.push(annotate_detail);
			}
			annotation_positions.push(position);
			//} //For testing only
			if(data.clauses[i].survivable == 1){
				survivable_clauses.push(section_reference);
			}
		}
	}

// Add survivability statement at bottom of document
	printSurvivabilityStatement(survivable_clauses);

	var element = document.getElementById("current_document_content");
	var rect = element.getBoundingClientRect();
	//console.log("BIG RECTANGLE");
	//console.dir(rect);
	//console.log("MY ANNOTATE");
	//console.dir(annotate);

	drawAnnotate(annotate, rect);

	/*
 	<canvas id="myCanvas" width="200" height="100"
		style="border:1px solid #000000;">
	</canvas>
	*/
	// Make a canvas like that...

	//console.log(rect.top, rect.right, rect.bottom, rect.left);

	//$( ".annotation" ).tooltip({ track: true });
	$( ".annotation" ).tooltip({
		show: {
			effect: "slideDown",
			delay: 250
		}
	});

	//$( ".clause-paragraph" ).accordion( "option", "active", 2 );
/*
	$(".accordion").accordion({collapsible: true,
			heightStyle: "content",
			icons: { "header": "ui-icon-triangle-1-w", "activeHeader": "ui-icon-triangle-1-s" }
	});
*/
//	$(".accordion").removeClass("ui-state-active");
//	$(".ui-accordion-content").removeClass("ui-cornner-bottom");
/*
	var options = $( ".accordion" ).accordion( "option" );
	console.dir(options);
	*/
}

function printSurvivabilityStatement(survivable_clauses) {

		var survivability_statement;
		var label = (survivable_clauses.length == 1) ? "Paragraph" : "Paragraphs";

		if(survivable_clauses.length > 0) {
			survivability_statement = "<strong>Survivability</strong>.  ";
			survivability_statement += "The provisions of "+label+" ";
			survivability_statement += explodeLegalArray(survivable_clauses, "clause");
			survivability_statement += " will survive the expiration or early termination of this CRADA.";

			$("#current_document_content")
					.append($("<div>")
						.addClass('clause-container')
								.append($('<p>')
									.addClass('survivability-statement')
									.append(survivability_statement)
								)
					);
		}
}

function scrollToAnchor(aid){
		var aTag = $("a[name='"+ aid +"']");
		$('html,body').animate({scrollTop: aTag.offset().top},'slow');
}

function drawAnnotate(annotate, rect) {

	var canvas_height = rect.height;
	var canvas_width = 60;
	//Create two canvases.  One for confidential and one for public
	var annotate_canvas = '<canvas id="conf_canvas" width="'+canvas_width+'" height="'+canvas_height+'" style="margin-left:-50px;">Canvas</canvas>';
	$("#current_annotation_content").append(annotate_canvas);
	annotate_canvas = '<canvas id="pub_canvas" width="'+canvas_width+'" height="'+canvas_height+'" style="margin-left:-600px;">Canvas</canvas>';
	$("#current_annotation_content").append(annotate_canvas);

	var conf_canvas = document.getElementById("conf_canvas");
	var pub_canvas = document.getElementById("pub_canvas");
	var conf_context = conf_canvas.getContext("2d");
	var pub_context = pub_canvas.getContext("2d");


/*
	for (var x = 0.5; x < canvas_width; x += 10) {
  	conf_context.moveTo(x, 0);
  	conf_context.lineTo(x, canvas_height);
  	pub_context.moveTo(x, 0);
  	pub_context.lineTo(x, canvas_height);
	}
*/

//Draw vertical lines
/*
	for (var y = 0.5; y < canvas_height; y += 10) {
	  conf_context.moveTo(0, y);
	  conf_context.lineTo(canvas_width, y);
	  pub_context.moveTo(0, y);
	  pub_context.lineTo(canvas_width, y);
	}
	conf_context.strokeStyle = "#eee";
	conf_context.stroke();
	pub_context.strokeStyle = "#eee";
	pub_context.stroke();
*/
	//Draw arrows
	/*
	conf_context.moveTo(15, 0);
	conf_context.lineTo(10, 50);
	conf_context.moveTo(10, 173);
	conf_context.lineTo(10, 375);
	conf_context.moveTo(5, 370);
	conf_context.lineTo(10, 375);
	conf_context.lineTo(15, 370);
	*/
	//conf_context.strokeStyle = "red";
	//conf_context.stroke();
	//console.log("ANNOTATION POSITION");
	//console.dir(annotation_positions);

	//console.log("ANNOTATE");
	//console.dir(annotate);
	$.each(annotate, function(key, value) {
			$.each(value, function(key2, value2) {
					//console.dir(value2);
					//console.log(value2.top);
					//console.log('From ('+(canvas_width-1)+','+(value2.top+ value2.annotate_offset)+') To:(0,'+value2.top+')');
					conf_context.moveTo(canvas_width-1, value2.top + value2.annotate_offset);
					conf_context.lineTo(0, value2.top);

			});
	});

	//conf_context.strokeStyle = "red";
	//conf_context.stroke();

}


function addAnnotationDiv(clause, section_reference, position, editable, clause_id) {
	//console.log('clause');
	//console.dir(clause);
	//console.log('section_reference');
	//console.info(section_reference);
	//console.log('position');
	//console.info(position);
// Set div editable value

	var annotation_index = 0;
	var annotation_types = ["confidential_annotation", "public_annotation"];
	var ref = "";
	var annotate = [];
	var annotate_offset = 0;
	var offset = 45; //ie. Height of annotate.

//	console.log("POSITION");
//	console.dir(position);

	$.each(annotation_types, function(key, annotation_type) {
	//	console.info("section_reference: "+section_reference+ " top :"+position.clause.top+" annotation_type: "+annotation_type);
		if(annotation_type == "confidential_annotation") {
			raw_annotations = clause.confidential_annotation;
			annotation_id = "CONF";
			//console.log(JSON.stringify(clause.confidential_annotation));
		} else{
			//alert('Pub');
			raw_annotations = clause.public_annotation;
			annotation_id = "PUB";
			//console.log(JSON.stringify(clause.public_annotation));
		}
	//split the annotation data on excel double return ie \n\n
		annotations = raw_annotations.split('\n\n');
		//console.info(annotations.length);
		var r = 0; //annotation revision number for paragraph
		$.each(annotations, function(key, annotation) {
			r++;
			if( JSON.stringify(annotation).length > 2) {
				annotate_div = $("<div/>").html(annotation).text();
				annotation_short = trimAnnotation(JSON.stringify(annotation));
				if(annotation_type == "confidential_annotation") {
					conf_total++;
					annotation_index = conf_total;
				} else {
					pub_total++;
					annotation_index = pub_total;
				}
				ref = annotation_id+""+annotation_index;
				if(r>1) {
					//append Revsion
					ref += "R"+r;
				}

				annotation_footnote = "Section "+section_reference+" ["+ref+"]";
				//addAnnotationDiv(data.clauses[i].confidential_annotation, 'Confid', "current_annotation_content");
				//console.log("ADDING ANNOTATION");
				var x = position.clause.top - position.clause.offset;
				//console.log(x);
				$("#current_annotation_content")
					.append(
						$("<p>")
							.addClass("contract_clause")
							.append( "<b>" + annotation_footnote + "</b> " + annotation_short)
							.addClass('annotation')
							.addClass(annotation_type)
							.attr("id", ref)
							.attr("data-annotate", annotate_div)
							.attr("title", annotation)
							.attr("dialog-title", annotation_footnote)
							.css("position", "absolute")
							.css("left", "0px")
							.css("top", x)
				);
				//console.info("editable ="+ editable);
				if(editable) {

					$("#"+ref)
						.attr("annotate-editable", "true");
				}

			// add
				var annotate_position = {
					ref : ref,
					height: 30,
					section_reference: section_reference,
					top : position.clause.top,
					annotate_offset: annotate_offset,
					clause_id: clause_id,
					editable: editable
				};
				annotate.push(annotate_position);
				annotate_offset += offset;

			}
		});
//console.log("ANNOTATE");
//sconsole.dir(annotate);
/*
		if(annotations.length > 1) {
			console.log('annotations with \n\n');
			console.dir(annotations);
		}
*/


	});
	return annotate;
	//annotations = JSON.stringify(annotation_data);
	//console.log("section_reference: ");
	//console.dir(annotation_data);
}

function displayClauseParagraph(section_number, minor_number, clause, index, element_id, editable) {

//	console.info('section_number '+section_number);
//	console.info('minor_number '+minor_number);
//	console.info('clause '+clause);
//	console.info('index '+index);
//	console.info('element_id '+element_id);
	//var clause_text = clause.text;
	//Search and replace \n with '<br>'
	// WEIRD. This causes too many returns...

	//clause.text = clause.text.replace(/(?:\r\n|\r|\n)/g, '<br>')

	$("#"+element_id)
		.append($('<div>')
					.addClass('clause-paragraph')
					.append($('<a>')
							.attr('name', '#clause-'+section_number+"-"+minor_number)
						)
					.append($('<div>')
								.addClass('minor-version')
								.append(section_number+"-"+minor_number)
					)

		.append(
		    $('<div>', {'class': 'clause-container'}).append(
		    	$('<p>', {'class':'clause'}).append(
		        	clause.text
		        )
		        .attr('id', 'clause-'+index)
		    )
		)
	);

	//console.log("document_version = "+parseInt(clause.document_verison));
	if(parseInt(clause.document_version) > 0) {
		$('#clause-'+index).addClass('clause-changed')
      .attr('alt','This clause has changed from original version.')
      .attr('title','This clause has changed from original version.');
	}
	//console.log("clause.answer_changed = "+parseInt(clause.answer_changed));
	if(parseInt(clause.answer_changed) == 1) {
		$('#clause-'+index).addClass('answer-changed')
      .attr('alt', 'This clause has changed because of a changed answer.')
      .attr('title', 'This clause has changed because of a changed answer.')
      ;
	}

	if(editable) {
		$('#clause-'+index).attr('contenteditable', 'true');
	} else {
		$('#clause-'+index).removeClass('clause')
      .addClass('clause-locked')
      .attr('alt', 'This clause is locked and not editable.');
	}

	var section_position = $('#'+element_id).position();
	var section_offset = $('#'+element_id).offset();
	var section_height = $('#'+element_id).height();

	var clause_position = $('#clause-'+index).position();
	var clause_offset = $('#clause-'+index).offset();
	var clause_height = $('#clause-'+index).height();
//	console.log('cluase-'+index +"  Should exist now...  Go check");

	var element = document.getElementById("clause-"+index);

	var clause_xPosition = (element.offsetLeft - element.scrollLeft + element.clientLeft);
	var clause_yPosition = (element.offsetTop - element.scrollTop + element.clientTop);

	/*
	console.log('position');
	console.dir(position);
	console.log('offset');
	console.dir(offset);
	console.log('height');
	console.log(height);
*/
	section_position['height'] = section_height;
	section_position['offset'] = section_height;

	clause_position['height'] = clause_height;
	clause_position['offset'] = clause_height;
	clause_position['xPosition'] = clause_xPosition;
	clause_position['yPosition'] = clause_yPosition;

	var position = {
		section: section_position,
		clause: clause_position
	};

//	console.dir(position);
	return position;
}

function displaySectionHeader(section_number, current_section, element_id) {
//	console.log("section_number = " + section_number);
//	console.log("current_section = " + current_section);
//	console.log("element_id = " + element_id);

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


function trimAnnotation(annotation) {
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

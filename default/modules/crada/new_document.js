jQuery(function($) {

// Global Variables go here
var document_id;
var sections = [];
var current_section_number;
var answers = {};
var definitions;
var demographics;

var chosen_subsections;

$(document).ready(function ($) {
	$("#begin_question_button").click(function() {
		$("#spinner").show();
		$("#instructions").hide ();
		setup_template_chooser();
	});
	$("#template_chooser_button").click(function() {
		$("#spinner").show();
		$("#choose_template").hide();
		document_id = parseInt($("#template_chooser").val());
		advance_progress_bar("demographics");
		setup_demographics();
	});

	$("#demographics_button").click(function() {
		//Save demographic answers
		demographics = $('#demographic-form').serializeObject();
		$("#spinner").show();
		$("#demographics").hide();
		get_all_definitions(document_id);
		advance_progress_bar("questions");
		identify_subsections();
		setup_sections();
	});
	$("#questions_button").click(function() {
		store_current_answers(current_section_number);
		if (current_section_number == sections.length-1) {
			//Generate Document
			$("#spinner").show();
			$("#spinner p")
				.append($('<span>')
					.append("Generating CRADA Document")
					.attr('style', 'font-weight:bold;font-size:150%;margin-left:10px;')
				);
			$("#progress_bar").hide();
			$("#questions").hide();
			setup_document();
		} else {
			$("#spinner").show();
			$("#questions").hide();
			current_section_number++;
			advance_progress_bar(current_section_number);
			setup_questions_for_section(current_section_number);
		}
	});
});

/*
function add_spinner(id) {
	$('#'+id).empty().append('<div></dir><p class="second-page-intro"><i class="fa fa-spinner fa-spin fa-3x"></i></p>Why is this not working....');
}
*/

function advance_progress_bar(step) {
	switch (step) {
		case 'template':
			$("#initial_step").removeClass("not_done").removeClass("done").addClass("working");
			break;
		case 'demographics':
			$("#initial_step").removeClass("not_done").removeClass("working").addClass("done");
			$("#demographic_step").removeClass("not_done").removeClass("done").addClass("working");
			break;
		case 'questions':
			$("#demographic_step").removeClass("not_done").removeClass("working").addClass("done");
			$("#question_step").removeClass("not_done").removeClass("done").addClass("working");
			break;
		default:
			if (step > 0) $("#question_step_" + (step-1)).removeClass("not_done").removeClass("working").addClass("done");
			$("#question_step_" + step).removeClass("not_done").removeClass("done").addClass("working");
			break;
	}
}

function setup_demographics() {
	ajax_caller('get_demographics', {'document_id': document_id}, get_demographics_callback);
}
function get_demographics_callback(data) {
	display_demographic_questions(data);
}

function display_demographic_questions(data) {
	//$("#demographic_questions").append(JSON.stringify(data));
	//var demographics = data.demographics;
	var row;

	$("#demographic-form")
		.append($('<h2>').append("Demographics")
		.append($('<hr>'))
	);

	$.each( data.demographics, function( i, demographic ) {
		row = "";
		if(demographic.html_type == 'pull down') {
			row = create_demographic_pulldown(demographic);
		} else {
			row = create_demographic_input(demographic);
		}

		row = '<div class="form-group">'+row+'</div>';
		$("#demographic-form").append(row);
	});

	// Add final questions: Alternate text and Subsections
	ajax_caller('get_alternate_text_types', {'document_id': document_id}, get_alternate_text_types_callback);
}

function get_alternate_text_types_callback(data) {

	$("#demographic-form")
		.append(
			$('<h2>').append("Alternate Text")
			.append($('<hr>'))
		);

	$("#demographic-form")
		.append(
			$('<p>')
				.append("This document has alternative text available to support contracts with multiple entities.")
		);

	var alternate_text_select = $("<SELECT id='alternate_text_type'>");
	for (i=0;i<data.types.length;i++) {
		alternate_text_select.append("<OPTION>" + data.types[i] + "</OPTION>");
	}

	$("#demographic-form")
		.append($('<label>')
				.append("Select an Alternative Text Type:")
				.addClass('demographic-label')
				.attr("for", "alternate_text_type")
		);

	$("#demographic-form")
		.append(alternate_text_select);

	ajax_caller('get_subsections', {'document_id': document_id}, get_subsections_callback);

}

function get_subsections_callback(data) {

	$("#demographic-form")
		.append($('<h2>').append("Subsections")
			.append($('<hr>'))
	);

	$("#demographic-form")
		.append($('<P>')
			.append("Select subsections that are applicable to this document:")
		);
	for (i=0;i<data.subsections.length;i++) {
		if (data.subsections[i] != "") {
				$("#demographic-form")
					.append($('<div>')
						.addClass('form-group')
						.addClass('subsections')
						.attr('name', data.subsections[i])
						.append("<LABEL class='demographic-label' for='subsection_"+data.subsections[i]+"' >" + data.subsections[i] + "</LABEL>")
						.append("<SELECT id='subsection_"+data.subsections[i]+"' ><OPTION>Yes</OPTION><OPTION>No</OPTION></SELECT>")
						.append("<div style='clear:both;'></div>")
					);
		}
	}

		stop_spinner('spinner', 'demographics');
}

// Called when the demograhics advanced button is clicked
function identify_subsections() {

	chosen_subsections = new Object();

	$(".subsections").each( function () {
		chosen_subsections[$(this).attr("name")] = $(this).find("SELECT").val();
	});

//	console.log("The chosen subsetions");
//	console.dir(chosen_subsections);

}

function create_demographic_pulldown(demographic) {
	var row;
	//Add the label
	row = "<label for='"+demographic.variable+"' class='demographic-label'>"+demographic.question+"</label>";
	//Add the pulldown box
	row += '<select id="'+demographic.variable+'" name="'+demographic.variable+'">';
	row += '<option value="" selected></option>';
	$.each(demographic.pulldown_options, function( i, pulldown_option) {
		row += '<option value="'+pulldown_option+'">'+pulldown_option+'</option>';
	});
	row += '</select>\n';
	row += '<div style="clear:both;"></div>'
	return row;
}

function create_demographic_input(demographic) {
	var row;
	row = "<label for='"+demographic.variable+"' class='demographic-label'>"+demographic.question+"</label>";
	row += "<input type='text' class='demographic-input'  id='"+demographic.variable+"' name='"+demographic.variable+"'>";
	row += '<div style="clear:both;"></div>'
	return row;
}



function get_all_definitions(document_id) {
	ajax_caller('get_all_definitions', {'document_id': document_id}, get_all_definitions_callback);
}

function get_all_definitions_callback(data) {
//	alert (JSON.stringify(data.definitions, null, 2));
	definitions = {};
	for (i=0;i<data.definitions.length;i++) {
		definitions[data.definitions[i].term] = data.definitions[i].definition;
	}
}

function setup_sections() {
	// first update the progress bar to include each subsection
	ajax_caller('get_section_list', {'document_id': document_id}, setup_sections_callback);

}

function rebuild_progress_bar() {
	var width = 70 / sections.length;

	// We need to completely rebuild the progress bar

	$("#progress_bar").empty().append($("<TR>")
			.append($("<td class='done' id='initial_step'>Select a Base Template</td>"))
			.append($("<td class='done' id='demographic_step'>Demographic Information</td>")));

	for (i=0;i<sections.length;i++){
		$("#progress_bar").find("tr").append(
				$("<TD class='not_done' id='question_step_"+i+"' style='width:"+width+"%'>Questions:<br/>"+sections[i]+"</TD>"))
	}
	$("#progress_bar").find("tr").append($("<td class='not_done' id='conclusion_step'>Edit the Document</td>"));

}

function setup_sections_callback(data) {
	sections = data.sections;
	rebuild_progress_bar();
	current_section_number = 0;
	advance_progress_bar(current_section_number);
	setup_questions_for_section(current_section_number);
}

function setup_questions_for_section(i) {
	//if this is the last section change button text to "Generate Document"
	if(i == sections.length-1) {
		$("#questions_button").text("Generate Document");
	}

	ajax_caller('get_questions_for_section', {'document_id':document_id, 'section':sections[i]}, setup_questions_for_section_callback);

}

function setup_questions_for_section_callback(data) {

//	console.log('Just got a section');
//	console.dir(data);

	var q = data.questions; //Set q to array of questions sent in
	var section_title = q[0].section;
//	console.log('section: '+section_title);

	$("#question_section").empty()
		.append($('<h2>').append(section_title)
		.append($('<hr>'))
		.append($('<div>')
			.attr('id',"question_detail")
			.attr('style', 'padding-left:30px;')
		)
	);
	var display = $("#question_detail");

//	console.log(data);
//	console.log('Questions q for this section');
//	console.log(q);


	var number_of_questions_asked = 0;
//	console.log('q')
	for (i=0;i<q.length; i++) {
		if (q[i].subsection != "" && chosen_subsections[q[i].subsection] == "No") continue;  // If the subsection is No skip question
//		console.log(">>>>" + q[i].subsection + ":" + chosen_subsections[q[i].subsection]);
		if (q[i].text == 'REQUIRED') {
			answers[q[i].question_id] = 0;  //If the question is == "REQUIRED", skip and set answer to 0
		} else {
			answers[q[i].question_id] = 0;  //Set default to 0
//			alert(JSON.stringify(q[i],null,2));
			number_of_questions_asked++;
			display
				.append($('<div>')
					.append(q[i].text)
					.addClass("question-text")
				);
			for (j=0;j<q[i].answers.length; j++) {
				var label_name = 'answer-'+q[i].question_id+"-"+j;
				display
					.append($("<label>")
							.attr('for', label_name)
							.css('display', 'none')
							.append(q[i].answers[j])
						)
					.append($("<INPUT type='radio' name='"+q[i].question_id+"' value='"+j+"'>"+q[i].answers[j] + "</INPUT><br>")
							.attr('checked', (j==0))
							.attr('id', label_name)
						);
			}
			display.append("<br />")
		}
	}
	if (number_of_questions_asked == 0) {
		display.append("<P>This section has no questions.</P>");
	}

	stop_spinner('spinner', 'questions');
}

function store_current_answers(section_number) {
//	console.log("store_current_answers");
//	console.log('section_number');
//	console.log(section_number);

	var question_pane = $("#question_section");
	var str = "";
	$("#question_section").find("INPUT[type=radio]:checked").each(function () {
		str += $(this).attr("name") + ":" + $(this).val() + "\n";
		answers[$(this).attr("name")] = $(this).val();
	})
//	console.dir(answers);
	//alert (JSON.stringify(answers));
}

function setup_template_chooser() {
	ajax_caller('get_document_templates', null, setup_template_chooser_callback);
	$("#choose_template")
		.prepend($('<hr>'))
		.prepend($('<h2>').append("Base Template")
	);

	$("#document_title").focus();
	advance_progress_bar("template");
}

function setup_template_chooser_callback(data) {
//	console.log("setup_template_chooser_callback");
//	console.log(data);

	if(data.templates.length == 0) {
		// Stop Progress because no templates available.
		// Display message to user.
		$("#choose_template > hr")
			.append($('<div>')
				.append($('<p>')
					.append("No <b>Base Templates</b> have been loaded.  Please contact an administrator and ask them to load a <b>Master Base Template</b>.")
					.css('padding', '15px')
					.css('color', 'black')
					.css('width', '400px')
				)
			)
		$('#basic_document_info').remove();
		$('#progress_bar').remove();
	} else {
		for (var i=0; i<data.templates.length; i++) {
			var name = data.templates[i].name;
			$("#template_chooser").append($("<OPTION>").attr('value', data.templates[i].id).append(name));
		}
	}

//	alert (JSON.stringify(data, null, 2));
	stop_spinner('spinner', 'choose_template');
}

function setup_document() {
//	alert (JSON.stringify(answers, null, 2));
	var answers_encoded = JSON.stringify(answers);
//	console.dir(answers);
//	console.log(answers_encoded);
	//alert(answers_encoded);

	var alternate_text_type = $("#alternate_text_type").val();
//	console.log("Alternate Text Type: " + alternate_text_type);
	ajax_caller('get_clauses_from_answers',
		{'document_id':document_id, 'answers':answers_encoded, 'alternate_text': alternate_text_type},
		setup_document_callback, 'POST');
}

function setup_document_callback(data) {
	var new_clauses = new Object();
	var alternate_text = data.alternate_text;
	var used_terms = get_used_terms(data.clauses);


//	console.log("Where are the question/answers????");
//	console.log(used_terms);
//	console.log(data);
	//Adding Definition sections
	for (i=0;i<used_terms.length; i++) {

		if (used_terms[i] == "") continue;
		var definition = definitions[used_terms[i]];
		new_clauses[i] = new Object();
		new_clauses[i].text = "<B>" + add_demographics(used_terms[i]) + "</B>: " + add_demographics(definition);
		new_clauses[i].section = 	"Definitions";
		new_clauses[i].survivable = 0;
//		console.log("new_clauses["+(i)+"]");
//		console.dir(new_clauses[i]);

	}

//	console.log("new_clauses");
//	console.dir(new_clauses);
//	console.log("ANSWERS");
//	console.dir(answers);

	for (var i=0; i<data.clauses.length; i++) {

		new_clauses[used_terms.length+i] = new Object();
		new_clauses[used_terms.length+i].text = add_demographics(data.clauses[i].text);
		new_clauses[used_terms.length+i].section = data.clauses[i].section;
		new_clauses[used_terms.length+i].confidential_annotation = data.clauses[i].confidential_annotation;
		new_clauses[used_terms.length+i].public_annotation = data.clauses[i].public_annotation;
		new_clauses[used_terms.length+i].survivable = data.clauses[i].survivable;
		/*
		console.log("new clauses populate where i = "+i);
		console.log("question  = "+i);
		console.log("answer = "+answers[i]);
		*/
/*************************************************
* DON'T STOP LOOKING AT THE NEXT 2 LINES
**********************************************/
		//new_clauses[used_terms.length+i].source_question = (i+1);  // this is wrong
		//new_clauses[used_terms.length+i].source_answer = answers[i+1];
		new_clauses[used_terms.length+i].source_question = data.clauses[i].question_id;  //This works
		new_clauses[used_terms.length+i].source_answer = data.clauses[i].answer_id;;

	}

	var new_clauses_encoded = JSON.stringify(new_clauses);
//	console.log("NEW CLAUSES");
//	console.dir(new_clauses);

	var title = $("#document_title").val();
	if (title == null || title == "") title = "New Title";
	var name = $("#document_name").val();
	if (name == null || name == "") name = "filename";
	var master_document_id = $("#template_chooser").val();
	if (master_document_id == null || master_document_id == "") master_document_id = "1";
	//console.log("About to send demographic");
	//console.dir(demographics);
	//console.log(JSON.stringify(demographics));
//	alert("About to send demographics");

	ajax_caller('create_new_document', {'demographic_answers':JSON.stringify(demographics), 'data':new_clauses_encoded, 'alternate_text':alternate_text, 'title':title, 'master_document_id':master_document_id},
		create_new_document_callback, 'POST');
}

function create_new_document_callback(data) {
	//alert (JSON.stringify(data, null, 2));
	//console.log("create_new_document_callback");
	//console.log(data);
	if(data.status == "Error") {
		console.error("Server Error");
		console.log(data.message);
	}

	location.href = "load_document?action=Load&document_id=" + data.document_id + "&version=1";
	//$("#crada_document").empty().append(JSON.stringify(data, null, 2));
}


function add_demographics(madlib) {
	//serarch and replace {} with demographic answers
	//Example search {Agency} replace with FDA
    var search_term;
		var replace_term;

		//Check to make sure a madlib is defined
		if (typeof madlib == 'undefined') {
			return;
		}

		if (madlib) {
			$.each(demographics, function(key, val) {
				search_term = "{"+key+"}";
				replace_term = val;
				if(replace_term == "") {
					replace_term = "<strong>["+key+"]</strong>";
				};
				madlib = madlib.replace(new RegExp(search_term, "g"), replace_term);
			});
		}

	return madlib;

}


function get_used_terms(clauses) {
	var used_terms = {};
	for (i=0; i<clauses.length;i++) {
		var terms = clauses[i].terms;
		for (j=0;j<terms.length;j++) {
			used_terms[terms[j]] = true;
		}
	}

	var terms = Object.keys(used_terms);
	terms.sort();

	return terms;
}
//// Helper functions


////  End Code
});

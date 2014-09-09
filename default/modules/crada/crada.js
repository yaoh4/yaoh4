jQuery(function($) {
	
// Global Variables go here
var document_id;
var sections = [];
var current_section_number;
var answers = {};
var definitions;

$(document).ready(function ($) {
	$("#begin_question_button").click(function() {
		$("#instructions").hide ();
		$("#choose_template").fadeIn();
		setup_template_chooser();
	});
	$("#template_chooser_button").click(function() {
		$("#choose_template").hide();
		$("#demographics").fadeIn();
		document_id = parseInt($("#template_chooser").val());
		advance_progress_bar("demographics");
		setup_demographics();
	});
	$("#demographics_button").click(function() {
		$("#demographics").hide();
		$("#questions").fadeIn();
		get_all_definitions(document_id);
		advance_progress_bar("questions");
		setup_sections();
	});	
	$("#questions_button").click(function() {
		store_current_answers(current_section_number);
		if ((current_section_number+1) >= sections.length) {
			$("#progress_bar").hide();
			$("#questions").hide();
			$("#crada_document").fadeIn();
			setup_document();
		} else {
			current_section_number++;
			advance_progress_bar(current_section_number);
			setup_questions_for_section(current_section_number);
		}
	});	
});

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
 $("#demographics").append("Info Goes Here");
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
			.append($("<td class='done' id='initial_step'>Pick an Initial Template</td>"))
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
	ajax_caller('get_questions_for_section', {'document_id':1, 'section':sections[i]}, setup_questions_for_section_callback);
	
}

function setup_questions_for_section_callback(data) {

	var display = $("#question_section");
	display.empty().append("<br />");
	
	var q = data.questions;
	
//	alert (JSON.stringify(q, null, 2));
	var number_of_questions_asked = 0;
	for (i=0;i<q.length; i++) {
		if (q[i].text == 'REQUIRED') answers[q[i].question_id] = 0;
		else {
			number_of_questions_asked++;
			display.append(q[i].text).append("<br />");
			for (j=0;j<q[i].answers.length; j++) {
				display.append($("<INPUT type='radio' name='" + q[i].question_id + "' value='" + j + "'>" + q[i].answers[j] + "</INPUT>"));
			}
			display.append("<br /><br />")
		}
	}
	if (number_of_questions_asked == 0) {
		display.append("<P>This section has no questions.</P>");
	}
}

function store_current_answers(section_number) {
	var question_pane = $("#question_section");
	var str = "";
	$("#question_section").find("INPUT[type=radio]:checked").each(function () {
		str += $(this).attr("name") + ":" + $(this).val() + "\n";
		answers[$(this).attr("name")] = $(this).val();
	})
//	alert (str);
}

function setup_template_chooser() {
	ajax_caller('get_document_templates', {'document_id':1}, setup_template_chooser_callback);
	$("#document_title").focus();
	advance_progress_bar("template");
}

function setup_template_chooser_callback(data) {
	var template_select = $("#template_chooser");
	for (var i=0; i<data.templates.length; i++) {
		var name = data.templates[i].name;
		template_select.append($("<OPTION>").attr('value', data.templates[i].id).append(name));
	}
	
//	alert (JSON.stringify(data, null, 2));	
}

function setup_document() {
//	alert (JSON.stringify(answers, null, 2));
	var answers_encoded = JSON.stringify(answers)
	ajax_caller('get_clauses_from_answers', {'document_id':1, 'answers':answers_encoded}, setup_document_callback, 'POST');
}

function setup_document_callback(data) {
//	alert (JSON.stringify(data, null, 2));
	
	var title = $("#document_title").val();
	$("#crada_document").empty().append($("<H1>").append(title)).append("<hr />");

	$("#crada_document").append("<br /><br />").append($("<H2>").append("Coversheet")).append("<hr />");
	$("#crada_document").append("<br />Coming Soon<br />");

	$("#crada_document").append("<br /><br />").append($("<H2>").append("Definitions")).append("<hr />");
	var used_terms = get_used_terms(data.clauses);
//	alert (JSON.stringify(definitions, null, 2));
//	alert (used_terms[0] + ":" + definitions[used_terms[0]] + "\n" + JSON.stringify(definitions, null, 2))
	for (i=0;i<used_terms.length; i++) {
		var definition = definitions[used_terms[i]];
		$("#crada_document").append("<P><B> " +used_terms[i] + "</B>: " + definition + "</P>");	
	}

	current_section = "";
	for (var i=0; i<data.clauses.length; i++) {
		if (!(data.clauses[i].text == 'silent') ) {
			if (data.clauses[i].section != current_section) {
				$("#crada_document").append("<br /><br />").append($("<H2>").append(data.clauses[i].section)).append("<hr />");
				current_section = data.clauses[i].section;
			}
			$("#crada_document").append($("<P>").append(data.clauses[i].text));	
		} 
	}
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
	return Object.keys(used_terms);
}
//// Helper functions


////  End Code
});

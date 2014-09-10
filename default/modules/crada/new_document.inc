<?php


function crada_new_document() {
	$module_path = drupal_get_path('module', 'crada');
	drupal_add_css('http://ajax.googleapis.com/ajax/libs/jqueryui/1.10.4/themes/start/jquery-ui.css');
	drupal_add_css($module_path . '/crada.css');
	drupal_add_js($module_path . '/crada_helper_functions.js',array('cache' => false, 'preprocess' => false));
	drupal_add_js($module_path . '/crada.js',array('cache' => false, 'preprocess' => false));

	$page_data = "";
	$page_data .= add_progress_bar_div();	
	$page_data .= add_instructions_div();	
	$page_data .= add_choose_template_div();
	$page_data .= add_demographics_div();
	$page_data .= add_questions_div();
	$page_data .= add_document_div();
	return $page_data;
}
function add_progress_bar_div() {
	return "
<div id='progress_bar'> 
<table class='progress_bar'>
<tr>
	<td class='not_done' id='initial_step'>Pick an Initial Template</td>
	<td class='not_done' id='demographic_step'>Demographic Information</td>
	<td class='not_done' id='question_step'>Answer Questions</td>
	<td class='not_done' id='conclusion_step'>Edit the Document</td>
</tr>			
</table>
</div>
			
";
}

function add_instructions_div() {
	return "

<div id='instructions'  > 
	
	<p>We will be building an initial version of your document by pulling from and existing template,
	filling out a simple form, and then answering some questions.</p>
	
	<p> Above you will see a progress bar that shows the steps you need to complete before your document is generated. <p>
	<UL>
	<LI> Stage 1: We need to know which template you would like to use as a base: </LI>
	<LI> Stage 2: You can enter some basic demographic information to tailor the initial document </LI>
	<LI> Stage 3: You will answer a series of questions that will help determine what sections are needed </LI>
	<LI> Stage 4: You can edit or annotate all the clauses generated </LI>
	</UL>			
	<BR/><P>Click on the button to begin: <BUTTON id='begin_question_button'>Begin</BUTTON></P>
</div>
			
			
";
	
}

function add_choose_template_div() {
	return "

<div id='choose_template' style='display:none'> 
<TABLE id='basic_document_info'>
<TR><TD>Document Title: </TD><TD><INPUT id='document_title' size='40' type='text' /></TD></TR>
<TR><TD>Document Filename: </TD><TD><INPUT id='document_title' size='30' type='text' /></TD></TR>	
<TR><TD>Choose your template: </TD><TD><SELECT id='template_chooser'> </SELECT></TD></TR>
<TR><TD></TD><TD><BUTTON id='template_chooser_button'>Advance to Next Section</BUTTON></TD></TR>
</TABLE>
</div>
";
	
}

function add_demographics_div() {
	return "
<div id='demographics' style='display:none'>
	<div id='demographic_questions'></div>
	<BUTTON id='demographics_button'>Advance to Next Section</BUTTON>	
</div>
";

}

function add_questions_div() {
	return "

<div id='questions' style='display:none'>
	<form id='demographic-form'>
	</form>	
	<BUTTON id='questions_button'>Advance to Next Section</BUTTON>
</div>
";

}

function add_document_div() {
	return "

<div id='crada_document' style='display:none'>
</div>
";

}
///  Other sections
function crada_current_document() {
	return '<p>Current Document Functions Go Here</p>';
}

function crada_search_documents() {
	return '<p>Search Documents Functions Go Here</p>';
}

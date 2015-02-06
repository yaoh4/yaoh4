<?php


function crada_new_document() {
	$module_path = drupal_get_path('module', 'crada');
	drupal_add_css('http://ajax.googleapis.com/ajax/libs/jqueryui/1.10.4/themes/start/jquery-ui.css');
	drupal_add_css($module_path . '/crada.css');
	drupal_add_js($module_path . '/crada_helper_functions.js',array('cache' => false, 'preprocess' => false));
	drupal_add_js($module_path . '/new_document.js',array('cache' => false, 'preprocess' => false));
	drupal_add_css('sites/all/libraries/fontawesome/css/font-awesome.css');

	$page_data = "";
	$page_data .= add_progress_bar_div();
	$page_data .= add_spinner_div();
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
	<td class='not_done' id='initial_step'>Select a Base Template</td>
	<td class='not_done' id='demographic_step'>Demographic Information</td>
	<td class='not_done' id='question_step'>Answer Questions</td>
	<td class='not_done' id='conclusion_step'>Edit the Document</td>
</tr>
</table>
</div>

";
}

function add_spinner_div() {
	return "
<div id='spinner' style='display:none;'>
	<p class='second-page-intro'>
		<i class='fa fa-spinner fa-spin fa-3x'></i>
	</p>
</div>
";
}

function add_instructions_div() {
	return "

<div id='instructions'>

	<p><b>Make New Document</b> will create an initial document using information from an existing base template.   After answering questions and filling out a simple form a new document will be created.</p>
	<p> The progress bar above shows the steps to complete before the document is generated. <p>
	<UL>
	<LI> <b>Stage 1</b>: Select a base template which will be used to generate a new document</LI>
	<LI> <b>Stage 2</b>: Enter basic demographic information, alternate text, and subsection options to customize the document</LI>
	<LI> <b>Stage 3</b>: Answer a series of questions which will determine which clauses will be included in document</LI>
	<LI> <b>Stage 4</b>: Edit document, set document permissions, archive versions, and/or download document</LI>
	</UL>
	<BR/><P>Click on the button to begin: <BUTTON id='begin_question_button'>Begin</BUTTON></P>
</div>


";

}

function add_choose_template_div() {
	return "

<div id='choose_template' style='display:none'>
<form id='choose_template_form'>
<TABLE id='basic_document_info'>
<TR>
<TD>
  <label for='document_title'>Document Title:</label>
</TD>
<TD>
  <INPUT id='document_title' size='40' type='text' required placeholder='Document Title'  title='Please enter a Document Title' />
</TD>
</TR>
<TR><TD>

<label for='template_chooser'>
Choose Base Template:
</label>

</TD><TD><SELECT id='template_chooser'> </SELECT></TD></TR>
<TR><TD></TD><TD><BUTTON type='submit' id='template_chooser_button'>Advance to Next Section</BUTTON></TD></TR>
</TABLE>
</form>
</div>
";

}

function add_demographics_div() {
	return "
<div id='demographics' style='display:none'>
	<form id='demographic-form'>
	</form>
</div>
";

}

function add_questions_div() {
	return "

<div id='questions' style='display:none'>
	<DIV id='question_section'> </DIV>
	<BUTTON id='questions_button'>Advance to Next Section</BUTTON>
	<div id='debug-output'></div>
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

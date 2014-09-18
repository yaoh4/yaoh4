<?php

function crada_current_document() {
	$module_path = drupal_get_path('module', 'crada');
	drupal_add_library('system','ui.dialog');
	drupal_add_css($module_path . '/crada.css');
	drupal_add_js($module_path . '/crada_helper_functions.js',array('cache' => false, 'preprocess' => false));
	drupal_add_js($module_path . '/current_document.js',array('cache' => false, 'preprocess' => false));
//  testing annotator
//	drupal_add_css('http://assets.annotateit.org/annotator/v1.1.0/annotator.min.css');
//	drupal_add_js('http://assets.annotateit.org/annotator/v1.1.0/annotator-full.min.js',array('cache' => false, 'preprocess' => false));

	
	$data = "";
	$data .= "<div id='current_document_container'>";
	$data .= add_command_bar();
	$data .= add_load_dialog();
	$data .= "  <DIV id='current_document_content'></DIV>";
	$data .= "  <DIV id='current_annotation_content'></DIV>";
	$data .= "</div>";

	$data .= "<div id='change_answer_container' style='display:none;'>";
	$data .= add_change_answer_bar();
	$data .= "</div>";
	$data .= "<div class='both' style='height:15px;'></div>";
	$data .= "<div id='document_footer' class='both'></div>";

 	return $data;
}

// Command Bar
function add_command_bar() {
	$data = "<DIV id='command_bar' class='document_toolbar'>";
	$data .= "<div id='annotation_selector'>";
	$data .= "<label for='annotation_options'>Annotation: </label>";
	$data .='<select id="annotation_options">
				<option value="confidential">confidential</option>
  				<option value="public">public</option>
  				<option value="both" selected>both</option>
  				<option value="off">off</option>
			</select>';
	$data .= "</div>";
	$data .= add_button("change_answer_button", "Change Answer", "Change an answer for this document", "pull-right");
	$data .= add_button("load_button", "Load New Document");
	$data .= add_button("save_button", "Save Document");
	$data .= "</DIV>";
	
	return $data;
}

// change answer Bar
function add_change_answer_bar() {
	$data = "<DIV id='change_answer_bar' class='document_toolbar'>";
	$data .= add_button("back_button_answer", "Back", "Return to document", "pull-right");
	$data .= "</DIV>";
	
	return $data;
}

function add_button($id, $name, $title="", $class="") {
	$button = "";
	if($title == ""){
		$button = "<BUTTON class='button' id='$id'>$name</BUTTON>";
	} else {
		$button = "<BUTTON class='button' id='$id' title='$title'>$name</BUTTON>";
	}
	if($class != ""){
		$button = "<BUTTON class='button' id='$id' title='$title' class='$class'>$name</BUTTON>";
	}
	return $button;
}

// Dialogs

function add_load_dialog() {
	$data = "<DIV id='load_dialog'>";
	$data .= "<TABLE>";
	$data .= "<TR><TD>Document:</TD><TD> <SELECT id='document_select'>";
	$data .= "</SELECT></TD></TR>";
	$data .= "<TR><TD>Version:</TD><TD><SELECT id='document_version'>";
	$data .= "<OPTION> Current </OPTION>";
	$data .= "<OPTION> v1 </OPTION>";
	$data .= "</SELECT></TD></TR>";
	$data .= "<TR><TD>&nbsp;</TD><TD>";
	$data .= add_button("select_document_button", "Select");
	$data .= "</TD></TR>";
	$data .= "</TABLE>";
	$data .= "</DIV>";
	return $data;
}

<?php

function crada_current_document() {
	$module_path = drupal_get_path('module', 'crada');
	drupal_add_library('system','ui.dialog');
	drupal_add_css($module_path . '/crada.css');
	drupal_add_js($module_path . '/crada_helper_functions.js',array('cache' => false, 'preprocess' => false));
	drupal_add_js($module_path . '/current_document.js',array('cache' => false, 'preprocess' => false));
	
	$data = "";
	$data .= add_command_bar();
	$data .= add_load_dialog();
	$data .= "<DIV id='current_document_content'></DIV>";
	
 	return $data;
}


// Command Bar
function add_command_bar() {
	$data = "<DIV id='command_bar'>";
	$data .= add_button("load_button", "Load New Document");
	$data .= add_button("save_button", "Save Document");
	$data .= "</DIV>";
	
	return $data;
}

function add_button($id, $name) {
	return "<BUTTON class='button' id='$id'>$name</BUTTON>";
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

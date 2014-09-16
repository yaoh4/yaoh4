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
	$data .= add_command_bar();
//  Annotator test	
//	$data .= add_annotator_div_test();

	$data .= add_load_dialog();
	$data .= "<DIV id='current_document_content'></DIV>";
	$data .= "<DIV id='current_annotation_content'></DIV>";
	
 	return $data;
}

function add_annotator_div_test() {
	return "
	<h3>annotator</h3>
	<div id='annotator' style='height:200px;border:1px solid red;'>
not statutory but good business practice -- ask OGC if this is required by statute and if it needs to say \"carried out by Parties\".

	</div>
	<h3>annotator1</h3>
	<div id='annotator1' style='height:200px;border:1px solid blue;'>
		Public vs Privite Annotations
	</div>
	<h3>annotator2</h3>
	<div id='annotator2' style='height:200px;border:1px solid green;'>
		Statutory requirement.  ICD must ensure that the government retains specific rights; i.e., ICD requires the Collaborator to grant licenses for specific health and safety purposes or public use regulations.  See 15USC§3710a(b)(1)(B)
	</div>

	";
}

// Command Bar
function add_command_bar() {
	$data = "<DIV id='command_bar'>";
	$data .= "<div id='annotation_selector'>";
	$data .= "<label for='annotation_options'>Annotation: </label>";
	$data .='<select id="annotation_options">
				<option value="confidential">confidential</option>
  				<option value="public">public</option>
  				<option value="both" selected>both</option>
  				<option value="off">off</option>
			</select>';
	$data .= "</div>";
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

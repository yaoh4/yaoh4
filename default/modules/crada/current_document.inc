<?php

require_once 'crada_utilities.inc';


if(function_exists('openssl_encrypt')) {
  crada_log("BTW, PHP opensslq_encrypt function exists.  Yes... (current_document.inc)");
}

function crada_current_document() {

	crada_include();
	$module_path = drupal_get_path('module', 'crada');
	drupal_add_library('system','ui.dialog');
	drupal_add_css($module_path . '/crada.css');
	drupal_add_js($module_path . '/crada_helper_functions.js', array('cache' => false, 'preprocess' => false));
	drupal_add_js($module_path . '/current_document.js', array('cache' => true, 'preprocess' => false));
	drupal_add_css('sites/all/libraries/fontawesome/css/font-awesome.css');
	drupal_add_js('sites/all/modules/ckeditor/ckeditor.js', array('cache' => false, 'preprocess' => false));
		//drupal_add_js('sites/default/libraries/gibberish-aes/gibberish.aes.min.js', array('cache' => true, 'preprocess' => false));

	global $user;

	if (!isset($user->uid)) {
		//user is not logged in
		crada_log("User is not logged in");
		$user_info = array("uid"=>0, "name"=>"unknown");
	} else {
		$user_info = array("id"=> $user->uid,"name"=> $user->name);
		crada_log("user->uid = ". $user->uid);
		crada_log("user->name = ". $user->name);
	}

	crada_cookie_save('user', $user_info);
	//dpm($module_path, "Module Path");
	$data = "";
	$data .= "<div id='current_document_container'>";
	$data .= add_command_bar();
	$data .= add_load_dialog();
	$data .= "  <DIV id='current_document_content'>";
	$data .= '    <div></div><p class="second-page-intro"><i class="fa fa-spinner fa-spin fa-2x"></i></p>';
	$data .= "  </DIV>";
	$data .= "  <DIV id='current_annotation_content'></DIV>";
	$data .= "</div>";

	$data .= "<div id='second_page_container' style='display:none;'>";
	$data .= add_change_answer_bar();
	$data .= "<div id='second_page'></div>";
	$data .= "</div>";
	$data .= "<div class='both' style='height:15px;'></div>";
	$data .= "<div id='document_footer' class='both'></div>";

 	return $data;
}
/*
function crada_log($data, $debug_level=0) {

	$filename = "/tmp/crada.log";
	$timestamp = date("Y-m-d H:i:s", time());
	$output = $timestamp . " " . $data ;

	if($debug_level > 0 ) {
		file_put_contents($filename,  "*******************************************" . PHP_EOL, FILE_USE_INCLUDE_PATH | FILE_APPEND );
	}

	file_put_contents($filename,  $output . PHP_EOL, FILE_USE_INCLUDE_PATH | FILE_APPEND );

	if($debug_level > 0 ) {
		foreach (debug_backtrace() as $key => $value) {
			//alert( index + ": " + value );
			file_put_contents($filename,  	$key . ": " . json_encode($value).PHP_EOL , FILE_USE_INCLUDE_PATH | FILE_APPEND );
		}
	}

}
*/
function add_command_bar() {

	$data = "<DIV id='command_bar' class='document_toolbar'>";

	$data .= "<div id='annotation_selector'>";

	$data .= "<label for='annotation_options'>Annotation: </label>";
	$data .='<select id="annotation_options">
				<option value="confidential">confidential</option>
  				<option value="public">public</option>
  				<option value="both">both</option>
  				<option value="off">off</option>
			</select>';
	$data .= "</div>";
	$data .= add_button("load_button", "Load", "Load a new document", "fa fa-file");
	$data .= add_button("archive_version_button", "Archive", "Archive Version", "fa fa-archive");
	$data .= add_button("change_answer_button", "Change Answer", "Change an answer for this document", "fa fa-edit");
	$data .= add_button("change_permission_button", "Set Permission", "Set permission for this document", "fa  fa-key");
//	$data .= '<i class="fa fa-cloud-download fa-2x"></i>';
//	$data .= '<i class="fa fa-file fa-2x"></i>';
	$data .= '<span class="download-button-container">';
//	$data .= ' <button id="download-word" title="Download Word"><i class="fa fa-file-word-o"></i></button>';
	$data .= add_button("download-word", "", "Download Word", "fa fa-file-word-o");
	$data .= '</span>';
	$data .= '<span class="download-button-container">';
	$data .= '<button id="download-pdf"  title="Download PDF"><i class="fa fa-file-pdf-o"></i></button>';
	$data .= '</span>';
	$data .= "</DIV>";

	return $data;
}

// change answer Bar
function add_change_answer_bar() {
	$data = "<DIV id='change_answer_bar' class='document_toolbar'>";
	$data .= add_button("back_button_answer", "Back", "Return to document", "fa fa-mail-reply");
	$data .= "</DIV>";

	return $data;
}

function add_button($id, $name, $title="", $class="") {
	$button = "";
	if($title == ""){
		$button = "<BUTTON class='button' id='$id' class='fa fa-home'>$name</BUTTON>";
	} else {
		$button = "<BUTTON class='button' id='$id' title='$title'>$name</BUTTON>";
	}
	if($class != ""){
		$button = "<BUTTON class='button' id='$id' title='$title'><i class='$class'></i>&nbsp;$name</BUTTON>";
	}
	return $button;
}

// Dialogs
function add_load_dialog() {

	$data = "<DIV id='load_dialog'>";
  $data .= '<div id="load_dialog_spinner"><p class="second-page-intro"><i class="fa fa-spinner fa-spin fa-2x"></i></p></div>';
	$data .= "<TABLE id='load_dialog_select' style='display:none;'>";
	$data .= "<TR><TD>";
	$data .= "<label for='document_select'>Document:</label>";
	$data .= "</TD><TD><SELECT id='document_select'>";
	$data .= "</SELECT></TD></TR>";
	$data .= "<TR><TD>";
	$data .= "<label for='document_version'>Version:</label>";
	$data .= "</TD><TD><SELECT id='document_version'>";
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

function crada_cookie_save($key, array $values) {
  foreach ($values as $field => $value) {
    // Set cookie for 365 days.
    setrawcookie('Drupal.visitor.' . $key . '.' . $field, rawurlencode($value), REQUEST_TIME + 31536000, '/');
  }
}

function crada_cookie_delete($key, $cookie_name) {
	setrawcookie('Drupal.visitor.' . $key . '.' . $cookie_name, '', REQUEST_TIME - 3600, '/');
}

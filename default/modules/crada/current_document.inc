<?php

function crada_current_document() {
	$module_path = drupal_get_path('module', 'crada');
	drupal_add_library('system','ui.dialog');
	drupal_add_css($module_path . '/crada.css');
	drupal_add_js($module_path . '/crada_helper_functions.js',array('cache' => false, 'preprocess' => false));
	drupal_add_js($module_path . '/current_document.js',array('cache' => false, 'preprocess' => false));

	//$data = "Hello00";
	//$filename = "/tmp/crada.log";
	//dpm($module_path, "Module Path");
	//dpm($filename, "Attempting to create this file");
	crada_log("Attempting to create this file.");
	crada_log("Now this is cool.");


	//drupal_add_css("sites/all/modules/libraries/bootstrap/css/bootstrap.min.css", 'theme', 'all', FALSE);
	//drupal_add_js("sites/all/modules/libraries/bootstrap/js/bootstrap.min.js",array('cache' => false, 'preprocess' => false));
    //drupal_add_css('sites/all/modules/libraries/fontawesome/css/font-awesome.css');

	// C:\a\htdocs\drupal\sites\all\modules\libraries\fontawesome\css
	//drupal_set_message("Fontawesome: ".json_encode($font_awesome));
	//drupal_set_message("Base_path(): ".base_path());
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
function crada_log($data) {

	$filename = "/tmp/crada.log";
	$timestamp = date("Y-m-d H:i:s", time());
	$output = $timestamp . " " . $data ;

	//file_put_contents($filename, $timestamp, FILE_USE_INCLUDE_PATH | FILE_APPEND );
	file_put_contents($filename,  "*******************************************" . PHP_EOL, FILE_USE_INCLUDE_PATH | FILE_APPEND );
	file_put_contents($filename,  $output . PHP_EOL, FILE_USE_INCLUDE_PATH | FILE_APPEND );

	//$.each(debug_backtrace(), function( index, value ) {
	foreach (debug_backtrace() as $key => $value) {
		//alert( index + ": " + value );
		file_put_contents($filename,  	$key . ": " . json_encode($value).PHP_EOL , FILE_USE_INCLUDE_PATH | FILE_APPEND );
	}


}

// Command Bar
function add_command_bar() {
	//drupal_set_message(t('Fontawesome location'));
	//$hello = libraries_get_path('fontawesome');
	//$hold = drupal_get_path('library', 'fontawesome');
	//drupal_set_message("Hold: ".json_encode($hold));

	$data = "<DIV id='command_bar' class='document_toolbar'>";
/*
	$data .= '<div class="btn-group open">
  <a class="btn btn-primary" href="#"><i class="fa fa-user fa-fw"></i> User</a>
  <a class="btn btn-primary dropdown-toggle" data-toggle="dropdown" href="#">
    <span class="fa fa-caret-down"></span></a>
  <ul class="dropdown-menu">
    <li><a href="#"><i class="fa fa-pencil fa-fw"></i> Edit</a></li>
    <li><a href="#"><i class="fa fa-trash-o fa-fw"></i> Delete</a></li>
    <li><a href="#"><i class="fa fa-ban fa-fw"></i> Ban</a></li>
    <li class="divider"></li>
    <li><a href="#"><i class="i"></i> Make admin</a></li>
  </ul>
</div>';
	$data .= '<a class="btn btn-large btn-danger" href="#">
  <i class="icon-flag icon-2x pull-left"></i> Font Awesome<br>Version 3.2.1</a>';
  */

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
		$button = "<BUTTON class='button' id='$id' class='fa fa-home'>$name</BUTTON>";
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

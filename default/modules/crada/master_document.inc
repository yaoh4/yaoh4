<?php 


function master_document_form($form, &$form_state) {
	$form['description'] = array(
			'#type' => 'item',
			'#title' => t('Master Document Creator'),
	);
	
	$form['document_title'] = array(
			'#type' => 'textfield',
			'#title' => 'Document Title',
			'#required' => TRUE,
			'#description' => 'Please enter a document title',
			'#size' => 100,
	);
	
	$form['document_name'] = array(
			'#type' => 'textfield',
			'#title' => 'Document Short Name',
			'#required' => TRUE,
			'#description' => 'Please enter the reference-name for this document',
			'#size' => 20,

	);
	
	$form['file'] = array(
			'#type' => 'file',
			'#title' => 'Filename',
			'#description' => 'Choose a File',
	);

	$form['submit'] = array(
			'#type' => 'submit',
			'#value' => 'Create New Master Document',


	);
	return $form;
}

function master_document_form_validate($form, &$form_state) {
	$validators = array(
			'file_validate_extensions' => array('csv txt xls xlsx'), // does not work for user 1
			'file_validate_size' => array(1000000, 0),
	);
	$file = file_save_upload('file',$validators);

	if ($file) {
		// Make it a permanent file so it doesn't get deleted by cron.
		$file->status = 1;
		file_save($file);

		// Save file name for submit handler.
		$form_state['storage']['filename'] = $file->uri;

	} else {
		form_set_error('file',t('The file could not be uploaded'));
	}
}

function master_document_form_submit ($form, &$form_state) {
	global $filename, $shorthand, $title;
	
	drupal_set_message(t('Loading in the form: '));
	$filename = $form_state['storage']['filename'];
	
	$title = trim($form_state['values']['document_title']);
	$shorthand = trim($form_state['values']['document_name']);
	
	load_excel_file();
}

function load_excel_file() {
	global $filename, $xls_reader;
	$library = libraries_load('PHPExcel');
	if (empty($library['loaded'])) {
		drupal_set_message( "Failed to Load PHPExcel Library" );
		return;
	}
		
	drupal_set_message(t("Filename is: $filename"));

	$filetype = PHPExcel_IOFactory::identify($filename);
	$xls_reader = PHPExcel_IOFactory::createReader($filetype);
	$xls_reader->setReadDataOnly(TRUE);
		
	$transaction = db_transaction();
	try {
		$document_id = db_insert_crada_document();
		load_coversheet();
		load_definitions();
		load_demographics();
		load_sections();
	} catch (Exception $e) {
		$transaction->rollback();
		watchdog_exception('crada', $e, $e->getMessage());
		drupal_set_message("Failed on insert crada_document: ". $e->getMessage(), "error");
	}
		
}

function load_coversheet() {
	global $xls_reader, $filename;
	drupal_set_message("Coversheet Loading Coming Soon");
}

function load_definitions() {
	global $xls_reader, $filename;
	
	drupal_set_message("Definitions Loading Coming Now");
	$xls_reader->setLoadSheetsOnly("Definitions");
	$xls_data = $xls_reader->load($filename);
	$array_data = $xls_data->getActiveSheet()->toArray(null,true,true,true);
	foreach ($array_data as $line) {
		drupal_set_message($line["A"] . ":" . $line["B"]);
	}
}

function load_demographics() {
	global $xls_reader, $filename;
	drupal_set_message("Demographics Loading Comming Soon");
}

function load_sections() {
	global $xls_reader, $filename;

	global $clause_id;
	$clause_id = 0;
	
	
	drupal_set_message("Sections Loading Comming Soon");
	//  Pull the names of the worksheets before even loading the file
	$sheet_names = $xls_reader->listWorksheetNames($filename);
	foreach($sheet_names as $i => $sheet_name) {
		if (	$sheet_name != 'Coversheet' &&
				$sheet_name != 'Definitions' &&
				$sheet_name != 'Demographics' &&
				$sheet_name != 'Signature') {
			load_specific_section($i, $sheet_name);
		}
	}
}

function load_specific_section($i, $section_name) {
	global $xls_reader, $filename;
	
	$xls_reader->setLoadSheetsOnly($section_name);
	$xls_data = $xls_reader->load($filename);
	drupal_set_message("Loading Section: " . $section_name);
	
	$array_data = $xls_data->getActiveSheet()->toArray(null,true,true,true);

	// There is a lot of data that reflects information from a previous row to get the right values for the current row
	// Consequently there are a lot of global variables that hold the state as we pass from one line to the next
	// Most will be in the load_line function, but a couple are needed here.
	global $clause_id, $row_in_section;
	
	// Row 0 is the header, we will treat it special
	$row_in_section = 0;
	foreach ($array_data as $line) {
		if ($clause_id > 0) {
			load_line($section_name, $line);
		}
		$clause_id++;
		$row_in_section++;
	}
}

// This is where the meat of the code lives.  This function loads a new question and all the actions from the answer
function load_line($section, $line) {
	drupal_set_message("LINE: " . json_encode($line));
	
	
}

// Pure Database Functions

function db_insert_crada_document() {
	global $title, $shorthand;
	$document_id = db_insert('crada_document')->fields(array(
			'document_name' => $shorthand,
			'title' => $title,
			'is_master' => 1,
	))->execute();
	drupal_set_message("Successfully inserted into crada_document");
	return $document_id;
}

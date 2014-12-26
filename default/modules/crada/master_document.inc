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
	global $filename, $xls_reader, $document_id;
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
	
	$xls_reader->setLoadSheetsOnly("Definitions");
	$xls_data = $xls_reader->load($filename);
	$array_data = $xls_data->getActiveSheet()->toArray(null,true,true,true);
	foreach ($array_data as $line) {
		if ($line["A"] != 'Term' && $line["A"] != 'Defined Term' ) db_insert_definition($line["A"], $line["B"]);
	}
}

function load_demographics() {
	global $xls_reader, $filename;
	$section_name = "Demographics";
	drupal_set_message("$section_name Loading");
	$xls_reader->setLoadSheetsOnly($section_name);
	$xls_data = $xls_reader->load($filename);
	$array_data = $xls_data->getActiveSheet()->toArray(null,true,true,true);

	$row_in_section = 0;
	$first_row = true;
	$previous_variable = "";
	foreach ($array_data as $line) {
		if ($first_row) {
			drupal_set_message(implode(' | ', $line));
			$first_row = false;
		} else {
			$question = $line["A"];
			$type = $line["B"];
			$option = $line["C"];
			$variable = $line["D"];

			drupal_set_message("question:$question, type:$type, option:$option, variable:$variable");
			if($variable != $previous_variable) {
				db_insert_crada_demographics($variable, $question, $type);
			}
			if(strtoupper($type) == strtoupper("pull down")) {
				db_insert_crada_demographics_pulldown_options($variable, $option);
			}
			$previous_variable = $variable;
		}
		$row_in_section++;
	}
}

function load_sections() {
	global $xls_reader, $filename;

	global $clause_id;
	$clause_id = 0;
	
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
	
	show_definitions();
}

function show_definitions() {
	global $all_defined_terms;
	$keys = array_keys($all_defined_terms);
	
	for ($i=0; $i<count($keys); $i++) {
		drupal_set_message("Terms are: $keys[$i]");
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
	// We need to find out which columns are: 'Defined Terms', 'Confidential Annotation', and 'Public Annotation'
	
	$row_in_section = 0;
	$first_row = true;
	foreach ($array_data as $line) {
		if ($first_row) {
			drupal_set_message("-------------------------------------------------------------------");
			determine_columns($line);
			$first_row = false;
		} else {
			load_line($section_name, $line);
		}
		$clause_id++;
		$row_in_section++;
	}
}
function determine_columns($line) {
	global $defined_terms_column, $confidential_annotation_column, $public_annotation_column, $alternate_text_columns;
	
	// This limits the number of alternate texts to about 30
 
	$column_names = preg_split("/,/", 
		"A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,AA,AB,AC,AD,AE,AF,AG,AH,AI,AJ,AK,AL,AM,AN,AO");
	
	$i = 0;
	foreach ($column_names as $c) {
		if (isset($line[$c])) {
			if ($line[$c] == "Defined Terms") $defined_terms_column = $c;
			else if ($line[$c] == "Confidential Annotation") $confidential_annotation_column = $c;
			else if ($line[$c] == "Public Annotation") $public_annotation_column = $c;
			else if ($c != "A" && $c != "B" && $c != "C" && $c != "D" && $c != "E" && $line[$c] != "Survivablity") {
				// All these columns make up the alternate text
				$alternate_text_columns[$i]['name'] = $line[$c];
				$alternate_text_columns[$i]['column_id'] = $c;
				$i++;
			}
		} 
	}
	
	drupal_set_message("Alternate Text:" . json_encode($alternate_text_columns));
	drupal_set_message("Defined Terms Column: $defined_terms_column");
	drupal_set_message("Confidential Annotation Column: $confidential_annotation_column");
	drupal_set_message("Public Annotation Column: $public_annotation_column");
}


// This is where the meat of the code lives.  This function loads a new question and all the actions from the answer
function load_line($section, $line) {
	global $document_id, $current_subsection, $current_question, $current_answer,
		$question_number_for_subsection, $answer_number_for_question, $user_name, $question_id,
        $clause_id, $row_in_section,
		$defined_terms_column, $confidential_annotation_column, $public_annotation_column, $alternate_text_columns;
	
//	drupal_set_message("LINE: " . json_encode($line));
	
	$subsection = $line["A"];
	if ($subsection == null) $subsection = "";
	// Subsection stuff below is old and may not be needed
	$new_subsection = false; $new_question = false; $new_answer = false;
	if ($line["A"] !=  $current_subsection && $line["A"] != "") {
		$current_subsection = $line["A"];
		$new_subsection = true;
		$question_number_for_subsection = 0;
	}
	if ($line["B"] !=  $current_question && $line["B"] != "") {
		$current_question = $line["B"];
		$new_question = true;
		$question_number_for_subsection++;
		$answer_number_for_question = -1; // Next line increments it to 0;
	}
	if ($line["C"] == 'REQUIRED') {
		$current_answer = $line["C"];
		$new_answer = true;
		$answer_number_for_question=0;
	} else if ($line["C"] !=  $current_answer) {
		$current_answer = $line["C"];
		$new_answer = true;
		$answer_number_for_question++;
	}
	// We need to deal with the followon question later (line['D'])
	$default_text = $line["E"]; //This can be hard coded to letter E

	
	$alternate_text_array = array();
	for ($i=0; $i<count($alternate_text_columns);$i++) {
		drupal_set_message("### Alternate Text[".$i."]:" . json_encode($alternate_text_columns[$i]));
		$column_id = $alternate_text_columns[$i]['column_id'];
		if ($column_id && $column_id != "") {
			$alternate_text['text'] = $line[$column_id];
			if ($alternate_text['text'] != "") {
				$alternate_text['column_id'] = $column_id;
				$alternate_text['name'] = $alternate_text_columns[$i]['name'];
				
				array_push($alternate_text_array, $alternate_text);
				drupal_set_message("* Alternate Text[" . $clause_id . "]:[" . json_encode($alternate_text) ."]". json_encode($alternate_text_columns[$i]) ."|");
			}
		}
	}
	
	$default_confidential_annotation = $line[$confidential_annotation_column]; 
	$default_public_annotation = $line[$public_annotation_column];

	$defined_terms = $line[$defined_terms_column];
	
	
	// Ignore the header line
	if ($current_question == 'Question') return;
	
	// We catch the exception above this
	
	if ($new_question || $current_answer == "REQUIRED") $question_id = db_insert_crada_question($section, $subsection, $current_question);
	drupal_set_message(">>>($question_id):$current_question:A($answer_number_for_question): $current_answer");
	if ($new_answer || $current_answer == 'REQUIRED') {
		db_insert_crada_answer($question_id, $answer_number_for_question, $current_answer);
		$clause_group_id = db_insert_crada_clause_group($section, $current_question, $current_answer);
		db_insert_crada_valid_clause_group_by_answer($question_id, $answer_number_for_question, $clause_group_id);
		db_insert_crada_document_element($clause_id, $section, $subsection, "default", $default_text, $default_confidential_annotation, $default_public_annotation);
		foreach ($alternate_text_array as $a) {
			if($a['text'] && $a['text'] != "") {
				drupal_set_message("******* Alternate Text[" . $clause_id . "," . $a['name'] . "]:[" . $a['text'] ."]");
				db_insert_crada_document_element($clause_id, $section, $subsection, $a['name'], $a['text'], $default_confidential_annotation, $default_public_annotation);
			}
		}
		db_insert_crada_clause_group_to_clause ($clause_group_id, $clause_id);
	}
	if ($defined_terms) {
		load_all_defined_terms($clause_id, $defined_terms);
	}
}


function load_all_defined_terms($clause_id, $defined_terms) {
	global $document_id, $all_defined_terms;
	
	$list_of_terms = preg_split("/\r?\n/", $defined_terms);
	foreach ($list_of_terms as $single_term) {
		$all_defined_terms[$single_term] = true;
		db_insert_crada_definition_element_crosslink($clause_id, $single_term);
		drupal_set_message("Defined Term for Clause($clause_id): $single_term");
	}
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


function db_insert_crada_demographics($variable, $question, $type) {
	global $document_id;
	db_insert('crada_demographics')->fields(array(
		'document_id' => $document_id,
		'variable' => $variable,
		'question' => $question,
		'type' => $type
	))->execute();
	drupal_set_message("[$document_id:$variable]:$question:$type" );
}

function db_insert_crada_demographics_pulldown_options($variable, $pulldown_option) {
	global $document_id;
	drupal_set_message("Insert Crata Demographics Pulldown Options");
	drupal_set_message("[$document_id:$variable]:$pulldown_option");
	db_insert('crada_demographics_pulldown_options')->fields(array(
		'document_id' => $document_id,
		'variable' => $variable,
		'pulldown_option' => $pulldown_option
	))->execute();
}

function db_insert_definition($term, $definition) {
	global $document_id;
	db_insert('crada_definition')->fields(array(
		'document_id' => $document_id,
		'term' => $term,
		'definition' => $definition
	))->execute();
	drupal_set_message("[$term]:$definition" );
}

function db_insert_crada_definition_element_crosslink($clause_id, $term) {
	global $document_id;
	db_insert('crada_definition_element_crosslink')->fields(array(
		'document_id' => $document_id,
		'clause_id' => $clause_id,
		'term' => $term
	))->execute();
	drupal_set_message("[$document_id:$clause_id]:$term" );
}

function db_insert_crada_question($section, $subsection, $current_question) {
	global $document_id;
	$question_id = db_insert('crada_question')->fields(array(
		'document_id' => $document_id,
		'question_id' => 0,
		'section' => $section,
		'subsection' => $subsection,
		'predecessor' => 0,
		'question_text' => $current_question
	))->execute();
	drupal_set_message("Adding Question($question_id): $current_question");
	return $question_id;
}

function db_insert_crada_answer($question_id, $answer_number, $current_answer) {
	global $document_id;
	if ($current_answer == null) $current_answer = "";
	db_insert('crada_answer')->fields(array(
		'document_id' => $document_id,
		'question_id' => $question_id,
		'answer_id'   => $answer_number,
		'answer_text' => $current_answer
	))->execute();
	drupal_set_message("Adding Answer($question_id:$answer_number): $current_answer");
}

function db_insert_crada_clause_group($section, $current_question, $current_answer) {
	$clause_group_id = db_insert('crada_clause_group')->fields(array(
		'section' => $section,
		'description' => "$current_question:$current_answer"
	))->execute();
	drupal_set_message("Adding Clause Group($clause_group_id) to $section");
	return $clause_group_id;
}

function db_insert_crada_valid_clause_group_by_answer($question_id, $answer_number, $clause_group_id) {
	global $document_id;
	db_insert('crada_valid_clause_group_by_answer')->fields(array(
		'document_id' => $document_id,
		'question_id' => $question_id,
		'answer_id' => $answer_number,
		'clause_group_id' => $clause_group_id
	))->execute();
	drupal_set_message("Adding Valid Clause Group to Answer($question_id:$answer_number): $clause_group_id");
}

function db_insert_crada_document_element($clause_id, $section, $subsection, $alternate_type, $text, $confidential_annotation, $public_annotation) {
	global $user, $document_id;
	
	db_insert('crada_document_element')->fields(array(
		'document_element_id' => $clause_id,
		'document_version' => 0,
		'document_id' => $document_id,
		'section' => $section,
		'subsection' => $subsection,
		'location' => $clause_id,
		'alternate_text_type' => $alternate_type,
		'document_element_text' => $text,
		'confidential_annotation' => $confidential_annotation,
		'public_annotation' => $public_annotation,
		'survivable' => 0,
		'required' => 0,
		'updated_by' => $user->name,
		'updated_date' => date("Y-m-d H:i:s", time())
	))->execute();
	
	drupal_set_message("Adding Clause($document_id:$clause_id):($alternate_type) $text ($confidential_annotation)($public_annotation)");
}

function db_insert_crada_clause_group_to_clause ($clause_group_id, $clause_id) {
	global $document_id;
	db_insert('crada_clause_group_to_clause')->fields(array(
		'clause_group_id' => $clause_group_id,
		'document_id' => $document_id,
		'version' => 0,
		'clause_id' => $clause_id
	))->execute();
	drupal_set_message("Adding Clause Group to Clause ($clause_group_id): $clause_id");
}

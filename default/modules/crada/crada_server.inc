<?php

function crada_server() {
	$action = $_REQUEST['action'];

	try {
		// Functions for New Document
		if ($action == 'get_title') $output = db_get_title();
		else if ($action == 'get_document_templates') $output = db_get_document_templates();
		else if ($action == 'get_questions_for_section') $output = db_get_questions_for_section();
		else if ($action == 'get_section_list') $output = db_get_section_list();
		else if ($action == 'get_section') $output = db_get_section();
		else if ($action == 'get_clauses_from_answers') $output = get_clauses_from_answers();
		else if ($action == 'get_all_definitions') $output = db_get_all_definitions();
		else if ($action == 'get_demographics') $output = db_get_demographics();
		else if ($action == 'get_all_documents') $output = db_get_all_documents();
		else if ($action == 'create_new_document') $output = create_new_document();
		else if ($action == 'get_full_document') $output = get_full_document();
		else if ($action == 'get_answers') $output = get_answers();

		else if ($action == 'echo') $output = echo_data();
		
	} catch (Exception $e) {
		$output['status'] = 'Error';
		$output['message'] =$e->getMessage();
	}

	if (isset($output))
		echo json_encode($output, JSON_PRETTY_PRINT);
	else {
		$output["status"] = 'Error';
		$output["message"] = 'Unknown Function Called';
		echo json_encode($output, JSON_PRETTY_PRINT);
	}
	
/*	
	else if ($action == 'get_annotations') {
		$list = db_get_annotations();
		$output["status"] = $status;
		$output["name"] = $section;
		$output["elements"] = $list;
	} else if ($action == 'get_clauses_for_q_and_a') {
		$list = get_valid_clauses_by_question_answer_and_alternate_text_type();
		$output["status"] = $status;
		$output["clauses"] = $list;
	} else if ($action == 'load_document') {
		$list = db_get_all_document_elements();
		$output["status"] = $status;
		$output["elements"] = $list;
	} else if ($action == 'delete_document_element') {
		db_delete_document_element();
		$output["status"] = $status;
	} else if ($action == 'edit_document_element') {
		db_edit_document_element();
		$output["status"] = $status;
	} else if ($action == 'edit_annotation') {
		db_edit_annotation();
		$output["status"] = $status;
	} else if ($action == 'get_documents') {
		$output["documents"] = get_all_documents();
		$output["status"] = $status;
	} else if ($action == 'get_versions') {
		$output["versions"] = get_all_versions();
		$output["status"] = $status;
	} else if ($action == 'get_clause_groups') {
		$output["groups"] = get_clause_groups();
		$output["status"] = $status;
	} else if ($action == 'get_question') {
		$output["question"] = get_question();
		$output["status"] = $status;
	} else if ($action == 'get_valid_clause_groups') {
		$output["valid_group_ids"] = get_valid_clause_groups_by_answer();
		$output["status"] = $status;
	} else if ($action == 'get_clauses_from_clause_group') {
		$output["clauses"] = get_clauses_from_clause_group();
		$output["status"] = $status;
	} else if ($action == 'get_all_document_elements_for_group') {
		$list = get_all_document_elements_for_group();
		$output["status"] = $status;
		$output["elements"] = $list;
	} elseif  ($action = 'save_document') {
		db_save_document();
		$output["status"] = $status;
	}
*/	
	
	
}
function get_answers() {
	//$document_id = intval($_REQUEST['document_id']);
	//
	//Hard coded for now.  Retrieve master_document_id from crada_question
	//
	//Look up maste

	$document_id = $_REQUEST['document_id'];
	$result = db_select('crada_document', 'cd')
		->fields('cd')
		->condition('document_id', $document_id, '=')
		->execute();
	$row = $result->fetchObject();
	$master_document_id = $row->master_document_id;

	$result = db_select('crada_question', 'cq')
		->fields('cq')
		->condition('document_id', $master_document_id, '=')
		->condition('question_text', 'REQUIRED', '<>')
		->execute();
	
	$list = array();
	for ($count = 0; $row = $result->fetchObject(); $count++) {

		$question["question_id"] = $row->question_id;
		$question["section"] = $row->section;
		$question["question_text"] = $row->question_text;
		$question_id = $row->question_id;

		$r3 = db_select('crada_document_element', 'cde')
				->fields('cde')
				->condition('document_id', $document_id, '=')
				->condition('source_question', $question["question_id"], '=')
				->execute();
		$row3 = $r3->fetchObject();
		$question["answer"] = $row3->source_answer;

		$r2 = db_select('crada_answer', 'c')
			->fields('c')
			->condition('document_id', $master_document_id, '=')
			->condition('question_id', $question_id, '=')
			->execute();
		$question['answers'] = array();  //resetting this on each iteration
		for ($c2 = 0; $row2 = $r2->fetchObject(); $c2++) {
			$question['answers'][$c2] = $row2->answer_text;
		}
		array_push($list, $question);
	}
	$output = array();
    $output["status"] = "Ok";
    $output["questions"] = $list;
	return $output;
}
function get_clauses_from_answers() {
	$document_id = $_REQUEST['document_id'];
	$answers = json_decode($_REQUEST['answers']);
	// Save answers in crada_document

	if (isset($_REQUEST['alternate_text'])) $alternate_text = $_REQUEST['alternate_text'];
	else $alternate_text = 'default';
	if (isset($_REQUEST['version'])) $version = intval($_REQUEST['version']);
	else $version = 0;
	
	$clause_output = array();
	$count=0;
	foreach ($answers as $question => $answer) {
		$count++;
		$q = intval($question);
		$a = intval($answer);
		
		$cg = db_get_clause_group($document_id, $q, $a);
		$clauses = db_get_clause_ids_from_clause_group($document_id, $cg, $version);
		foreach ($clauses as $clause) {
			$full_clause = db_get_clause_information($document_id, $version, $clause);
			// put q&a
			$full_clause['terms'] = db_get_definitions_for_clause($document_id, $clause);
			array_push($clause_output, $full_clause);
		}
	}
	$output['status'] = 'Ok';
	$output['count'] = $count;
	$output['clauses'] = $clause_output;
	return $output;
}

function db_get_title() {
	$document_id = $_REQUEST['document_id'];
		$record = db_select('crada_document', 'cd')
			->fields('cd')
			->condition('document_id', $document_id, '=')
			->execute()
			->fetchObject();
		$output['status'] = 'Ok';
		$output['title'] = $record->title;
	
	return $output;	
}

function db_get_document_templates() {
	
	$result = db_select('crada_document', 'c')
		->fields('c')
		->condition('is_master', 1, '=')
		->execute();
	
	$list = array();
	for ($count = 0; $row = $result->fetchObject(); $count++) {
		$element = array();
		$document["name"] = $row->document_name;
		$document["title"] = $row->title;
		$document["id"] = $row->document_id;
		array_push($list, $document);
	}
	$output = array();
    $output["status"] = "Ok";
    $output["templates"] = $list;
	return $output;
}

function db_get_questions_for_section() {
	$document_id = intval($_REQUEST['document_id']);
	$section = $_REQUEST['section'];
	
	$result = db_select('crada_question', 'c')
		->fields('c')
		->condition('document_id', $document_id, '=')
		->condition('section', $section, '=')
		->execute();
	
	//	echo $query;
	$list = array();
	for ($count = 0; $row = $result->fetchObject(); $count++) {

		$question["question_id"] = $row->question_id;
		$question["document_id"] = $row->document_id;
		$question["section"] = $row->section;
		$question["predecessor"] = $row->predecessor;
		$question["text"] = $row->question_text;

		$question_id = $row->question_id;
		$r2 = db_select('crada_answer', 'c')
			->fields('c')
			->condition('question_id', $question_id, '=')
			->execute();
		$question['answers'] = array();  //resetting this on each iteration
		for ($c2 = 0; $row2 = $r2->fetchObject(); $c2++) {
			$question['answers'][$c2] = $row2->answer_text;
		}
		array_push($list, $question);

	}
	$output = array();
    $output["status"] = "Ok";
    $output["questions"] = $list;
	return $output;
}

function db_get_section_list() {
	$document_id = intval($_REQUEST['document_id']);
	
	$query = db_select('crada_document_element', 'c')
		->fields('c', array('section'))
		->distinct()
		->condition('document_id', $document_id, '=');

//	die($query);
	$result = $query->execute();
	
	$list = array();
	for ($count = 0; $row = $result->fetchObject(); $count++) {
		array_push($list, $row->section);
	}
	$output = array();
    $output["status"] = "Ok";
    $output["sections"] = $list;
	return $output;
}

function db_get_section() {
	$document_id = $_REQUEST['document_id'];
	$section = $_REQUEST['section'];


	// For now do it as two seperate queries.  First will get the macimum version for each element,
	// Second will run through and populate each element

	$query = db_select('crada_document_element', 'c')
		->fields('c', array('document_element_id', 'document_version'))
		->condition('document_id', $document_id, '=')
		->condition('section', $section, '=')
		->groupBy('document_element_id');
	$query->addExpression('MAX(document_version)', 'max_ver');
	$result = $query->execute();
	
//	$result = db_query(
//			"SELECT document_element_id, MAX(document_version) AS ver FROM crada_document_element
//			WHERE document_id = :doc_id AND   section = :section GROUP BY document_element_id",
//			array(':doc_id'=>$document_id, ':section'=>$section)
//		);

	$list = array();
	foreach ($result as $r) {
		$id = $r->document_element_id;
		$ver = $r->max_ver;
		
		$record = db_select('crada_document_element', 'cd')
			->fields('cd')
			->condition('document_id', $document_id, '=')
			->condition('document_element_id', $id, '=')
			->condition('document_version', $ver, '=')
			->execute()
			->fetchObject();
		
		$element = array();

		$element["id"] = $id;
		$element["version"] = $ver;
		$element["document_id"] = $record->document_id;
		$element["section"] = $record->section;
		$element["location"] = $record->location;
		$element["text"] = $record->document_element_text;
		$element["confidential_annotation"] = $record->confidential_annotation;
		$element["public_annotation"] = $record->public_annotation;
		$element["survivable"] = $record->survivable;
		$element["required"] = $record->required;
		array_push($list, $element);
	}

	$output = array();
    $output["status"] = "Ok";
    $output["elements"] = $list;
	return $output;
}

function db_get_clause_group($document_id, $q, $a) {

	$record = db_select('crada_valid_clause_group_by_answer', 'cd')
	->fields('cd')
	->condition('document_id', $document_id, '=')
	->condition('question_id', $q, '=')
	->condition('answer_id', $a, '=')
	->execute()
	->fetchObject();
	
	return $record->clause_group_id;
}

function db_get_clause_ids_from_clause_group($document_id, $cg, $version) {
	$result = db_select('crada_clause_group_to_clause', 'cd')
	->fields('cd')
	->condition('document_id', $document_id, '=')
	->condition('clause_group_id', $cg, '=')
	->condition('version', $version, '=')
	->execute();
	
	$list = array();
	for ($count = 0; $row = $result->fetchObject(); $count++) {
		array_push($list, $row->clause_id);
	}
	return $list;
}

function db_get_clause_information($document_id, $version, $clause) {
	$record = db_select('crada_document_element', 'cd')
	->fields('cd')
	->condition('document_id', $document_id, '=')
	->condition('document_version', $version, '=')
	->condition('document_element_id', $clause, '=')
	->execute()
	->fetchObject();
	
	$clause_info = array();
	$clause_info['section'] = $record->section;
	$clause_info['alternate_text'] = $record->alternate_text_type;
	$clause_info['text'] = $record->document_element_text;
	$clause_info['confidential_annotation'] = $record->confidential_annotation;
	$clause_info['public_annotation'] = $record->public_annotation;
	$clause_info['survivable'] = $record->survivable;
	$clause_info['required'] = $record->required;

	return $clause_info;

}

function db_get_definitions_for_clause($document_id, $clause_id) {
	
	$result = db_select('crada_definition_element_crosslink', 'cd')
	->fields('cd')
	->condition('document_id', $document_id, '=')
	->condition('clause_id', $clause_id, '=')
	->execute();
	
	$list = array();
	for ($count = 0; $row = $result->fetchObject(); $count++) {
		array_push($list, trim($row->term));
	}
	return $list;	
}

function db_get_demographics() {
	$document_id = $_REQUEST['document_id'];

	$result = db_select('crada_demographics', 'cd')
	->fields('cd')
	->condition('document_id', $document_id, '=')
	->execute();

	$list = array();
	for ($count = 0; $row = $result->fetchObject(); $count++) {
		$obj = array();
		$obj['variable'] = $row->variable;
		$obj['question'] = $row->question;
		$obj['html_type'] = $row->type;

		if(strtoupper($obj['html_type']) == strtoupper('pull down')) {
			$r2 = db_select('crada_demographics_pulldown_options', 'cdpo')
				->fields('cdpo')
				->condition('document_id', $document_id, '=')
				->condition('variable', $obj['variable'], '=')			
				->execute();
			$obj['pulldown_options'] = array();  //resetting this on each iteration
			for ($c2 = 0; $row2 = $r2->fetchObject(); $c2++) {
				$obj['pulldown_options'][$c2] = $row2->pulldown_option;
			}
		}
		array_push($list, $obj);
	}

	$output = array();
    $output["status"] = "Ok";
    $output["demographics"] = $list;
	return $output;

}

function db_get_all_definitions() {
	$document_id = $_REQUEST['document_id'];

	$result = db_select('crada_definition', 'cd')
	->fields('cd')
	->condition('document_id', $document_id, '=')
	->execute();
	
	$list = array();
	for ($count = 0; $row = $result->fetchObject(); $count++) {
		$obj = array();
		$obj['term'] = $row->term;
		$obj['definition'] = $row->definition;
		array_push($list, $obj);
	}
	$output = array();
    $output["status"] = "Ok";
    $output["definitions"] = $list;
	return $output;

}

// Functions for Load Documents

function db_get_all_documents() {

	$result = db_select('crada_document', 'cd')
	->fields('cd')
	->condition('is_master', 1, '!=')
	->execute();

	$list = array();
	for ($count = 0; $row = $result->fetchObject(); $count++) {
		$obj = array();
		$obj['document_id'] = $row->document_id;
		$obj['is_master'] = $row->is_master;
		$obj['name'] = $row->document_name;
		$obj['title'] = $row->title;
		array_push($list, $obj);
	}
	$output = array();
    $output["status"] = "Ok";
    $output["documents"] = $list;
	file_put_contents ( "crada_debug.txt" , "You just got db_get_all_documents", FILE_APPEND);

	return $output;


	global $conn;

	$query = "SELECT * from crada_document";
	$result = $conn->query($query);
	
	$document_list = array();
	for ($count = 0; $row = $result->fetch_object(); $count++) {
		$document['id'] = $row->document_id;
		$document['name'] = $row->document_name;
		$document['title'] = $row->title;
		array_push($document_list, $document);
	}
	return $document_list;
}

function create_new_document () {
	//global $user;

	$data = json_decode($_REQUEST['data'], true);
	$user = $_REQUEST['user'];
	$name = $_REQUEST['name'];
	$master_document_id = $_REQUEST['master_document_id'];
	crada_log('$master_document_id');
	crada_log($master_document_id);

	$title = $_REQUEST['title'];
	//$answers = $_REQUEST['answers'];
  	$output["status"] = "Ok";
  	crada_log('*** You passed in this: $_REQUEST');
  	crada_log(json_encode($_REQUEST));
  	crada_log($_REQUEST['master_document_id']);
  	crada_log("In create_new_document");
  	crada_log(json_encode($answers));

	$transaction = db_transaction();
	try {
	  	crada_log("In db_insert_new_document");
		$document_id = db_insert_new_document($name, $title, $master_document_id);
		db_insert_new_version($document_id, $user);

		for ($i=0;$i<count($data); $i++) {
			db_insert_document_element($document_id, $i, $data[$i]['section'], $data[$i]['text'], $data[$i]['confidential_annotation'], $data[$i]['public_annotation'], $data[$i]['source_question'], $data[$i]['source_answer'], $user);
			$output["row"][$i] = $data[$i]['text'];
			
		}

	  $output["document_id"] = $document_id;
	  $output["count"] = count($data);
  
  	return $output;



	} catch (Exception $e) {
		crada_log("Rolling Back - something went wrong");
		crada_log($e->getMessage(), 1);

		$transaction->rollback();
		watchdog_exception('crada', $e, $e->getMessage());
 	 	$output["status"] = "Error";
 	 	$output["error"] = $e->getMessage();
 	 	return $output;
	}
}

function db_insert_new_document ($name, $title, $master_document_id) {
	$nid = db_insert('crada_document') // Table name no longer needs {}
	->fields(array(
	  'is_master' => 0,
	  'document_name' => $name,
	  'title' => $title,
	  'master_document_id' => $master_document_id
	))
	->execute();	
	  //'master_document_id' => $master_document_id
	
	return $nid;
}

function db_insert_new_version ($document_id, $user) {
	$nid = db_insert('crada_document_version') // Table name no longer needs {}
	->fields(array(
	  'document_id' => $document_id,
	  'version' => 0,
	  'updated_by' => $user,
	  'updated_date' => date("Y-m-d H:i:s", time()), 
	))
	->execute();	
	
	return $nid;
}

function db_insert_document_element ($document_id, $document_element_id, $section, $text, $confidential_annotation, $public_annotation, $source_question, $source_answer, $user) {
	$nid = db_insert('crada_document_element') // Table name no longer needs {}
	->fields(array(
	  'document_element_id' => $document_element_id,
	  'document_version' => 0,
	  'document_id' => $document_id,
	  'section' => $section,
	  'location' => $document_element_id,
	  'alternate_text_type' => 'default',
	  'document_element_text' => $text,
	  'confidential_annotation' => $confidential_annotation,
	  'public_annotation' => $public_annotation,
	  'source_question' => $source_question,
	  'source_answer' => $source_answer,
	  'survivable' => 0,
	  'required' => 0,
	  'updated_by' => $user,
	  'updated_date' => date("Y-m-d H:i:s", time()), 
	))
	->execute();	
	
	return $nid;
}

function get_full_document() {
	$user = $_REQUEST['user'];
	$document_id = $_REQUEST['document_id'];
	$version = $_REQUEST['version'];
	
	$output['status'] = 'Ok';
	$output['title'] = db_get_title_for_document($document_id);
	
	$output['clauses'] = db_get_document_elements($document_id, $version);
	return $output;
	
}

function db_get_title_for_document($document_id) {

	$record = db_select('crada_document', 'cd')
	->fields('cd')
	->condition('document_id', $document_id, '=')
	->execute()
	->fetchObject();
	file_put_contents ( "crada_debug.txt" , "You just got db_get_title_for_document", FILE_APPEND);

	return $record->title;
}

function db_get_document_elements ($document_id, $version) {
	// if version == null, We will need to get the clause that is maximum version
	
	$result = db_select('crada_document_element', 'cd')
	->fields('cd')
	->condition('document_id', $document_id, '=')
	->condition('document_version', $version, '=')
	->execute();
	
	$list = array();
	for ($count = 0; $row = $result->fetchObject(); $count++) {
		$obj = array();
		$obj['text'] = $row->document_element_text;
		$obj['section'] = $row->section;
		$obj['confidential_annotation'] = trim($row->confidential_annotation);
		$obj['public_annotation'] = trim($row->public_annotation);
		array_push($list, $obj);
	}
	return $list;
}

// Debugging functions

function echo_stream() {
	$output = array();
  $output["status"] = "Ok";
  $output["echo"] = file_get_contents("php://input");
//  $output["echo"] = json_decode(file_get_contents("php://input"));
  return $output;
}

function echo_data() {
}

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

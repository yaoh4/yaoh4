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
		else if ($action == 'get_all_documents_info') $output = db_get_all_documents_info();
		else if ($action == 'create_new_document') $output = create_new_document();
		else if ($action == 'get_full_document') $output = get_full_document();
		else if ($action == 'get_answers') $output = get_answers();
		else if ($action == 'set_answer_retrieve_new_element') $output = set_answer_retrieve_new_element();
		else if ($action == 'set_answer') $output = set_answer();
		else if ($action == 'save_clause') $output = save_clause();

		else if ($action == 'echo') $output = echo_data();
		
	} catch (Exception $e) {
		$output['status'] = 'Error';
		$output['message'] = $e->getMessage();
	}

	if (isset($output))
		echo json_encode($output, JSON_PRETTY_PRINT);
	else {
		$output["status"] = 'Error';
		$output["message"] = 'Unknown Function Called. action = '.$action;
		echo json_encode($output, JSON_PRETTY_PRINT);
	}
}

function save_clause() {
	$document_id = $_REQUEST['document_id'];
	$document_element_id = $_REQUEST['document_element_id'];
	crada_log("*** save_clause");
	crada_log("document_element_id = $document_element_id");
	//Get current version
	$element_exits = db_document_element_version_exist($document_id, $document_element_id);
	$current_version = db_document_current_version($document_id, $document_element_id);
	crada_log("element_exists = $element_exits, current_version = $current_version");
	if($element_exits) {
		//Insert a new element
		crada_log("INSERT ELEMENT");

		$query = "insert into crada_document_element  ";
		$query .= "(select ";
		$query .= "cde2.document_element_id, $current_version, cde2.document_id, cde2.section, ";
		$query .= "cde2.location, cde2.alternate_text_type, cde2.document_element_text, ";
		$query .= "cde2.confidential_annotation, cde2.public_annotation, cde2.survivable, ";
		$query .= "cde2.required, cde2.source_question, cde2.source_answer, cde2.answer_changed, ";
		$query .= "cde2.updated_by, cde2.updated_date ";
		$query .= "from crada_document_element cde2, ";
		$query .= "(SELECT max(document_version) as latest_document_version FROM crada_document_element where ";
		$query .= "document_id = $document_id and document_element_id = $document_element_id) cde1 ";
		$query .= "where cde2.document_id = $document_id and cde2.document_element_id = $document_element_id ";
		$query .= "and cde2.document_version = cde1.latest_document_version);";
		crada_log($query);
		$result=db_query($query);
		crada_log(json_encode($result));

		$row = $result->fetchObject();
		crada_log(json_encode($row));
	}
	//Update text and remove changed_answer
	crada_log("UPDATE ELEMENT");
}

function db_document_current_version($document_id, $document_element_id) {
	$query = "SELECT max(document_version) as max_document_version FROM crada_document_element ";
	$query .= " where document_element_id =$document_element_id and document_id=$document_id;";
	crada_log($query);
	$result=db_query($query);

	$row = $result->fetchObject();
	crada_log(json_encode($row));


return $row->max_document_version;

}
function db_document_element_version_exist($document_id, $document_element_id) {
	//
	//  Determine if an document_element exists for the latest version
	//
	$query = "SELECT IF(cde.max_document_version  = cdv.max_version,'true','false') as element_exists FROM ";
	$query .= "(SELECT max(document_version) as max_document_version FROM crada_document_element ";
	$query .= " where document_element_id =$document_element_id and document_id=$document_id) cde, ";
	$query .= "(SELECT max(version) as max_version from  crada_document_version ";
	$query .= " where document_id = $document_id) cdv;";
	crada_log($query);
	$result=db_query($query);

	$row = $result->fetchObject();
	crada_log(json_encode($row));

return $row->element_exists;
	

}
function set_answer_retrieve_new_element() {

	$output = array();
	$list = array();

	$document_id = $_REQUEST['document_id'];
	$question_id = $_REQUEST['question_id'];
	$answer_id = $_REQUEST['answer_id'];

	crada_log("set_answer_retrieve_new_element");
	crada_log("document_id = $document_id question_id = $question_id answer_id = $answer_id");

	$query = "SELECT cde.document_element_text, cde.confidential_annotation, cde.public_annotation ";
	$query .= "FROM drupal.crada_document cd, crada_valid_clause_group_by_answer cv, crada_clause_group_to_clause cc, crada_document_element cde ";
	$query .= "where cd.document_id = $document_id  ";
	$query .= "and cv.question_id = $question_id ";
	$query .= "and cv.answer_id = $answer_id ";
	$query .= "and cd.master_document_id = cv.document_id ";
	$query .= "and cc.document_id = cd.master_document_id  ";
	$query .= "and cc.clause_group_id = cv.clause_group_id ";
	$query .= "and cde.document_element_id = cc.clause_id  ";
	$query .= "and cde.document_id = cd.master_document_id; ";

	$result=db_query($query);

	$row = $result->fetchObject();
	$list["document_element_text"] = $row->document_element_text;
	$list["confidential_annotation"] = $row->confidential_annotation;
	$list["public_annotation"] = $row->public_annotation;
	$output["status"] = "Ok";
    $output["element"] = $list;

    $result = db_select('crada_document', 'cd')
		->fields('cd')
		->condition('document_id', $document_id, '=')
		->execute();
	$row = $result->fetchObject();
	$output["demographic_answers"] = $row->demographic_answers;

	$output["document_id"] = $document_id;
	$output["question_id"] = $question_id;
	$output["answer_id"] = $answer_id;

    return $output;
}

function set_answer() {
	/*
	crada_log("set_answer()");
	crada_log($_REQUEST['data']);
	$data = json_decode($_REQUEST['data'], true);
	$user = $_REQUEST['user'];
	$document_id = $data['document_id'];
	$question_id = $data['question_id'];
	$answer_id = $data['answer_id'];


	crada_log("document_id=".$data['document_id']." question_id=".$data['question_id']." answer_id =".$data['answer_id']. " user =".$user);
	//Find the latest version in crada_document_version

//	get last version and copy to an new record.  add
	$query = "SELECT max(version) as max_version FROM crada_document_version where document_id = $document_id";
 	$result=db_query($query);
  	$row = $result->fetchObject();	
  	crada_log("max_version = ".$row->max_version);
  	$max_version = $row->max_version;
  	crad_log("Max Version = "+$max_version);
  	return;

//	Determine if element exists
  	crada_log("Does row already exist");
  	$query ="SELECT count(*) as count FROM crada_document_element ";
  	$query .= "  where document_id = $document_id ";
  	$query .= "    and source_question = $question_id ";
  	$query .= "    and document_version = $max_version;";
 	$result = db_query($query);
  	$row = $result->fetchObject();	
  	$count = $row->count;
  	crada_log("Row Count = "+$count)
  	return;
  	if($count == 0) {
  		//Insert new row
		$query  = "insert into crada_document_element select cde.document_element_id, cdv.max_version, cde.document_id,	cde.section, ";
		$query .= " 	cde.location, cde.alternate_text_type, cde.document_element_text, cde.confidential_annotation, ";
		$query .= " 	cde.public_annotation, 	cde.survivable, cde.required, cde.source_question, cde.source_answer, 1, ";
		$query .= " 		'unknown', 	now()";
		$query .= " 	from (SELECT * FROM crada_document_element where document_id = $document_id and source_question = $question_id and document_version = $max_version) ";
		$query .= " 	cde, ";
		$query .= " 	(SELECT max(version) as max_version FROM crada_document_version where document_id = 2) cdv;";
	 	$result = db_query($query);

  	}
  	// Update row
	$transaction = db_transaction();
	try {/*
	  	if($count == 0) {
	  		//Insert new row
			$query  = "insert into crada_document_element select cde.document_element_id, cdv.max_version, cde.document_id,	cde.section, ";
			$query .= " 	cde.location, cde.alternate_text_type, cde.document_element_text, cde.confidential_annotation, ";
			$query .= " 	cde.public_annotation, 	cde.survivable, cde.required, cde.source_question, cde.source_answer, 1, ";
			$query .= " 		'unknown', 	now()";
			$query .= " 	from (SELECT * FROM crada_document_element where document_id = $document_id and source_question = $question_id and document_version = $max_version) ";
			$query .= " 	cde, ";
			$query .= " 	(SELECT max(version) as max_version FROM crada_document_version where document_id = 2) cdv;";
		 	$result=db_query($query);

	  	}
		db_update('crada_document_element')
		->fields(array(
		  'document_element_text' => $data["document_element_text"],
		  'confidential_annotation' => $data["public_annotation"],
		  'public_annotation' => $data["public_annotation"],
		  'updated_by' => $user
		))
		->condition('document_id', $document_id, '=')
		->condition('source_question', $question_id, '=')
		->condition('document_version', $max_version, '=')
		->execute();	
		*/      
/*

	$result = db_select('crada_document', 'cd')
		->fields('cd')
		->condition('document_id', $data.['document_id'], '=')
		->execute();

	$row = $result->fetchObject();
	$master_document_id = $row->master_document_id;
*/

/*		$output = array();
	    $output["status"] = "Ok";
	    $output["version"] = $max_version;

	    crada_log("*** Leaving set_answer");
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
*/
}

function get_answers() {
	//$document_id = intval($_REQUEST['document_id']);
	//
	//Hard coded for now.  Retrieve master_document_id from crada_question
	//
	//Look up master

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

function db_get_all_documents_info() {

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

		$r2 = db_select('crada_document_version', 'cdv')
			->fields('cdv')
			->condition('document_id', $row->document_id, '=')
			->execute();
		$obj['versions'] = array();  //resetting this on each iteration
		for ($c2 = 0; $row2 = $r2->fetchObject(); $c2++) {
			$obj['versions'][$c2] = $row2->version;
		}

		$list[$row->document_id] = $obj;
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
	crada_log("create_new_document");
	crada_log("_REQUEST['demographic_answers']");
	crada_log($_REQUEST['demographic_answers']);
	$demographic_answers = json_decode($_REQUEST['demographic_answers'], true);
	crada_log("demographic_answers");
	crada_log(json_encode($demographic_answers));

	$data = json_decode($_REQUEST['data'], true);
	$user = $_REQUEST['user'];
	$name = $_REQUEST['name'];
	$master_document_id = $_REQUEST['master_document_id'];
	crada_log('$master_document_id');
	crada_log($master_document_id);

	$title = $_REQUEST['title'];
	//$answers = $_REQUEST['answers'];
  	$output["status"] = "Ok";
  	crada_log(json_encode($answers));

	$transaction = db_transaction();
	try {
	  	crada_log("**About to step into db_insert_new_document");
	  	//Add document
		$document_id = db_insert_new_document($name, $title, $master_document_id, $_REQUEST['demographic_answers']);
		//Add initial versions
		db_insert_new_version($document_id, $user);
		//Add elements(i.e. clauses, i.e. sections)
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

function db_insert_new_document ($name, $title, $master_document_id, $demographic_answers) {
  	crada_log("***We are in db_insert_new_document");

	crada_log("db_insert_new_document");
	crada_log("name =$name, $master_document_id = $master_document_id");
	crada_log("**************************** DEMOGRAPHIC_ANSWERS");
	crada_log("demographic_answers = $demographic_answers");

	$nid = db_insert('crada_document')
	->fields(array(
	  'is_master' => 0,
	  'document_name' => $name,
	  'title' => $title,
	  'master_document_id' => $master_document_id,
	  'demographic_answers' => $demographic_answers
	))
	->execute();	
	  //'master_document_id' => $master_document_id
	
	return $nid;
}

function db_insert_new_version ($document_id, $user) {
	// All new documents will have a version 0 and 1.
	// Version 0 represents the initial state of the document while all new edits will be saved under version1
	$nid = db_insert('crada_document_version') // Table name no longer needs {}
	->fields(array(
	  'document_id' => $document_id,
	  'version' => 0,
	  'updated_by' => $user,
	  'updated_date' => date("Y-m-d H:i:s", time()), 
	))
	->execute();

	$nid = db_insert('crada_document_version') // Table name no longer needs {}
	->fields(array(
	  'document_id' => $document_id,
	  'version' => 1,
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
	crada_log("get_full_document()");

	$user = $_REQUEST['user'];
	$document_id = $_REQUEST['document_id'];
	$version = $_REQUEST['version'];
	crada_log("user: $user, document_id: $document_id, version: $version");
	
	$output['status'] = 'Ok';
	$output['title'] = db_get_title_for_document($document_id);
	crada_log("get_full_document()");
	$output['editable'] = db_is_version_editable($document_id, $version);
	crada_log("get_full_document()");
	$output['clauses'] = db_get_document_elements($document_id, $version);
	crada_log("get_full_document()");
	$output['version'] = $version;
	return $output;
	
}

function db_is_version_editable($document_id, $version) {

	$query = "SELECT max(version) as max_version FROM crada_document_version where document_id =$document_id";
	crada_log($query);
	$result = db_query($query);

	$row = $result->fetchObject();
	if($row->max_version == $version) {
		$editable = 1;
	} else {
		$editable = 0;
	}

	return $editable;
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
	/* Example query 
SELECT * FROM crada_document_element WHERE document_element_id IN (
	select cde.document_element_id from (
		SELECT document_element_id, MAX(document_version) AS ver
			FROM crada_document_element
			WHERE document_id = 2 
				and document_version <= 2 			
			GROUP BY document_element_id
		) as cde
	)
	and document_id =2;
*/

	$query = "SELECT * FROM crada_document_element WHERE document_element_id IN (";
	$query .= "  select cde.document_element_id from (";
	$query .= "    SELECT document_element_id, MAX(document_version) AS ver";
	$query .= "			FROM crada_document_element ";
	$query .= "			WHERE document_id = $document_id ";
	$query .= "				and document_version <= $version ";
	$query .= "			GROUP BY document_element_id ";
	$query .= "		) as cde ";
	$query .= "	) ";
	$query .= "	and document_id = $document_id; ";

	$result=db_query($query);

	//$list["document_element_text"] = $row->document_element_text;
	//$list["confidential_annotation"] = $row->confidential_annotation;
	//$list["public_annotation"] = $row->public_annotation;

	//$output["status"] = "Ok";
    //$output["element"] = $list;
/*    

	$result = db_select('crada_document_element', 'cd')
	->fields('cd')
	->condition('document_id', $document_id, '=')
	->condition('document_version', $version, '=')
	->execute();
*/	
	$list = array();
	for ($count = 0; $row = $result->fetchObject(); $count++) {
		crada_log(json_encode($row));
		$obj = array();
		$obj['text'] = $row->document_element_text;
		$obj['section'] = $row->section;
		$obj['confidential_annotation'] = trim($row->confidential_annotation);
		$obj['public_annotation'] = trim($row->public_annotation);
		$obj['document_version'] = $row->document_version;
		$obj['answer_changed'] = $row->answer_changed;
		array_push($list, $obj);
		crada_log("count= ".$count."  document_version= ".$row->document_version);
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

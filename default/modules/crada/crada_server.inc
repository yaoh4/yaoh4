<?php
require_once 'crada_utilities.inc';


	global $user;
//
// Make sure user is logged into Drupal
// If not exit.
//
	if (!isset($user->uid)) {
		//user is not logged in Exit()
		crada_log("User is not logged in (crada_server)");
		$user_info = array("uid"=>0, "name"=>"unknown");
		$output["status"] = 'Error';
		$output["message"] = 'Drupal user unknown';
		echo json_encode($output, JSON_PRETTY_PRINT);
		exit();
	} else {
		crada_log("User is logged in (crada_server)");
		$user_info = array("id"=> $user->uid,"name"=> $user->name);
		crada_log("user->uid = ". $user->uid);
		crada_log("user->name = ". $user->name);
	}

function crada_server() {
	$action = $_REQUEST['action'];
	crada_log("ACTION: ".$action);

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
		else if ($action == 'get_document_permissions') $output = get_document_permissions();
		else if ($action == 'set_document_permissions') $output = set_document_permissions();
		else if ($action == 'get_document_owners') $output = get_document_owners();
		else if ($action == 'set_document_owner') $output = set_document_owner();
		else if ($action == 'set_answer_retrieve_new_element') $output = set_answer_retrieve_new_element();
		else if ($action == 'set_answer') $output = set_answer();
		else if ($action == 'save_element') $output = save_element();
		else if ($action == 'archive_version') $output = archive_version();
		else if ($action == 'create_word_file') $output = create_word_file();
		else if ($action == 'get_alternate_text_types') $output = get_alternate_text_types();
		else if ($action == 'get_subsections') $output = get_subsections();
		else if ($action == 'get_clause_information') $output = get_clause_information();
		else if ($action == 'get_document_count') $output = get_document_count();
		else if ($action == 'search_clauses') $output = search_clauses();
		else if ($action == 'echo') $output = echo_data();

	} catch (Exception $e) {
		crada_log("SQL EXCEPTION");
		$output['status'] = 'Error';
		$output['message'] = $e->getMessage();
		$output['e'] = $e;

		crada_log(json_encode($e));
		crada_log_sql_error($e);
	}

	if (isset($output))
		echo json_encode($output, JSON_PRETTY_PRINT);
	else {
		$output["status"] = 'Error';
		$output["message"] = 'Function action = '.$action .'does not produce an output.';
		echo json_encode($output, JSON_PRETTY_PRINT);
	}
}
function search_clauses() {
	$searchterm = $_REQUEST['searchterm'];
	crada_log("SEARCH CLAUSE");
	crada_log("searchterm: $searchterm");

/* OLD Search searches all clauses (faster) */

	$query = "select distinct(user_document.document_id) as document_id from (";
	$query .= base_access_query();
	$query .= ") as user_document, ";
	$query .= "crada_document_element cde ";
	$query .= "WHERE user_document.document_id = cde.document_id ";
	$query .= "AND cde.document_element_text like '%{$searchterm}%' ";
	$query .= "	OR user_document.title like '%{$searchterm}%' ";

/* NEW Search searches only latest clause (slower)*/
/*
	$query = "select distinct(finallist.document_id) as document_id from  ( ";
	$query .= "select document_id, document_element_text, title from ( ";
	$query .= "select newone.document_id, newone.document_element_id, max(newone.document_version) as maxversion, newone.title from ( ";
	$query .= "select cde_link.*, base_search.title from ( ";
	$query .= base_access_query();
	$query .= ") as base_search, ";
	$query .= "(select document_id, document_element_id, document_version from crada_document_element) as cde_link ";
	$query .= "WHERE base_search.document_id = cde_link.document_id ";
	$query .= ") as newone ";
	$query .= "GROUP BY newone.document_id, newone.document_element_id ";
	$query .= ") as maxelements, ";
	$query .= "(select document_id as document_id2, document_element_id, document_version, document_element_text from  ";
	$query .= "crada_document_element) as cde2 ";
	$query .= "WHERE maxelements.document_id = cde2.document_id2 and  maxelements.document_element_id = cde2.document_element_id and maxelements.maxversion = cde2.document_version ";
	$query .= "and cde2.document_element_text like '%{$searchterm}%' or maxelements.title like '%{$searchterm}%'  ";
	$query .= ") as finallist ";
*/
	crada_log ("Start here.  Get only the current clauses.");
	crada_log($query);

	$result = db_query($query);

	$list = array();
	for ($count = 0; $row = $result->fetchObject(); $count++) {
		array_push($list, $row->document_id);
	}

	$output = array();
  $output["status"] = "Ok";
  $output["searchterm"] = $searchterm;
  $output["filter"] = $list;
  crada_log(print_r($output,true));

	return $output;

}

function search_clauses_old_faster() {

	$searchterm = $_REQUEST['searchterm'];
	crada_log("SEARCH CLAUSE");
	crada_log("searchterm: $searchterm");
/* SQL: Example
select distinct(user_document.document_id) as document_id  from
	(select * from ( select docs.*, cdv.version FROM crada_document_version cdv,  (SELECT distinct(cd.document_id), cd.title, cd.is_master FROM users_roles ur, crada_document_role cdr, crada_document cd where ur.uid = 1  and ur.rid = cdr.rid and cdr.document_id = cd.document_id and cdr.access <> '' and cd.uid <> 1 UNION SELECT document_id, title, is_master from crada_document where uid = 1 ) as docs  where  docs.is_master = 0 and docs.document_id = cdv.document_id order by docs.title, cdv.version desc ) as doc_version group by document_id )
as user_document,
crada_document_element cde
WHERE user_document.document_id = cde.document_id
AND cde.document_element_text like '%entity%';
*/
	$query = "select distinct(user_document.document_id) as document_id from (";
	$query .= base_access_query();
	$query .= ") as user_document, ";
	$query .= "crada_document_element cde ";
	$query .= "WHERE user_document.document_id = cde.document_id ";
	$query .= "AND cde.document_element_text like '%{$searchterm}%' ";
	$query .= "	OR user_document.title like '%{$searchterm}%' ";
	crada_log ("Start here.  Get only the current clauses.");
	crada_log($query);

	$result = db_query($query);

	$list = array();
	for ($count = 0; $row = $result->fetchObject(); $count++) {
		array_push($list, $row->document_id);
	}

	$output = array();
  $output["status"] = "Ok";
  $output["searchterm"] = $searchterm;
  $output["filter"] = $list;
  crada_log(print_r($output,true));

	return $output;

}

function get_document_owners() {

	$document_id = $_REQUEST['document_id'];

	$query = "SELECT uid, name, mail FROM users WHERE uid > 1 ORDER BY mail ";
	$result = db_query($query);
	$list["users"] = $result->fetchAll();

	$query = "SELECT uid FROM crada_document WHERE document_id = $document_id ";
	$result = db_query($query);
	$row = $result->fetchObject();
	$list["current_document_owner_uid"] = $row->uid;

	return $list;
}

function get_alternate_text_types() {
	$document_id = $_REQUEST['document_id'];

  $result = db_select('crada_document_element', 'cd')
		->fields('cd', array('alternate_text_type'))
		->condition('document_id', $document_id, '=')
		->distinct()
		->execute();

	$list = array();
	for ($count = 0; $row = $result->fetchObject(); $count++) {
		$type = $row->alternate_text_type;
		array_push($list, $type);
	}
	$output = array();
    $output["status"] = "Ok";
    $output["types"] = $list;
	return $output;

}

function get_subsections() {
	$document_id = $_REQUEST['document_id'];

  $result = db_select('crada_document_element', 'cd')
		->fields('cd', array('subsection'))
		->condition('document_id', $document_id, '=')
		->distinct()
		->execute();

	$list = array();
	for ($count = 0; $row = $result->fetchObject(); $count++) {
		$type = $row->subsection;
		array_push($list, $type);
	}
	$output = array();
    $output["status"] = "Ok";
    $output["subsections"] = $list;
	return $output;
}

function create_word_file() {
	//$library = libraries_load('PHPWord');
	//$library = libraries_load('PHPExcel');
	//crada_log('LIBRARY ...');
	//crada_log(serialize($library));

	//require_once 'src/PhpWord/Autoloader.php';
	// \PhpOffice\PhpWord\Autoloader::register();
//	Autoloader::register();
	//$autoloader = new PHPWord_Autoloader();

//	$phpWord = new PhpWord();
	$filename = 'CRADA.docx';


// Create a new PHPWord Object
	$PHPWord = new PHPWord();

// Every element you want to append to the word document is placed in a section. So you need a section:
	$section = $PHPWord->createSection();

// After creating a section, you can append elements:
	$section->addText('Making a word document.  What');
	//$phpword = new SplClassLoader('PHPWord','sites/default/libraries');
	//$phpword->register();
// At least write the document to webspace:

	$objWriter = PHPWord_IOFactory::createWriter($PHPWord, 'Word2007');
	$objWriter->save('word/'.$filename);

	crada_log("Making the WORD Document");
	$output = array();
	$output['status'] = "OK";
	$output['filename'] = $filename;

	return $output;


	global $filename, $xls_reader, $document_id;

	$filename = "chris.xls";

	$library = libraries_load('PHPExcel');
	if (empty($library['loaded'])) {
		drupal_set_message( "Failed to Load PHPExcel Library" );
		return;
	}

	drupal_set_message(t("Filename is: $filename"));

	$filetype = PHPExcel_IOFactory::identify($filename);
	$xls_reader = PHPExcel_IOFactory::createReader($filetype);
	$xls_reader->setReadDataOnly(FALSE);

	crada_log("Making the WORD Document");
	$output = array();
	$output['status'] = "OK";
	$output['filename'] = $filename;

	return $output;

	/*
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
	*/

}

function archive_version() {
	$output = array();

	$document_id = $_REQUEST['document_id'];
	$version = $_REQUEST['version'];
	$updated_by = $_REQUEST['updated_by'];
	$next_version = $version + 1;

	$query = "insert into crada_document_version  VALUES (";
	$query .= "$document_id, $next_version, '$update_by', ";
	$query .=  "'".date('Y-m-d H:i:s', time())."');";

	crada_log($query);
	$result=db_query($query);
	crada_log("RESULTS:");
	crada_log(json_encode($result));

	//$row = $result->fetchObject();
	//crada_log(json_encode($row));

	$output["status"] = "Ok";
	$output["version"] = $next_version;
	crada_log("OUTPUT IS");
	crada_log(json_encode($output));
	return $output;
}

function save_element() {
	crada_log('save_element- Starting to save crada_document_element');
	crada_log("Alternate Text Type needs to be 'current' for DOCUMENTS ");
	crada_log('save_element- PRIMARY KEY (`document_element_id`,`document_version`,`document_id`,`alternate_text_type`),');

	$alternate_text_type = 'current';

	$document_id = $_REQUEST['document_id'];
	$document_element_id = $_REQUEST['document_element_id'];
	crada_log("*** save_element");
	crada_log("REQUEST is ...");
	crada_log(serialize($_REQUEST));
	//Get current version
	$element_exists = db_document_element_version_exist($document_id, $document_element_id);
	$last_element_version = db_document_last_element_version($document_id, $document_element_id);
	$current_document_version = db_current_document_version($document_id, $document_element_id);
	crada_log('save_element- Does element exists?');
	crada_log("save_element- element_exists = $element_exists");
	crada_log("save_element- last_ element_version = $last_element_version");

	crada_log("save_element- key- document_element_id = $document_element_id");
	crada_log("save_element- key- current_document_version = $current_document_version");
	crada_log("save_element- key- document_id = $document_id");
	crada_log("save_element- key- alternate_text_type = $alternate_text_type");

	if(!$element_exists) {
		crada_log("save_element- HERE IS WHERE THE TROUBLE BEGINS...");
		//Insert a new element
		crada_log("INSERT ELEMENT.  Make new copy of the last element version to the current document version.");
//
//  THIS IS THE ERROR.....
//
		$query = "insert ignore into crada_document_element  ";
		$query .= "(select ";
		$query .= "cde2.document_element_id, $current_document_version, cde2.document_id, cde2.section, ";
		$query .= "cde2.subsection, cde2.location, cde2.alternate_text_type, cde2.document_element_text, ";
		$query .= "cde2.confidential_annotation, cde2.public_annotation, cde2.survivable, ";
		$query .= "cde2.required, cde2.source_question, cde2.source_answer, cde2.answer_changed, ";
		$query .= "cde2.updated_by, cde2.updated_date ";
		$query .= "from crada_document_element cde2, ";
		$query .= "(SELECT max(document_version) as latest_document_version FROM crada_document_element where ";
		$query .= "document_id = $document_id and document_element_id = $document_element_id) cde1 ";
		$query .= "where cde2.document_id = $document_id and cde2.document_element_id = $document_element_id ";
		$query .= "and cde2.alternate_text_type = '$alternate_text_type' ";
		$query .= "and cde2.document_version = cde1.latest_document_version);";
		crada_log($query);
		crada_log("Not executing above");
		try {
			$result=db_query($query);
		} catch (Exception $e) {
			//
			// I don't know why this is throwing a general MYSQL error.  But ignoring becasue it works.
			// This started happening after the PRIMARY KEY was expanded to include alternate_text_type
			crada_log("save_element- Ignore this exception");
			crada_log_sql_error($e);
		}

		/*
		$result=db_query($query);
		crada_log("RESULTS:");
		//crada_log(json_encode($result));
		$row = $result->fetchObject();
		crada_log(json_encode($row));
		*/
	}
	//Update text and remove changed_answer
  global $user;

	$query = "update crada_document_element ";
	$query .= "set ";
	$query .= "{$_REQUEST['update_column']} = '{$_REQUEST['column_text']}',  ";
	$query .= "answer_changed = {$_REQUEST['answer_changed']}, ";
	$query .= "updated_by= '{$user->uid}', ";
	$query .= "updated_date = '".date('Y-m-d H:i:s', time())."' ";
	$query .= "where document_id = $document_id and ";
	$query .= "document_element_id = $document_element_id and ";
	$query .= "document_version = $current_document_version;";
	crada_log($query);

	$result = db_query($query);

	crada_log("UPDATE ELEMENT RESULTS:");

	crada_log(json_encode($result));

  //
  //Increment modified from crada_document.  This automatically updates the updated date.
  //
  $query = 'UPDATE crada_document ';
  $query .= 'SET modified = modified+1, ';
  $query .= 'updated = now() ';
  $query .= "WHERE document_id = $document_id ";
  crada_log("UPDATE CRADA_DOCUMENT  ");
  crada_log($query);

  $result=db_query($query);

	$output["status"] = "Ok";
	crada_log(json_encode($output));

	return $output;

}

function db_current_document_version($document_id) {
	$query = "SELECT max(version) as current_document_version FROM crada_document_version ";
	$query .= " where document_id=$document_id;";
	crada_log($query);
	$result=db_query($query);
	$row = $result->fetchObject();
	crada_log(json_encode($row));

	return $row->current_document_version;
}

function db_document_last_element_version($document_id, $document_element_id) {
	crada_log("last_element_version- ");

	$query = "SELECT max(document_version) as max_document_version FROM crada_document_element ";
	$query .= " where document_element_id =$document_element_id and document_id=$document_id;";
	crada_log($query);
	$result=db_query($query);

	$row = $result->fetchObject();
	crada_log("last_element_version- "+json_encode($row));
	crada_log("last_element_version- max_document_version = {$row->max_document_version}");

	return $row->max_document_version;

}

function db_document_element_version_exist($document_id, $document_element_id) {
	//
	//  Determine if an document_element exists for the latest version
	//
	crada_log("version_exist- Determine if an document_element exists for the latest version");

	$query = "SELECT IF(cde.max_document_version  = cdv.max_version, true ,false) as element_exists FROM ";
	$query .= "(SELECT max(document_version) as max_document_version FROM crada_document_element ";
	$query .= " where document_element_id =$document_element_id and document_id=$document_id) cde, ";
	$query .= "(SELECT max(version) as max_version from  crada_document_version ";
	$query .= " where document_id = $document_id) cdv;";
  crada_log("Determine if a document_element exists for the latest version");
	crada_log($query);
	$result=db_query($query);

	$row = $result->fetchObject();
	crada_log(json_encode($row));
	crada_log("version_exist- Element exists: {$row->element_exists}");

	return $row->element_exists;
}

function set_document_permissions() {

	$output = array();
	$list = array();

	$document_id = $_REQUEST['document_id'];
	$rid = $_REQUEST['question_id'];
	$answer = $_REQUEST['answer_id'];

	crada_log("set_document_permissions");
	crada_log("document_id = $document_id question_id = $rid answer_id = $answer");

	$query = "INSERT INTO crada_document_role (document_id, rid, access) ";
	$query .= "VALUES ($document_id, $rid, '$answer') ";
	$query .= "ON DUPLICATE KEY UPDATE access='$answer' ";
	crada_log($query);

	$result = db_query($query);
	crada_log("RESULTS:");
	crada_log(json_encode($result));

	//$row = $result->fetchObject();
	//crada_log(json_encode($row));

	$output["status"] = "Ok";
	crada_log("OUTPUT IS");
	crada_log(json_encode($output));


	return $output;

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
	$query .= "FROM crada_document cd, crada_valid_clause_group_by_answer cv, crada_clause_group_to_clause cc, crada_document_element cde ";
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

function get_document_permissions() {

	$document_id = $_REQUEST['document_id'];

	$query = "SELECT r.rid, r.name, cdr.access FROM role r ";
	$query .= "LEFT JOIN (select * from crada_document_role where document_id =$document_id) as cdr ";
	$query .= "ON r.rid = cdr.rid ";
	$query .= "WHERE r.rid > 3 ";
	$query .= "order by r.rid; ";

	$result = db_query($query);
	$list = $result->fetchAll();

	return $list;
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

	$message = "";

	$clause_output = array();
	$count=0;
	foreach ($answers as $question => $answer) {
		$count++;
		$q = intval($question);
		$a = intval($answer);

		$cg = db_get_clause_group($document_id, $q, $a);
		$clauses = db_get_clause_ids_from_clause_group($document_id, $cg, $version);
		foreach ($clauses as $clause) {
			$full_clause = db_get_clause_information($document_id, $version, $clause, $alternate_text);
			if ($full_clause == NULL) $full_clause = db_get_clause_information($document_id, $version, $clause, "default");
			// put q&a
			$full_clause['terms'] = db_get_definitions_for_clause($document_id, $clause);
			if ($full_clause['alternate_text'] == $alternate_text) {
				array_push($clause_output, $full_clause);
			} else if ($full_clause['alternate_text'] == "default") {
				// Check to see if the clause is already there.
				array_push($clause_output, $full_clause);
			}
//			array_push($clause_output, $full_clause);
		}
	}
	$output['status'] = 'Ok';
	if ($message != "") $output['message'] = $message;
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
		$question["subsection"] = $row->subsection;
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

// Temp access function
function get_clause_information () {
	$out = db_get_clause_information(1,0,5,'Multiple Collaborators');
	if ($out == NULL) return "NO";

	return $out;
}


function db_get_clause_information($document_id, $version, $clause, $alternate_text_type) {
	$record = db_select('crada_document_element', 'cd')
	->fields('cd')
	->condition('document_id', $document_id, '=')
	->condition('document_version', $version, '=')
	->condition('document_element_id', $clause, '=')
	->condition('alternate_text_type', $alternate_text_type, '=')
	->execute()
	->fetchObject();

	if ($record == NULL) return NULL;

	$clause_info = array();
	$clause_info['document_id'] = $record->document_id;
	$clause_info['version'] = $record->document_version;
	$clause_info['clause_id'] = $record->document_element_id;
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
function base_access_query() {
	//
	// Get the base access_query for accessable documents for this user
	//  This is meant to be used over in over in a few areas
	//  Wanted a single function to reuse so as not to make a mistake.
	//
	global $user;

	$query = "select * from ";
	$query .= "( ";
	$query .= "select docs.*, cdv.version FROM ";
	$query .= "crada_document_version cdv,  ";
	$query .= "(SELECT distinct(cd.document_id), cd.title, cd.is_master ";
	$query .= "FROM users_roles ur, crada_document_role cdr, crada_document cd ";
	$query .= "where ur.uid = {$user->uid}  ";
	$query .= "and ur.rid = cdr.rid ";
	$query .= "and cdr.document_id = cd.document_id ";
	$query .= "and cdr.access <> '' ";
	$query .= "and cd.uid <> {$user->uid} ";
	$query .= "UNION ";
	$query .= "SELECT document_id, title, is_master from crada_document where uid = {$user->uid} ";
	$query .= ") as docs  ";
	$query .= "where  ";
	$query .= "docs.is_master = 0 ";
	$query .= "and docs.document_id = cdv.document_id ";
	$query .= "order by docs.title, cdv.version desc ";
	$query .= ") as doc_version ";
	$query .= "group by document_id ";
	crada_log("BASE QUERY");
	crada_log($query);
	return $query;
}

function get_document_count() {

	crada_log("get_document_count");

	$query = "select count(*) as count from (";
	$query .= base_access_query();
	$query .= ") as document_count; ";

	$output = array();

	$result = db_query($query);
	$row = $result->fetchObject();

	$output["status"] = "Ok";
	$output["count"] = $row->count;

	return $output;
}

function db_get_all_documents_info() {

	global $user;

	$query = "select * from ";
	$query .= "( ";
	$query .= base_access_query();
	$query .= ") as doc_version ";
	$query .= "group by document_id; ";

	$output = array();
	crada_log("db_get_all_documents_info");
	crada_log($query);

	$result = db_query($query);

	$output["status"] = "Ok";
	$output["documents"] = $result->fetchAll();

	return $output;
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
		crada_log("Just created a new document.  Here is the document_id $document_id");
		//Add initial versions
		db_insert_new_version($document_id);
		//Add elements(i.e. clauses, i.e. sections)
		for ($i=0;$i<count($data); $i++) {
			$subsection = $data[$i]['subsection'];
			if ($subsection == NULL) $subsection = "";
			db_insert_document_element($document_id, $i, $data[$i]['section'], $subsection, $data[$i]['text'], $data[$i]['confidential_annotation'], $data[$i]['public_annotation'], $data[$i]['source_question'], $data[$i]['source_answer']);
			$output["row"][$i] = $data[$i]['text'];

		}

	  $output["document_id"] = $document_id;
	  $output["count"] = count($data);

  	return $output;



	} catch (Exception $e) {
		crada_log("Rolling Back db_transaction - something went wrong.");
		crada_log($e->getMessage(), 1);
		crada_log(json_encode($e));

		$transaction->rollback();
		watchdog_exception('crada', $e, $e->getMessage());
		crada_log_sql_error($e);
		$output["status"] = "Error";
		$output["error"] = $e->getMessage();
 	 	return $output;
	}
}

function db_insert_new_document ($name, $title, $master_document_id, $demographic_answers) {
  global $user;
  crada_log("***We are in db_insert_new_document");

	crada_log("db_insert_new_document");
	crada_log("name =$name, master_document_id = $master_document_id");
	crada_log("**************************** DEMOGRAPHIC_ANSWERS");
	crada_log("demographic_answers = $demographic_answers");
  crada_log("GLOBAL uid {$user->uid}");

	$document_id = db_insert('crada_document')
	->fields(array(
	  'is_master' => 0,
	  'document_name' => $name,
	  'title' => $title,
	  'master_document_id' => $master_document_id,
	  'demographic_answers' => $demographic_answers,
    'status' => 1,
    'uid' => $user->uid
	))->execute();
	  //'master_document_id' => $master_document_id
  crada_log($document_id);
  //
  //Grant administrator role write access to document
  //
  $administrator_rid = 3;
  $nid = db_insert('crada_document_role')
  ->fields(array(
    'document_id' => $document_id,
    'rid' => $administrator_rid,
    'access' => 'write'
  ))->execute();


	return $document_id;
}

function db_insert_new_version($document_id) {
	// All new documents will have a version 0 and 1.
	// Version 0 represents the initial state of the document while all new edits will be saved under version1
  global $user;
  crada_log('db_insert_new_version 0');
	$nid = db_insert('crada_document_version') // Table name no longer needs {}
	->fields(array(
	  'document_id' => $document_id,
	  'version' => 0,
	  'updated_by' => $user->uid,
	  'updated_date' => date("Y-m-d H:i:s", time()),
	))
	->execute();

  crada_log('db_insert_new_version 1');
	$nid = db_insert('crada_document_version') // Table name no longer needs {}
	->fields(array(
	  'document_id' => $document_id,
	  'version' => 1,
	  'updated_by' =>  $user->uid,
	  'updated_date' => date("Y-m-d H:i:s", time()),
	))
	->execute();

	return $nid;
}

function db_insert_document_element ($document_id, $document_element_id, $section, $subsection, $text, $confidential_annotation, $public_annotation, $source_question, $source_answer) {

	global $user;

	$nid = db_insert('crada_document_element') // Table name no longer needs {}
	->fields(array(
	  'document_element_id' => $document_element_id,
	  'document_version' => 0,
	  'document_id' => $document_id,
	  'section' => $section,
	  'subsection' => $subsection,
	  'location' => $document_element_id,
	  'alternate_text_type' => 'current',
	  'document_element_text' => $text,
	  'confidential_annotation' => $confidential_annotation,
	  'public_annotation' => $public_annotation,
	  'source_question' => $source_question,
	  'source_answer' => $source_answer,
	  'survivable' => 0,
	  'required' => 0,
	  'updated_by' => $user->uid,
	  'updated_date' => date("Y-m-d H:i:s", time()),
	))
	->execute();

	return $nid;
}

function get_full_document() {
  //
  //Make sure user is authorized to view document...
  //
	global $user;
  //
  //Make sure user is authorized to view or edit this document.
  //
  // Administrators and Document Owners can always edit.
  // Have to make sure to Lock document by making it not active...
  //

	$document_id = $_REQUEST['document_id'];
	$version = $_REQUEST['version'];
  // If $version is not a number then go get the current version
	crada_log("user: {$user->uid}, document_id: $document_id, version: $version");

	$output['status'] = 'Ok';
	$output['document'] = db_get_document_info($document_id);
  // If $version is not a number then go get the current version
  if(!is_numeric($version)) {
  	$version = $output['document']['current_version'];
  	crada_log("SETTING TO LATEST VERSION $version");
  }
  //
  //Check if user has access to this document.
  //
  $output['access'] = db_get_document_access($document_id, $user->uid);
  if($output['access'] == "none") {
  	//
  	//  User does not have access to document.
  	//  Exit without sending document details.
  	//
  	return $output;
  }
	$output['editable'] = db_is_version_editable($document_id, $version);
	//
	// Lock document... make it uneditable if user only has read access.
	//
  if($output['access'] == "read") {
		$output['editable'] = 0;
  }

  $output['version'] = $version;
  crada_log(json_encode($output));

	$output['clauses'] = db_get_document_elements($document_id, $version);

	return $output;

}

function db_get_document_access($document_id, $uid) {

	global $user;
	//
	//$access can be read, write, or none
	//
	$access = "none";
	//
	//  Get max_access user has any for this document
	//
	$query = "select max(cdr.access) as max_access from crada_document_role cdr, users_roles ur
where cdr.rid = ur.rid
and cdr.access <> ''
and ur.uid = $uid
and cdr.document_id = $document_id;";
	$result = db_query($query);
	$row = $result->fetchObject();
	if ($row->max_access != null) {
		$access = $row->max_access;
	}
	//
	// if user is document owner give write access
	//
	$query = "SELECT uid FROM crada_document where document_id = $document_id";
	$result = db_query($query);
	$row = $result->fetchObject();
	if ($row->uid == $uid) {
		crada_log('User is document owner');
		$access = 'write';
	}

	//
	// if user is an administrator give write access
	//
	if (in_array('editor', $user->roles)) {
		crada_log('User is an administrator');
		$access = 'write';
	}

	return $access;

}


function db_is_version_editable($document_id, $version) {

	$query = "SELECT max(version) as max_version FROM crada_document_version where document_id =$document_id";
	$result = db_query($query);

	$row = $result->fetchObject();
	if($row->max_version == $version) {
		$editable = 1;
	} else {
		$editable = 0;
	}

	return $editable;
}

function db_get_document_info($document_id) {
	crada_log("get_document_info- HERE WE GO");

	$output = array();

	$record = db_select('crada_document', 'cd')
	->fields('cd')
	->condition('document_id', $document_id, '=')
	->execute()
	->fetchObject();
	crada_log("You just got db_get_title_for_document");

	$output['title'] = $record->title;
  $output['uid'] = $record->uid;
  //
  // Get current version for this document
  //
	$query = "SELECT max(version) as current_version FROM crada_document_version where document_id = $document_id";
	$result = db_query($query);

	$row = $result->fetchObject();
	$output['current_version'] = $row->current_version;
	crada_log("LATEST VERSION LOOKED UP");
	crada_log(json_encode($output));

  return $output;
}

function db_get_document_elements ($document_id, $version) {
	$query = "SELECT cde2.* FROM crada_document_element cde2, ";
	$query .= "(SELECT document_element_id, MAX(document_version) AS ver ";
	$query .= "		FROM crada_document_element ";
	$query .= "		WHERE document_id = $document_id and document_version <= $version ";
	$query .= "		GROUP BY document_element_id ) cde1 ";
	$query .= "WHERE cde2.document_element_id = cde1.document_element_id ";
	$query .= " and cde2.document_version = cde1.ver ";
	$query .= " and cde2.document_id = $document_id;";
	crada_log("GET LATEST DOCUMENT BASED on a version number");
	crada_log($query);
	$result=db_query($query);
	$list = array();
	for ($count = 0; $row = $result->fetchObject(); $count++) {
		$obj = array();
		$obj['text'] = $row->document_element_text;
		$obj['section'] = $row->section;
		$obj['confidential_annotation'] = trim($row->confidential_annotation);
		$obj['public_annotation'] = trim($row->public_annotation);
		$obj['document_version'] = $row->document_version;
		$obj['answer_changed'] = $row->answer_changed;
		$obj['survivable'] = $row->survivable;

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

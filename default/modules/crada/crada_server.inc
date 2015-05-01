<?php
require_once 'crada_utilities.inc';

//Create new document steps:
//Step 1: get_clauses_from_answers()
//Step 2: get_all_definitions() //Filter by terms used
//Step 3: create_new_document()
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
//		crada_log("User is logged in (crada_server)");
	$user_info = array("id"=> $user->uid,"name"=> $user->name);
//		crada_log("user->uid = ". $user->uid);
//		crada_log("user->name = ". $user->name);
}

/**
* Checks each character of the uri
* Must be a A-Z, a-z, 0-1, +, /, &, ?
* See table:
* http://en.wikipedia.org/wiki/Base64
*/
function valid_uri($uri) {
//
//Remove text from set_annotation and save_element
//
/*
http://localhost/crada/crada/server?action=save_element&document_id=26&document_element_id=10008
&column_text=
ZDxzdHJvbmc
%2B

Q29uZmlkZW50aWFsIEluZm9ybWF0aW9uPC9zdHJvbmc%2BOiAmbGRxdW87Q29uZmlkZW50aWFsIEluZm9ybWF0aW9uJnJkcXVvOyBtZWFucyBjb25maWRlbnRpYWwgc2NpZW50aWZpYywgYnVzaW5lc3MsIG9yIGZpbmFuY2lhbCBpbmZvcm1hdGlvbiBwcm92aWRlZCBieSBlaXRoZXIgUGFydHkgdG8gdGhlIG90aGVyIFBhcnR5IHRoYXQgaXMgbmVjZXNzYXJ5IHRvIGNvbmR1Y3QgdGhlIGFjdGl2aXRpZXMgb2YgdGhlIFJlc2VhcmNoIFBsYW4uIENvbmZpZGVudGlhbCBJbmZvcm1hdGlvbiBkb2VzIG5vdCBpbmNsdWRlIHRoZSBmb2xsb3dpbmc6IChhKSBpbmZvcm1hdGlvbiB0aGF0IGlzIG9yIGJlY29tZXMgcHVibGljbHkga25vd24gb3IgdGhhdCBpcyBhdmFpbGFibGUgZnJvbSBwdWJsaWMgc291cmNlczsgKGIpIGluZm9ybWF0aW9uIHRoYXQgaGFzIGJlZW4gbWFkZSBhdmFpbGFibGUgYnkgaXRzIG93bmVyIHRvIG90aGVycyB3aXRob3V0IGEgY29uZmlkZW50aWFsaXR5IG9ibGlnYXRpb247IChjKSBpbmZvcm1hdGlvbiB0aGF0IGlzIGFscmVhZHkga25vd24gYnkgdGhlIHJlY2VpdmluZyBQYXJ0eSwgb3IgaW5mb3JtYXRpb24gdGhhdCBpcyBpbmRlcGVuZGVudGx5IGNyZWF0ZWQgb3IgY29tcGlsZWQgYnkgdGhlIHJlY2VpdmluZyBQYXJ0eSwgd2l0aG91dCByZWZlcmVuY2UgdG8gb3IgdXNlIG9mIHRoZSBwcm92aWRlZCBpbmZvcm1hdGlvbjsgb3IgKGQpIGluZm9ybWF0aW9uIHRoYXQgcmVsYXRlcyB0byBwb3RlbnRpYWwgaGF6YXJkcyBvciBjYXV0aW9uYXJ5IHdhcm5pbmdzIGFzc29jaWF0ZWQgd2l0aCB0aGUgcHJvZHVjdGlvbiwgaGFuZGxpbmcsIG9yIHVzZSBvZiB0aGUgc3ViamVjdCBtYXR0ZXIgb2YgdGhlIFJlc2VhcmNoIFBsYW47IG9yIChlKSBDUkFEQSBEYXRhLCBkZXNjcmlwdGlvbnMgb2YgQ1JBREEgTWF0ZXJpYWxzLCBvciBJUEksIHdoaWNoIGhhdmUgc2VwYXJhdGUgY29uZmlkZW50aWFsaXR5IG9ibGlnYXRpb25zLg

%3D
%3D

&update_column=document_element_text&

http://localhost/crada/crada/server?action=save_element&document_id=26&document_element_id=41&column_text=

SUNEIEZEQSBhbmQgTXkgQ29sbGFib3JhdG9yIGFncmVlIHRoYXQgdGhleSBoYXZlIHRoZSBmb2xsb3dpbmcgc3RhdHV0b3J5IG9ibGlnYXRpb25zOjxiciAvPgooMSkgR292ZXJubWVudCBsaWNlbnNlIGluIElDRCBGREEgU29sZSBDUkFEQSBTdWJqZWN0IEludmVudGlvbnMgYW5kIEpvaW50IENSQURBIFN1YmplY3QgSW52ZW50aW9ucyBwdXJzdWFudCB0byAxNSBVLlMuQy4gJnNlY3Q7IDM3MTBhKGIpKDEpKEEpIChoeXBlcmxpbmspOzxiciAvPgooMikgR292ZXJubWVudCBsaWNlbnNlIGluIENvbGxhYm9yYXRvciBTb2xlIENSQURBIFN1YmplY3QgSW52ZW50aW9ucyBwdXJzdWFudCB0byAxNSBVLlMuQy4gJnNlY3Q7IDM3MTBhKGIpKDIpIChoeXBlcmxpbmspOyBhbmQ8YnIgLz4KKDMpIFRoaXJkIFBhcnR5IExpY2Vuc2UgcHVyc3VhbnQgdG8gMTUgVS5TLkMuICZzZWN0OyAzNzEwYShiKSgxKShCKSAoaHlwZXJsaW5rKS48YnIgLz4KVGhlIEdvdmVybm1lbnQgd2lsbCBub3QgcHVibGljbHkgZGlzY2xvc2UgdHJhZGUgc2VjcmV0cyBvciBjb21tZXJjaWFsIG9yIGZpbmFuY2lhbCBpbmZvcm1hdGlvbiB0aGF0IGlzZCBwcml2aWxlZ2VkIG9yIGNvbmZpZGVudGlhbCB3aXRoaW4gdGhlIG1lYW5pbmcgb2YgNSBVLlMuQy4gJnNlY3Q7IDU1MihiKSg0KSBvciB3aGljaCB3b3VsZCBiZSBjb25zaWRlcmVkIHByaXZpbGVnZWQgb3IgY29uZmlkZW50aWFsIGlmIGl0IGhhZCBiZWVuIG9idGFpbmVkIGZyb20gYSBub24tZmVkZXJhbCBwYXJ0eS4%3D

&update_column=document_element_text&answer_changed=0&updated_by=admin

http://localhost/crada/crada/server?action=set_annotation
&document_id=26
&document_element_id=39
&annotation_position=7
&annotation_type=public_annotation

&new_annotation=VGhlcmVmb3JlLCB0aGVyZSBpcyBubyBuZWVkIGZvciB0aGUgQ29sbGFib3JhdG9yIHRvIGFkZCB0aGUgb2xkIGxhbmd1YWdlIHRoYXQgZ2l2ZXMgdGhlbSB0aGUgcmlnaHQgb2YgZmlyc3QgcmVmdXNhbCBmb3IgNiBvciBtb3JlIG1vbnRocy4gVGhlIGluY2x1c2lvbiBvZiB0aGF0IGNsYXVzZSBtYWtlcyBpdCBpbXBvc3NpYmxlIGZvciBPVFQgdG8gbWFya2V0IHRoZSB0ZWNobm9sb2d5IGFzIGNvbXBhbmllcyBkbyBub3Qgd2FudCB0byBzdGFydCBuZWdvdGlhdGlvbnMgaWYgYW5vdGhlciBjb21wYW55IGNhbiB0aGVuIHN0ZXAgaW4gYW5kIHRha2UgdGhlIGxpY2Vuc2UuZmRzYQ%3D%3D
*/
	if($_REQUEST['action'] == 'set_annotation') {
		//Remove new_annotation from uri
		$uri = substr($uri, 0, strrpos($uri, 'new_annotation'));
	}
	if($_REQUEST['action'] == 'save_element') {
		//Remove column_text from uri
		$uri = substr($uri, 0, strrpos($uri, 'column_text'));
	}
	if($_REQUEST['action'] == 'get_questions_for_section') {
		//Remove column_text from uri
		$uri = substr($uri, 0, strrpos($uri, 'section'));
	}

	$valid = true;
	// Don't forget & and ?
	$pattern = '([A-Z]|[a-z]|[0-9]|[\+]|[&]|[\?]|[=]|[_]|[/])';
//	$pattern = '/[A-Z]|[0-9]|[a-z]|[+]|[/]/';
	$number = preg_match_all($pattern, $uri, $matches);
	crada_log("preg_match_all: ".$number);
	crada_log("URI:");
	crada_log($uri);
	//crada_log(print_r($matches, true));
	$results = implode("", $matches[0]);
	crada_log($results);
	crada_log('******************************** MATCHES');
	crada_log("String length of uri: ".strlen($uri));
	crada_log("String length of search: ".strlen($results));

	if(strlen($results) != strlen($uri)) {
		$valid = false;
	}

	return $valid;
}

function sanitize_url_variables(){
	//
	//	Sanitize
	//
	crada_log("********************");
	crada_log("check_url_variables");
	crada_log("********************");
	//$uri = parse_url($url, PHP_URL_QUERY);

	$msg["status"] = 'OK';

	$uri = $_SERVER['REQUEST_URI'];

	crada_log($uri);
	//  make sure there are now % symbols
	//Stop any $uri that has an ascii value base 64
	if(!valid_uri($uri)) {
		$msg["status"] = 'Error';
		$msg["message"] = "URI has a none base 64 code.";
		return $msg;
	}

	crada_log(print_r($_REQUEST, true));
	foreach ($_REQUEST as $key => $value) {
		crada_log("Key: $key; Value: $value");
 	   	if($key == "document_id"  
 	   		|| $key == "question_id"
 	   		|| $key == "document_element_id"
 	   		|| $key == "annotation_position"
 	   		) {
			if(!is_numeric($value)) {
				$msg["status"] = 'Error';
				$msg["message"] = "The $key variable in the URI is incorrect.  A number was expected";
			}
 	   	}
	   	if($key == "version") {
			if(!is_numeric($value) && $value !="current" && $value !="undefined") {
				$msg["status"] = 'Error';
				$msg["message"] = "The $key variable in the URI is incorrect.  A number, 'current', or 'undefined' was expected";
			}
 	   	}
 	}

 	crada_log(print_r($msg, true));
	return $msg; 	
}

function crada_server() {
	$msg = sanitize_url_variables();
	if($msg['status'] == "Error") {
		http_response_code(403);
		echo json_encode($msg, JSON_PRETTY_PRINT);
		exit();
	}

	$action = $_REQUEST['action'];
	crada_log("ACTION: ".$action);

	try {
		// Functions for New Document
		if ($action == 'get_title') $output = db_get_title();
		else if ($action == 'get_document_templates') $output = db_get_document_templates();
		else if ($action == 'get_questions_for_section') $output = db_get_questions_for_section();
		else if ($action == 'get_section_list') $output = db_get_section_list();
		else if ($action == 'get_section') $output = db_get_section();
		else if ($action == 'get_demographics') $output = db_get_demographics();
		else if ($action == 'get_all_documents_info') $output = db_get_all_documents_info();
		else if ($action == 'get_clauses_from_answers') $output = get_clauses_from_answers(); //Step 1:
		else if ($action == 'get_all_definitions') $output = db_get_all_definitions(); //Step: 2
		else if ($action == 'create_new_document') $output = create_new_document(); //Step: 3
		else if ($action == 'get_full_document') $output = get_full_document();
		else if ($action == 'get_answers') $output = get_answers();
		else if ($action == 'get_document_permissions') $output = get_document_permissions();
		else if ($action == 'set_document_permissions') $output = set_document_permissions();
		else if ($action == 'set_annotation') $output = set_annotation();

		else if ($action == 'get_document_owners') $output = get_document_owners();
		else if ($action == 'set_document_owner') $output = set_document_owner();
		//else if ($action == 'set_answer_retrieve_new_element') $output = set_answer_retrieve_new_element();
		else if ($action == 'set_new_answer') $output = set_new_answer();
		//else if ($action == 'set_answer') $output = set_answer();
		else if ($action == 'save_element') $output = save_element();
		else if ($action == 'archive_version') $output = archive_version();
		else if ($action == 'create_word_file') $output = create_word_file();
		else if ($action == 'get_alternate_text_types') $output = get_alternate_text_types();
		else if ($action == 'get_subsections') $output = get_subsections();
/*		else if ($action == 'get_clause_information') $output = get_clause_information(); */
		else if ($action == 'get_document_count') $output = get_document_count();
		else if ($action == 'search_clauses') $output = search_clauses();
		else if ($action == 'echo') $output = echo_data();

	} catch (Exception $e) {
		crada_log("*******************");
		crada_log("*  SQL EXCEPTION  *");
		crada_log("*******************");
		$output['status'] = 'Error';
		$output['message'] = $e->getMessage();
		$output['e'] = $e;

		crada_log(json_encode($e));
		crada_log_sql_error($e);
	}

	if (isset($output))
		echo json_encode($output, JSON_PRETTY_PRINT);
	else {
		$error["status"] = 'Error';
		$error["message"] = 'Function action = '.$action .'does not produce an output.';
		
		echo json_encode($error, JSON_PRETTY_PRINT);
	}
}
function search_clauses() {
	$searchterm = htmlentities(checkaddslashes(mysql_real_escape_string(trim(base64_decode($_REQUEST['searchterm'])))));
	//$searchterm = checkaddslashes(trim(base64_decode($_REQUEST['searchterm'])));
	//crada_log("SEARCH CLAUSE");
	//crada_log("searchterm: $searchterm");

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
	//crada_log ("Start here.  Get only the current clauses.");
	//crada_log($query);

	$result = db_query($query);

	$list = array();
	for ($count = 0; $row = $result->fetchObject(); $count++) {
		array_push($list, $row->document_id);
	}

	$output = array();
  $output["status"] = "Ok";
  $output["searchterm"] = $searchterm;
  $output["filter"] = $list;
  //crada_log(print_r($output,true));

	return $output;

}

/*
function search_clauses_old_faster() {

	$searchterm = $_REQUEST['searchterm'];
	crada_log("SEARCH CLAUSE");
	crada_log("searchterm: $searchterm");

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
*/
function get_document_owners() {

	$document_id = $_REQUEST['document_id'];

	$query = "SELECT uid, name, mail FROM users WHERE uid > 1 ORDER BY name ";
	$result = db_query($query);
	$list["users"] = $result->fetchAll();

	$query = "SELECT uid FROM crada_document WHERE document_id = $document_id ";
	$result = db_query($query);
	$row = $result->fetchObject();
	$list["current_document_owner_uid"] = $row->uid;

	return $list;
}

function set_document_owner() {

	$document_id = $_REQUEST['document_id'];
	$user_id = $_REQUEST['user_id'];

	$query = "UPDATE crada_document SET uid = $user_id WHERE document_id = $document_id ";
	$result = db_query($query);

	return;

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

	//crada_log("Making the WORD Document");
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

	//crada_log("Making the WORD Document");
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
  //DO NOT LEAVE THIS FUNCTION ENCODE IT>>>>>>>>>
  //
	crada_log('save_element- Starting to save crada_document_element');
	crada_log("Alternate Text Type needs to be 'current' for DOCUMENTS ");
	crada_log('save_element- PRIMARY KEY (`document_element_id`,`document_version`,`document_id`,`alternate_text_type`),');

//	$alternate_text_type = 'current';

	$document_id = $_REQUEST['document_id'];
	$document_element_id = $_REQUEST['document_element_id'];
	crada_log("*** save_element");
	crada_log("REQUEST is ...");
	crada_log(serialize($_REQUEST));

	$current_document_version = create_element($document_id, $document_element_id);

	//Update text and remove changed_answer
  	global $user;
  	crada_log("BEFORE:");
  	crada_log($_REQUEST['column_text']);
  	$newtext = checkaddslashes(mysql_real_escape_string(trim(base64_decode($_REQUEST['column_text']))));
  	//$newtext = checkaddslashes(trim(base64_decode($_REQUEST['column_text'])));
  	crada_log("AFTER:");
  	crada_log($newtext);

	$query = "update crada_document_element ";
	$query .= "set ";
	$query .= "{$_REQUEST['update_column']} = '{$newtext}',  ";
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

function set_annotation() {
	crada_log("SET ANNOTATION");

	crada_log(print_r($_REQUEST, true));
	extract($_REQUEST);
	crada_log("new_annotation from _REQUEST");
	crada_log($_REQUEST['document_element_id']);
	//$new_annotation = htmlentities(mysql_real_escape_string(trim(base64_decode($new_annotation))));
	$new_annotation = trim(base64_decode($new_annotation));

	//Get rid of the stray apostrophe problem
	//$new_annotation = str_replace("'", "\'", $new_annotation);
	//$new_annotation = str_replace("’", "\'", $new_annotation);
	//$new_annotation = t(trim(base64_decode($new_annotation)));
	//$new_annotation = checkaddslashes(trim(base64_decode($new_annotation)));
	//Remove all \n\n+ series and replace with just one \n
	crada_log("BEFORE preg_replace of new line");
	crada_log($new_annotation);
	//$new_annotation = preg_replace('/\n\n+/', '<br />', $new_annotation);
	//$new_annotation = trim(preg_replace('/\n\n+/', ' ', $new_annotation));
	//$new_annotation = preg_replace("/\r\n|\r|\n/", ' ', $new_annotation);
	$new_annotation = str_replace("\\n", '<br />', $new_annotation);
	//$new_annotation = nl2br($new_annotation);
	crada_log("AFTER preg_replace of new line");
	crada_log($new_annotation);

	crada_log("new_annotation after urldecode and escape_string");
	crada_log($new_annotation);
	crada_log("set_annotation");
	crada_log("document_id = $document_id document_element_id = $document_element_id annotation_position = $annotation_position");
	//
	// Get current annotation.
	//
	$query = "select $annotation_type as annotation from ";
	$query .= "(SELECT max(document_version) as max_version FROM crada_document_element where ";
	$query .= "document_id = $document_id and document_element_id = $document_element_id ";
	$query .= ") mcde, crada_document_element cde ";
	$query .= "where mcde.max_version = cde.document_version ";
	$query .= "and cde.document_id = $document_id and cde.document_element_id = $document_element_id ";

	$result = db_query($query);
	$row = $result->fetchObject();
	crada_log("OLD annotation array");
	crada_log($row->annotation);

	// explode based on \n\n,
	$annotations = explode("\n\n", rtrim($row->annotation));
	//Remove the last element if it is empty
	crada_log("Annotation Count: " .count($annotations));
	crada_log($annotations[count($annotations)-1]);

	// Replace new annotation in the right position
	crada_log(print_r($annotations, true));
	crada_log("new_annotation");
	crada_log($new_annotation);
	$annotations[$annotation_position-1] = rtrim($new_annotation);
	//Remove blank annotations
	foreach ($annotations as $key => $annotation) {
		if(trim($annotation) == "" ) {
			unset($annotations[$key]);
		}
		//clean
		$clean[$key] = mysql_real_escape_string($annotation);
	}
	// Save new annotation in database
	crada_log("NEW annotation array");
	crada_log(print_r($annotations, true));
	//$annotation = implode('\n\n', $annotations);
	$annotation = implode('\n\n', $clean);
	crada_log("Put this back into the database");
	crada_log($annotation);

	//$annotation = checkaddslashes($annotation);
	//crada_log($annotation);

	$query = "UPDATE crada_document_element cde, ";
	$query .= "(SELECT max(document_version) as max_version FROM crada_document_element where  ";
	$query .= "document_id = $document_id and document_element_id = $document_element_id ";
	$query .= ") mcde ";
	//$query .= 'SET cde.'.$annotation_type.'  = "'.$annotation.'" ';
	$query .= "SET cde.$annotation_type  = '{".htmlentities($annotation)."'";
	$query .= "where mcde.max_version = cde.document_version ";
	$query .= "and cde.document_id = $document_id and cde.document_element_id = $document_element_id ";

	crada_log($query);
	$result = db_query($query);

	$output = array();
	$output["status"] = "Ok";

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

function query_get_master_clauses($master_document_id) {
	//
	// Rememember that document_version is always 0 on master_documents
	//  If that changes this query will become invalid.
	$query = "SELECT vc.*, ";
	$query .= "cde.document_element_id, cde.document_version,  ";
	$query .= "cde.alternate_text_type, cde.document_element_text, ";
	$query .= "cde.confidential_annotation, cde.public_annotation, cde.survivable ";
	$query .= "FROM ";
	$query .= "crada_valid_clause_group_by_answer vc, ";
	$query .= "crada_clause_group_to_clause gtc, ";
	$query .= "crada_document_element cde ";
	$query .= "WHERE vc.clause_group_id = gtc.clause_group_id ";
	$query .= "AND vc.document_id = gtc.document_id    ";
	$query .= "AND vc.document_id = cde.document_id ";
	$query .= "AND gtc.clause_id = cde.document_element_id ";
	$query .= "AND vc.document_id = $master_document_id ";
	return $query;
}

function create_element($document_id, $document_element_id) {
	//Returns $current_document_version
	$element_exists = db_document_element_version_exist($document_id, $document_element_id);
	$last_element_version = db_document_last_element_version($document_id, $document_element_id);
	$current_document_version = db_current_document_version($document_id, $document_element_id);
/*
	crada_log('save_element- Does element exists?');
	crada_log("save_element- element_exists = $element_exists");
	crada_log("save_element- last_ element_version = $last_element_version");

	crada_log("save_element- key- document_element_id = $document_element_id");
	crada_log("save_element- key- current_document_version = $current_document_version");
	crada_log("save_element- key- document_id = $document_id");
*/
	if(!$element_exists) {
		copy_element($document_id, $document_element_id);
	}

	return $current_document_version;
}

function copy_element($document_id, $document_element_id) {
		// Get current_document_version
		//
		$current_document_version = db_current_document_version($document_id, $document_element_id);

		crada_log("save_element- HERE IS WHERE THE TROUBLE BEGINS...");
		//Insert a new element
		crada_log("INSERT ELEMENT.  Make new copy of the last element version to the current document version.");
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
//		$query .= "and cde2.alternate_text_type = '$alternate_text_type' ";
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

}

function add_demographics($document_id, $text) {
// Get demographic answers from crada_document
	$query = "select demographic_answers from crada_document where document_id = $document_id";
	$result=db_query($query);
	$row = $result->fetchObject();
	//crada_log("add Mad Lib Demographics.");
	// Convert json to php array
	crada_log($row->demographic_answers);

	$demographics = json_decode($row->demographic_answers, true);
	//crada_log(print_r($demographics, true));

	//Walk through and perform search and replace
//	crada_log("Before demographic: TEXT");
//	crada_log($text);
	foreach ($demographics as $key => $value) {
		crada_log($key ." : " . $value);
		$search = "{".$key."}";
		if($value == "") {
			//If no value bold missing demographics
			$text = str_replace($search, "<strong>[".$key."]</strong>", $text);
		} else {
			$text = str_replace($search, $value, $text);
		}
	}
//	crada_log("After demographic: TEXT");
//	crada_log($text);

	return $text;

}

function set_new_answer() {

	global $user;

	$document_id = $_REQUEST['document_id'];
	$question_id = $_REQUEST['question_id'];
	$answer_id = $_REQUEST['answer_id'];

	crada_log("SET_NEW_ANSWER");
	$statement = "document_id: $document_id, question_id: $question_id, answer_id: $answer_id, user: {$user->uid}";
	crada_log($statement);
	//
	//GET element Row
	// This query will retrieve current version row from
	// crada_document_element table.
	$query = "select document_element_id, max_version, master_document_id, alternate_text_type from ( ";
	$query .= document_current_question_answer_sql($document_id);
	$query .= ") lca WHERE question_id = $question_id ";
	crada_log($query);
	
	$result=db_query($query);
	$row = $result->fetchObject();
	$document_element_id = $row->document_element_id;
	$max_version = $row->max_version;
	$master_document_id = $row->master_document_id;
	$alternate_text_type = $row->alternate_text_type;

	$statement = "
	document_element_id: $document_element_id;
	max_version: $max_version;
	master_document_id: $master_document_id;
	alternate_text_type: $alternate_text_type;";
	crada_log("WHAT IS THE ALTERNATE TEXT");
	crada_log($statement);

	//
	//Check to see if we need to make a copy of current element
	//
	$current_document_version = create_element($document_id, $document_element_id);
	//GET alternate text type.
	crada_log("Get alternate_text_type");
	crada_log("STEP 1:");

	$query = "select * from ( ";
	$query .= query_get_master_clauses($master_document_id);
	$query .= ") master ";
	$query .= "WHERE question_id = $question_id ";
	$query .= "AND answer_id = $answer_id ";
	$query .= "AND alternate_text_type = '$alternate_text_type' ";

	crada_log('QUERY Does it have annotations?????');
	crada_log($query);

	$result=db_query($query);
	$row = $result->fetchObject();
	crada_log("MASTER CLAUSE RESULTS");
	crada_log(print_r($row, true));
	crada_log($row->document_element_text);

	$new_document_element_text = $row->document_element_text;
	$new_confidential_annotation = $row->confidential_annotation;
	$new_public_annotation = $row->public_annotation;
	$new_survivable = $row->survivable;

	if($new_document_element_text == NULL) {
		crada_log("GET DEFAULT TEXT");
		$query = "select * from ( ";
		$query .= query_get_master_clauses($master_document_id);
		$query .= ") master ";
		$query .= "WHERE question_id = $question_id ";
		$query .= "AND answer_id = $answer_id ";
		$query .= "AND alternate_text_type = 'default' ";
		$result=db_query($query);
		$row = $result->fetchObject();
		$new_document_element_text = $row->document_element_text;
		$new_confidential_annotation = $row->confidential_annotation;
		$new_public_annotation = $row->public_annotation;
		$new_survivable = $row->survivable;
	}

	crada_log("***** JUST GOT DONE WITH GET CLAUSE.");

	//ADD Demographics
	crada_log('ORIGINAL TEXT');
	crada_log('document_element_text: '.$new_document_element_text);
	crada_log("confidential_annotation: ".$new_confidential_annotation);
	crada_log("public_annotation: ".$new_public_annotation);

	$new_document_element_text = add_demographics($document_id, $new_document_element_text);
	crada_log('MODIFIIED TEXT');
	crada_log($new_document_element_text);

	crada_log("Here is the new text before question change.");
	//replace in crada_document_element
	//Use this statement to update source_answer, answer_changed, updated_by, document_element_text
	$query = "select * from crada_document_element
					where document_id = $document_id
					and source_question = $question_id
					and document_version = $current_document_version";
	crada_log("document_element_text BEFORE: ");
	
	$result=db_query($query);
	$row = $result->fetchObject();
	$previous_answer_id = $row->source_answer;
	crada_log(print_r($row, true));

	$new_document_element_text = checkaddslashes($new_document_element_text);
	$new_confidential_annotation = checkaddslashes($new_confidential_annotation);
	$new_public_annotation = checkaddslashes($new_public_annotation);

	crada_log("****** Replace in crada_document_element.  Change answer_id.  Set Changed answer to true.");

	$query = "UPDATE ignore crada_document_element
				SET	document_element_text = '".$new_document_element_text."',
					confidential_annotation = '".$new_confidential_annotation."',
					public_annotation = '".$new_public_annotation."',
					survivable = '$new_survivable',
					source_answer = $answer_id,
					answer_changed = 1,
					updated_by = $user->uid
				WHERE document_id = $document_id
				and source_question = $question_id
				and document_version = $current_document_version";
crada_log($query);
	
	try {
		$result = db_query($query);
	} catch (Exception $e) {
		crada_log("***************************");
		crada_log("*  SQL EXCEPTION (BELOW)  *");
		crada_log("***************************");
    	crada_log($e->getMessage());
    	crada_log(print_r($e, true));
		crada_log("***************************");
		crada_log("*  SQL EXCEPTION (ABOVE)  *");
		crada_log("***************************");
	}

/* CHECK AFTER */
	$query = "select * from crada_document_element
					where document_id = $document_id
					and source_question = $question_id
					and document_version = $current_document_version";

	crada_log("document_element_text AFTER: ");
	$result=db_query($query);
	$row = $result->fetchObject();
	crada_log(print_r($row, true));
	//Increment modified.
	$query = 'UPDATE crada_document ';
	$query .= 'SET modified = modified+1, ';
	$query .= 'updated = now() ';
	$query .= "WHERE document_id = $document_id ";
	crada_log("UPDATE CRADA_DOCUMENT  ");
	crada_log($query);

	$result=db_query($query);

	
	//DATABASE Unique KEY: document_id, document_element_id, version

	set_new_definitions($document_id, $question_id, $answer_id, $max_version, $master_document_id);

	$output = array();
	$output["status"] = "OK";

	return $output;
}

function set_new_definitions($document_id, $question_id, $new_answer_id, $previous_answer_id, $max_version, $master_document_id) {
	$statement = "\n\tdocument_id: $document_id;\n\tquestion_id: $question_id;\n\tnew_answer_id: $new_answer_id;\n\tprevious_answer_id: $previous_answer_id;\n\tmax_version: $max_version;\n\tmaster_document_id: $master_document_id;";
	crada_log("SET NEW DEFINITIONS");
	crada_log($statement);

	crada_log("crada_definition for master_id: $master_document_id");
	$query = "SELECT * FROM crada_definition where document_id = 22";
	$result=db_query($query);
	crada_log("CRADA DEFINITIONS");
	for ($count = 0; $row = $result->fetchObject(); $count++) {
		crada_log("document_id: ".$row->document_id.", ".$row->term.", ". $row->definition);
	}
	crada_log("");
	//Get definitions used from previous answer
	//Get definitions from new answer
	//Remove terms that are in common and do nothing with them.
	//Find terms that have gone away.
	// Copy to current_version  and mark as silent.
	//If new (COPY) from defiition

}

/*
function set_answer() {

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
  	crada_log("Row Count = "+$count);
  	return;
  	if($count == 0) {
  		//Insert new row
		$query  = "insert into crada_document_element select cde.document_element_id, cdv.max_version, cde.document_id,	cde.section, ";
		$query .= " 	cde.location, cde.alternate_text_type, cde.document_element_text, cde.confidential_annotation, ";
		$query .= " 	cde.public_annotation, 	cde.survivable, cde.required, cde.source_question, cde.source_answer, 1, ";
		$query .= " 		'unknown', 	now()";
		$query .= " 	from (SELECT * FROM crada_document_element where document_id = $document_id and source_question = $question_id and document_version = $max_version) ";
		$query .= " 	cde, ";
		$query .= " 	(SELECT max(version) as max_version FROM crada_document_version where
		 = 2) cdv;";
	 	$result = db_query($query);

  	}
  	// Update row
	$transaction = db_transaction();
	try {
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



	$result = db_select('crada_document', 'cd')
		->fields('cd')
		->condition('document_id', $data.['document_id'], '=')
		->execute();

	$row = $result->fetchObject();
	$master_document_id = $row->master_document_id;


		$output = array();
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

}
*/
function get_document_permissions() {

	$document_id = $_REQUEST['document_id'];

	$query = "SELECT r.rid, r.name, cdr.access FROM role r ";
	$query .= "LEFT JOIN (select * from crada_document_role where document_id =$document_id) as cdr ";
	$query .= "ON r.rid = cdr.rid ";
	$query .= "WHERE r.rid > 3 ";
	$query .= "order by r.weight; ";

	$result = db_query($query);
	$list = $result->fetchAll();

	return $list;
}

function document_current_question_answer_sql($document_id) {
	//
	// Get question and answers for a document
	//
/*
	$query = "SELECT cde.document_id, cde.document_element_id, max(cde.document_version) as maxversion, cde.source_question, cde.source_answer ";
	$query .= "FROM crada_document_element cde ";
	$query .= "where cde.document_id = {$document_id} ";
	$query .= "and cde.source_question is not null ";
	$query .= "group by cde.document_element_id ";
*/

//Attach the right source_answer
	$query = "select a.*, b.source_answer FROM ( ";
/*Linking current document elements to questions and answers. */
	$query .= "select cde_b.*, cq.question_id, cq.section, cq.subsection, cq.predecessor, cq.question_text FROM ( ";

/* Link to master_document_id*/

	$query .= "select cde_a.*, cd.master_document_id FROM ( ";
/* Base Select with maxversion */
	$query .= "SELECT cde.document_id, cde.document_element_id, max(cde.document_version) as max_version, cde.source_question, cde.alternate_text_type ";
	$query .= "FROM crada_document_element cde ";
	$query .= "where cde.document_id = {$document_id} ";
	$query .= "and cde.source_question is not null ";
	$query .= "group by cde.document_element_id ";

	$query .= ") cde_a, crada_document cd ";
	$query .= "where cd.document_id = cde_a.document_id ";

	$query .= ") cde_b, crada_question cq ";
	$query .= "WHERE cde_b.master_document_id = cq.document_id ";
	$query .= "AND cde_b.source_question = cq.question_id ";
	$query .= "AND cq.question_text <> 'REQUIRED' ";
	$query .= "ORDER BY cde_b.source_question ";

	$query .= ") a, crada_document_element b
							where a.document_id = b.document_id
							and a.document_element_id = b.document_element_id
							and a.max_version = b.document_version ";

	crada_log('document_current_question_answer_sql(document_id)');
	crada_log($query);

return $query;

}

function get_answers() {
	//$document_id = intval($_REQUEST['document_id']);
	//
	//Hard coded for now.  Retrieve master_document_id from crada_question
	//
	//Look up master
	crada_log('get_answers');
	//crada_log(print_r($_REQUEST, true));
	$document_id = $_REQUEST['document_id'];

	//
	// Let's change all these to sql statements to see if we can make this more
	// effecient.
	//



	//
	// Look up crada_document (Get the master_document_id)
	//
	$result = db_select('crada_document', 'cd')
		->fields('cd')
		->condition('document_id', $document_id, '=')
		->execute();
	$row = $result->fetchObject();
	$master_document_id = $row->master_document_id;

	$list = array();
	$output = array();

// Part 2

	$query = document_current_question_answer_sql($document_id);

	crada_log('current_question_answer_sql');

	$result = db_query($query);

	for ($count = 0; $row = $result->fetchObject(); $count++) {
		$question["question_id"] = $row->question_id;
		$question["section"] = $row->section;
		$question["question_text"] = $row->question_text;
		$question["source_answer"] = $row->source_answer;
		$question_id = $row->question_id;
		//
		// Look at crada_document_element
		//
		$r3 = db_select('crada_document_element', 'cde')
				->fields('cde')
				->condition('document_id', $document_id, '=')
				->condition('source_question', $question["question_id"], '=')
				->execute();
		$row3 = $r3->fetchObject();
//		$question["answer"] = $row3->source_answer;
		//
		// Look at crada_answer
		//
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

	$output["status"] = "Ok";
	$output["questions"] =$list;
	$output["count"] = $count;
//	crada_log("get_answers 0 thru 2 output");
//	crada_log(print_r($output["questions"][0], true));
//	crada_log(print_r($output["questions"][1], true));
//	crada_log(print_r($output["questions"][2], true));
	return $output;
}

function get_clauses_from_answers() {
	$master_document_id = $_REQUEST['document_id'];
	$answers = json_decode($_REQUEST['answers']);
	crada_log(print_r($_REQUEST['answers'], true));

// Save answers in crada_document
/*
	if (isset($_REQUEST['alternate_text'])) $alternate_text = $_REQUEST['alternate_text'];
	else $alternate_text = 'default';
	if (isset($_REQUEST['version'])) $version = intval($_REQUEST['version']);
	else $version = 0;
*/

	$alternate_text = (isset($_REQUEST['alternate_text'])) ? $_REQUEST['alternate_text'] : 'default';
	$version = (isset($_REQUEST['version'])) ? $_REQUEST['version'] : 0;


	$clause_output = array();
	$count=0;
	foreach ($answers as $question_id => $answer_id) {
		$count++;
		//$q = intval($question_id);
		//$a = intval($answer);
		//
		// Get a single CLAUSE_GROUP_ID for this master_document_id, question_id, and answer_id
		//
		$cg = db_get_clause_group($master_document_id, $question_id, $answer_id);
		//
		// Get clause_ids from clause
		//
		$clauses = db_get_clause_ids_from_clause_group($master_document_id, $cg, $version);
		foreach ($clauses as $clause) {
			//
			// get ORIGINAL clause information from an ALTERNATE_TEXT
			//
			$full_clause = db_get_clause_information($master_document_id, $version, $clause, $alternate_text);
			if ($full_clause == NULL) {
					//		get ORIGINAL caluse information for "default"
					$full_clause = db_get_clause_information($master_document_id, $version, $clause, "default");
			}
			$full_clause['terms'] = db_get_definitions_for_clause($master_document_id, $clause);
			$full_clause['question_id'] = $question_id;
			$full_clause['answer_id'] = $answer_id;

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
//	$output['count'] = $count;
	$output['clauses'] = $clause_output;
	$output['alternate_text'] = $alternate_text;

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

	$query = db_select('crada_document', 'cd')
		->fields('cd')
		->condition('is_master', 1, '=')
		->orderBy('document_id', 'desc');

	//crada_log((string) $query);
	//crada_log($query->arguments());

	$result = $query->execute();
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
/*
function get_clause_information () {
	$out = db_get_clause_information(1,0,5,'Multiple Collaborators');
	if ($out == NULL) return "NO";

	return $out;
}
*/

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
//	crada_log("clause_info");
//	crada_log(print_r($clause_info, true));

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
//	crada_log("BASE QUERY");
//	crada_log($query);
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

	$data = json_decode($_REQUEST['data'], true);

	$demographic_answers = json_decode($_REQUEST['demographic_answers'], true);
	$master_document_id = $_REQUEST['master_document_id'];
	$alternate_text = $_REQUEST['alternate_text'];
	$title = $_REQUEST['title'];

	//$answers = $_REQUEST['answers'];
	crada_log("ANSWERS");
	crada_log(print_r($answers));

	crada_log(json_encode($answers));

//	crada_log("THE BIG ONE");
//	crada_log(print_r($data, true));

	$transaction = db_transaction();
	try {
		crada_log("**About to step into db_insert_new_document");
		//Add document
		$document_id = db_insert_new_document($title, $master_document_id,
					$_REQUEST['demographic_answers']);
		crada_log("Just created a new document.  Here is the document_id $document_id");
		//Add initial versions
		db_insert_new_version($document_id);
		//Add elements(i.e. clauses, i.e. sections)

		//
		//Walk through each clause and add them to the new document
		//
		//crada_log("HERE IS THE DATA I GOT");
		//crada_log(print_r($data, true));
		for ($i=0;$i<count($data); $i++) {
			//crada_log("section: {$data[$i]['section']}");
			//crada_log("text: {$data[$i]['text']}");
			$clause = $data[$i];
			//
			// To allow definitions to grow when changing answer 
			// we will add a 10,000 offset to the index.
			//	When user changes an answer the definitions can be deleted 
			//  with the offset of 10000
			//

/*
			crada_log(print_r($clause, true));
			crada_log("source_question : ".$clause['source_question']);
			crada_log("source_answer: ".$clause['source_answer']);

			crada_log("Adding New clauses.");
			crada_log("index: $i");
			crada_log("document_id: $document_id");
			crada_log("section:	".$clause['section']);
			crada_log("subsection:	".$clause['subsection']);

			crada_log("alternate_text: {$alternate_text}");
*/
			if($data[$i]['section'] == NULL) {
				crada_log("Got a blank seciton");
			}

			$subsection = $data[$i]['subsection'];
//			crada_log("subsection: $subsection");
			$subsection = ($subsection == NULL) ? "" : $subsection;
			$index = ($data[$i]['section'] == 'Definitions') ? $i + DEFINITION_INDEX_OFFSET : $i;
			crada_log("index = $index");

//			crada_log(print_r($clause,true));
			/*******************************
			// CAUSING ALL THE PROBLEMS HERE
			********************************/
//			crada_log("DB_INSERT_DOCUMENT_ELEMENT");
			db_insert_document_element($document_id,
					$index,
					$data[$i]['section'],
					$subsection,
					$data[$i]['text'],
					$data[$i]['confidential_annotation'],
					$data[$i]['public_annotation'],
					$data[$i]['survivable'],
					$data[$i]['source_question'], $data[$i]['source_answer'],
					$alternate_text);
					$output["row"][$i] = $data[$i]['text'];

	}
		$output["status"] = "Ok";
		$output["document_id"] = $document_id;

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

function db_insert_new_document ($title, $master_document_id, $demographic_answers) {
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
	  'document_name' => '(depricated)',
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

function db_insert_document_element ($document_id, $document_element_id, $section,
		$subsection, $text, $confidential_annotation, $public_annotation, $survivable,
		$source_question, $source_answer, $alternate_text) {

	global $user;

	$survivable = (isset($survivable)) ? $survivable : 0;
	$alternate_text = (isset($alternate_text))? $alternate_text : 'default';
	crada_log("doc: $document_id, element: $document_element_id, survivable: $survivable, alternate_text: $alternate_text, section = $section	");
	$nid = db_insert('crada_document_element') // Table name no longer needs {}
	->fields(array(
	  'document_element_id' => $document_element_id,
	  'document_version' => 0,
	  'document_id' => $document_id,
	  'section' => $section,
	  'subsection' => $subsection,
	  'location' => $document_element_id,
	  'alternate_text_type' => $alternate_text,
	  'document_element_text' => $text,
	  'confidential_annotation' => $confidential_annotation,
	  'public_annotation' => $public_annotation,
	  'source_question' => $source_question,
	  'source_answer' => $source_answer,
	  'survivable' => $survivable,
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
	// Append version to filename if not current
	//
	if($version != $output['document']['current_version']) {
		$output['document']['filename'] .= " (version ". $version.")";
		$output['document']['footer_title'] .= " (version ". $version.")";
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
	$output['filename'] = preg_replace('/[^A-Za-z0-9\. -]/', '', $record->title);
	$output['footer_title'] = $record->title;
	$output['document_id'] = $document_id;

  //
  // Get current version for this document
  //
	$query = "SELECT max(version) as current_version FROM crada_document_version where document_id = $document_id";
	$result = db_query($query);

	$row = $result->fetchObject();
	$output['current_version'] = $row->current_version;
//
// Get template document_name
//
	$query = "SELECT cdm.document_name as base_template 	";
	$query .= "FROM crada_document cd, crada_document cdm  ";
	$query .= "where cd.master_document_id = cdm.document_id ";
	$query .= "and cd.document_id = $document_id";

	$result = db_query($query);
	$row = $result->fetchObject();
	$output['base_template'] = $row->base_template;

// Determine if user can set permission for this document
	global $user;
	$admin_rid = 3;
	$query = "select if(admin.count + doc_owner.count>0, TRUE, FALSE) as set_permission from ( ";
	$query .= "select count(*) as count  ";
	$query .= "from users_roles where uid = {$user->uid} and rid = $admin_rid ";
	$query .= ") admin,  ";
	$query .= "(select count(*) as count  ";
	$query .= "from crada_document where uid = {$user->uid} and document_id = $document_id) doc_owner ";
	$result = db_query($query);
	$row = $result->fetchObject();
	$output['set_permission'] = $row->set_permission;



	crada_log(print_r($output, true));

  return $output;
}

function db_get_document_elements ($document_id, $version) {

	crada_log("GET LATEST DOCUMENT BASED on a version number");

	$list = array();

	$m_query = "select doc3.*, ca.answer_text from ( ";
	$m_query .= "select doc2.*, cq.question_text from (";
	$m_query .= "select doc.*, cd.master_document_id from (";
	$m_query .= "SELECT cde2.* FROM crada_document_element cde2, ";
	$m_query .= "(SELECT document_element_id, MAX(document_version) AS ver ";
	$m_query .= "		FROM crada_document_element ";
	$m_query .= "		WHERE document_id = $document_id and document_version <= $version ";
	$m_query .= "		GROUP BY document_element_id ) cde1 ";
	$m_query .= "WHERE cde2.document_element_id = cde1.document_element_id ";
	$m_query .= " and cde2.document_version = cde1.ver ";
	$m_query .= " and cde2.document_id = $document_id ";
	$m_query .= " and document_element_text not like 'silent' ";
	$m_query .= ") doc, crada_document cd ";
	$m_query .= "where doc.document_id = cd.document_id ";
	$m_query .= ") doc2 ";
	$m_query .= "left join crada_question cq ";
	$m_query .= "on doc2.master_document_id = cq.document_id ";
	$m_query .= "and doc2.source_question = cq.question_id ";
	$m_query .= ") doc3 ";
	$m_query .= "left join crada_answer ca ";
	$m_query .= "on doc3.master_document_id = ca.document_id ";
	$m_query .= "and doc3.source_question = ca.question_id ";
	$m_query .= "and doc3.source_answer = ca.answer_id ";
	//
	// Make three searches filters
	//
	// Filter 1: Search Preamble
	$query = "select * from ( ";
	$query .= $m_query;
	$query .= ") final where section = 'Preamble'; ";

	$result=db_query($query);
	$display_clause_number = FALSE;
	for ($count = 0; $row = $result->fetchObject(); $count++) {
		$obj = array();
		$obj['document_element_id'] = $row->document_element_id;
		$obj['text'] = $row->document_element_text;
		$obj['section'] = $row->section;
		$obj['confidential_annotation'] = trim($row->confidential_annotation);
		$obj['public_annotation'] = trim($row->public_annotation);
		$obj['document_version'] = $row->document_version;
		$obj['answer_changed'] = $row->answer_changed;
		$obj['survivable'] = $row->survivable;
		$obj['question_text'] = is_null($row->question_text) ? 'DEFINITION' : $row->question_text;
		$obj['answer_text'] = $row->answer_text;
		$obj['display_clause_number'] = $display_clause_number;
		array_push($list, $obj);
	}
	// Filter 2: Search Definitions
	$query = "SELECT sf.document_element_id, sf.document_version, sf.document_id, sf.section, sf.subsection, sf.location, sf.alternate_text_type, sf.document_element_text, sf.confidential_annotation, sf.public_annotation, sf.survivable, sf.required, sf.source_question, sf.source_answer, sf.answer_changed, sf.updated_by, sf.updated_date from ( ";
	$query .= "select if(SUBSTR(final.document_element_text,1,8)='<strong>', SUBSTR(final.document_element_text,9), final.document_element_text) as filter_text, final.* from ( ";

	$query .= $m_query;
	$query .= ") final where section = 'Definitions' ";
	$query .= ") sf order by sf.filter_text; ";

	crada_log('DEFINITION Query');
	crada_log($query);

	$result=db_query($query);
	$display_clause_number = FALSE;
	for ($count = 0; $row = $result->fetchObject(); $count++) {
		$obj = array();
		$obj['document_element_id'] = $row->document_element_id;
		$obj['text'] = $row->document_element_text;
		$obj['section'] = $row->section;
		$obj['confidential_annotation'] = trim($row->confidential_annotation);
		$obj['public_annotation'] = trim($row->public_annotation);
		$obj['document_version'] = $row->document_version;
		$obj['answer_changed'] = $row->answer_changed;
		$obj['survivable'] = $row->survivable;
		$obj['question_text'] = is_null($row->question_text) ? 'DEFINITION' : $row->question_text;
		$obj['answer_text'] = $row->answer_text;
		$obj['display_clause_number'] = $display_clause_number;
		array_push($list, $obj);
	}

	// Filter 3: Everything except Definition and Preamble
	$query = "select * from ( ";
	$query .= $m_query;
	$query .= ") final where section <> 'Definitions' and  section <> 'Preamble'; ";

	$result=db_query($query);
	$display_clause_number = TRUE;
	for ($count = 0; $row = $result->fetchObject(); $count++) {
		$obj = array();
		$obj['document_element_id'] = $row->document_element_id;
		$obj['text'] = $row->document_element_text;
		$obj['section'] = $row->section;
		$obj['confidential_annotation'] = trim($row->confidential_annotation);
		$obj['public_annotation'] = trim($row->public_annotation);
		$obj['document_version'] = $row->document_version;
		$obj['answer_changed'] = $row->answer_changed;
		$obj['survivable'] = $row->survivable;
		$obj['question_text'] = is_null($row->question_text) ? 'DEFINITION' : $row->question_text;
		$obj['answer_text'] = $row->answer_text;
		$obj['display_clause_number'] = $display_clause_number;
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

/*
	$query = "select clause_group_id ";
	$query .= "FROM  crada_valid_clause_group_by_answer ";
	$query .= "WHERE document_id = $master_document_id  ";
	$query .= "and question_id = $question_id  ";
	$query .= "and answer_id = $answer_id; ";

	$result=db_query($query);
	$row = $result->fetchObject();
	crada_log(print_r($row, true));
	$clause_group_id = $row->clause_group_id;

	//GET element text
	crada_log("Get clause");
	crada_log("STEP 2:");
	$clauses = db_get_clause_ids_from_clause_group(
				$master_document_id, $clause_group_id, 0);

	crada_log("CLAUSES RESULTS");
	crada_log(json_encode($clauses));


	foreach ($clauses as $clause) {
		//
		// get ORIGINAL clause information from for an ALTERNATE_TEXT
		//
		$full_clause = db_get_clause_information($master_document_id, $version, $clause, $alternate_text);
		if ($full_clause == NULL) {
				//		get ORIGINAL caluse information for "default"
				$full_clause = db_get_clause_information($master_document_id, $version, $clause, "default");
		}
	}
*/

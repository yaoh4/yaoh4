<?php

require_once 'crada_server.inc';
require_once 'sites/default/libraries/PHPRtfLite/PHPRtfLite.php';

function download_document() {
	//echo "Hello";
	crada_log("Hello Download Document");
	create_rtf_file();
}



function create_rtf_file() {
	$doc = get_full_document();

	$random = rand ( 11111 , 32000 );
	$filename = 'CRADA-'.$random.'.rtf';

	PHPRtfLite::registerAutoloader();

	try {

		crada_log("Creating ".$filename);
		// Set page properties
		$rtf = new PHPRtfLite();

		$rtf->setMargins(2.54, 2.54, 2.54, 2.54);
		$rtf->setPaperFormat(PHPRtfLite::PAPER_LETTER);

		// Add footer
		$footer = $rtf->addFooter();
		$fontFooter = new PHPRtfLite_Font(8, 'Arial', '#000000');
		$alignFooter = new PHPRtfLite_ParFormat(PHPRtfLite_ParFormat::TEXT_ALIGN_CENTER);
		$footer->writeText('CRADA - <pagenum>',$fontFooter, $alignFooter);

		//Define fonts
		$fontH1 = new PHPRtfLite_Font(16, 'Arial', '#000000');
		$fontH2 = new PHPRtfLite_Font(14, 'Arial', '#000000');
		$fontP = new PHPRtfLite_Font(12, 'Helvetica', '#000000');

		// Vertical space
		$formatH1 = new PHPRtfLite_ParFormat();
		$formatH1->setSpaceAfter(8);
		$formatH2 = new PHPRtfLite_ParFormat();
		$formatH2->setSpaceAfter(6);
		$formatP = new PHPRtfLite_ParFormat();
		$formatP->setSpaceAfter(3);

		// Page content
		$section = $rtf->addSection();
		$section->writeText($doc['title'], $fontH1, $formatH1);
		/*
		foreach ($doc['clauses'] as $key => $value) {
			$section->writeText($key." ".$doc['clauses'][$value]['text'], $fontP, $formatP);
		}
		*/
		for($i=0;$i<count($doc['clauses']);$i++) {
			$section->writeText($doc['clauses'][$i]['text'], $fontP, $formatP);	
		}
		//while()
		$section->writeText('Here is your new document.....', $fontH2, $formatH2);
		//$section->setNoBreak();

		$section->writeText('<i>Add <u>your <strong>clause</strong> <b>here</b></u></i>', $fontP, $formatP);
		//end while()

		$rtf->sendRtf($filename);

	} catch(Exception $e) {
		$error = $e->getMessage();
		crada_log("RTF Document Error: ".$error);
		crada_log(serialize($e));
		return $error;
	}
}

/* Place these function into a common file...*/
/*
function get_full_document() {

	$user = $_REQUEST['user'];
	$document_id = $_REQUEST['document_id'];
	$version = $_REQUEST['version'];
	crada_log("user: $user, document_id: $document_id, version: $version");
	
	$output['status'] = 'Ok';
	$output['title'] = db_get_title_for_document($document_id);
	$output['editable'] = db_is_version_editable($document_id, $version);
	$output['clauses'] = db_get_document_elements($document_id, $version);
	$output['version'] = $version;
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
	$query = "SELECT cde2.* FROM crada_document_element cde2, ";
	$query .= "(SELECT document_element_id, MAX(document_version) AS ver ";
	$query .= "		FROM crada_document_element ";
	$query .= "		WHERE document_id = $document_id and document_version <= $version ";
	$query .= "		GROUP BY document_element_id ) cde1 ";
	$query .= "WHERE cde2.document_element_id = cde1.document_element_id ";
	$query .= " and cde2.document_version = cde1.ver ";
	$query .= " and cde2.document_id = $document_id;";

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
		array_push($list, $obj);
	}
	return $list;
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



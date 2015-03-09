<?php

ini_set('max_execution_time', 90);

require_once 'crada_utilities.inc';
require_once 'crada_server.inc';
require_once 'crada_pdf.inc';
require_once 'crada_rtf.inc';

//Document Format Details
define('CRADA_FONT_SMALL', 10); //Footer
define('CRADA_FONT_MEDIUM', 12); //Body
define('CRADA_FONT_LARGE', 14); //Major section titles such as Cover Page and Signature Page.
define('CRADA_FONT_FAMILY', 'Times New Roman');
define('CRADA_FONT_COLOR', '#000000');

//Page Titles
define('COVERSHEET_PAGE_TITLE', 'COOPERATIVE RESEARCH AND DEVELOPMENT AGREEMENT (CRADA)');
define('SIGNATURE_PAGE_TITLE', 'SIGNATURE PAGE');
define('CONTACTS_PAGE_TITLE', 'CONTACTS INFORMATION PAGE');
define('SUMMARY_PAGE_TITLE', 'SUMMARY PAGE');
define('APPENDIX_PAGE_TITLE', 'APPENDIX A');
define('DOCUMENT_HEADER', "Cooperative Research Development Agreement");
//Section Titles (if needed)
define('SIGNATURE_SECTION_TITLE', 'ACCEPTED AND AGREED');
define('APPENDIX_SECTION_TITLE', 'RESEARCH PLAN');
//Statments (if needed)
define('SUMMARY_STATEMENT', "EITHER PARTY MAY, WITHOUT FURTHER CONSULTATION OR PERMISSION, RELEASE THIS SUMMARY PAGE TO THE PUBLIC.");
define('SIGNATURE_STATEMENT', "By executing this agreement, each Party represents that all statements made herein are true, complete, and accurate to the best of its knowledge.  Collaborator acknowledges that it may be subject to criminal, civil, or administrative penalties for knowingly making a false, fictitious, or fraudulent statement or claim.");

function download_document() {

	$doc = get_full_document();
  crada_log(print_r($doc,true));

	if($_REQUEST['document_type'] == "PDF") {
		crada_log('Creating PDF file');
		create_pdf_file($doc);
	} else {
		crada_log('Creating RTF file');
		create_rtf_file($doc);
	}

}

function create_pdf_file($doc) {

	$filename = $doc['document']['filename'].'.pdf';

	// create new PDF document
	$pdf = new CradaPdf(PDF_PAGE_ORIENTATION, PDF_UNIT, PDF_PAGE_FORMAT, true, 'UTF-8', false);
	// set document information
	$pdf->setDocumentInfo($doc);
	$pdf->setCoversheet($doc);
	$pdf->setBody($doc);
	// Close and output PDF document
	// This method has several options, check the source code documentation for more information.
	$pdf->Output($filename, 'I');


}

function create_rtf_file($doc) {

  PHPRtfLite::registerAutoloader();
  PHPRtfLite_Unit::setGlobalUnit(PHPRtfLite_Unit::UNIT_INCH);  // inputs used as inches

	$filename = $doc['document']['filename'].'.rtf';

	$rtf = new CradaRtf();
  $rtf->setHeader(DOCUMENT_HEADER);
  $rtf->setFooter($doc['document']);
	$rtf->setCoversheet(COVERSHEET_PAGE_TITLE, $doc['document']);
	$rtf->setBody($doc);
  $rtf->setSignature(SIGNATURE_PAGE_TITLE, SIGNATURE_SECTION_TITLE, SIGNATURE_STATEMENT);
  $rtf->setContacts(CONTACTS_PAGE_TITLE);
  $rtf->setSummary(SUMMARY_PAGE_TITLE, SUMMARY_STATEMENT);
  $rtf->setAppendix(APPENDIX_PAGE_TITLE, APPENDIX_SECTION_TITLE);

	$rtf->send($filename);

}

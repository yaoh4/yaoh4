<?php

require_once 'crada_utilities.inc';
require_once 'crada_server.inc';

function download_document() {
	//echo "Hello";
	crada_log("Awesome sauce.  PDF or Word, You choose...");

	if($_REQUEST['document_type'] == "PDF") {
		create_pdf_file(get_full_document());
	} else {
		create_rtf_file(get_full_document());
	}
}


function create_pdf_file($doc) {
	//============================================================+
	// File name   : example_001.php
	// Begin       : 2008-03-04
	// Last Update : 2013-05-14
	//
	// Description : Example 001 for TCPDF class
	//               Default Header and Footer
	//
	// Author: Nicola Asuni
	//
	// (c) Copyright:
	//               Nicola Asuni
	//               Tecnick.com LTD
	//               www.tecnick.com
	//               info@tecnick.com
	//============================================================+

	/**
	 * Creates an example PDF TEST document using TCPDF
	 * @package com.tecnick.tcpdf
	 * @abstract TCPDF - Example: Default Header and Footer
	 * @author Nicola Asuni
	 * @since 2008-03-04
	 */

	require_once 'sites/default/libraries/tcpdf/tcpdf.php';

	//$doc = get_full_document();

	// create new PDF document
	$pdf = new TCPDF(PDF_PAGE_ORIENTATION, PDF_UNIT, PDF_PAGE_FORMAT, true, 'UTF-8', false);

	// set document information
	$pdf->SetCreator(PDF_CREATOR);
	$pdf->SetAuthor('Crada Builder');
	$pdf->SetTitle('TCPDF Example 001');
	$pdf->SetSubject('TCPDF Tutorial');
	$pdf->SetKeywords('TCPDF, PDF, example, test, guide');

	// set default header data
	$pdf->SetHeaderData(PDF_HEADER_LOGO, PDF_HEADER_LOGO_WIDTH, PDF_HEADER_TITLE.' 001', PDF_HEADER_STRING, array(0,64,255), array(0,64,128));
	$pdf->setFooterData(array(0,64,0), array(0,64,128));

	// set header and footer fonts
	$pdf->setHeaderFont(Array(PDF_FONT_NAME_MAIN, '', PDF_FONT_SIZE_MAIN));
	$pdf->setFooterFont(Array(PDF_FONT_NAME_DATA, '', PDF_FONT_SIZE_DATA));

	// set default monospaced font
	$pdf->SetDefaultMonospacedFont(PDF_FONT_MONOSPACED);

	// set margins
	$pdf->SetMargins(PDF_MARGIN_LEFT, PDF_MARGIN_TOP, PDF_MARGIN_RIGHT);
	$pdf->SetHeaderMargin(PDF_MARGIN_HEADER);
	$pdf->SetFooterMargin(PDF_MARGIN_FOOTER);

	// set auto page breaks
	$pdf->SetAutoPageBreak(TRUE, PDF_MARGIN_BOTTOM);

	// set image scale factor
	$pdf->setImageScale(PDF_IMAGE_SCALE_RATIO);

	// set some language-dependent strings (optional)
	if (@file_exists(dirname(__FILE__).'/lang/eng.php')) {
		require_once(dirname(__FILE__).'/lang/eng.php');
		$pdf->setLanguageArray($l);
	}

	// ---------------------------------------------------------

	// set default font subsetting mode
	$pdf->setFontSubsetting(true);

	// Set font
	// dejavusans is a UTF-8 Unicode font, if you only need to
	// print standard ASCII chars, you can use core fonts like
	// helvetica or times to reduce file size.
	$pdf->SetFont('dejavusans', '', 14, '', true);

	// Add a page
	// This method has several options, check the source code documentation for more information.
	$pdf->AddPage();

	// set text shadow effect
	$pdf->setTextShadow(array('enabled'=>true, 'depth_w'=>0.2, 'depth_h'=>0.2, 'color'=>array(196,196,196), 'opacity'=>1, 'blend_mode'=>'Normal'));

	// Set some content to print
$html = <<<EOD
	<h1>Welcome to <a href="http://www.tcpdf.org" style="text-decoration:none;background-color:#CC0000;color:black;">&nbsp;<span style="color:black;">TC</span><span style="color:white;">PDF</span>&nbsp;</a>!</h1>
	<i>This is the first example of TCPDF library.</i>
	<p>This text is printed using the <i>writeHTMLCell()</i> method but you can also use: <i>Multicell(), writeHTML(), Write(), Cell() and Text()</i>.</p>
	<p>Please check the source code documentation and other examples for further information.</p>
	<p style="color:#CC0000;">TO IMPROVE AND EXPAND TCPDF I NEED YOUR SUPPORT, PLEASE <a href="http://sourceforge.net/donate/index.php?group_id=128076">MAKE A DONATION!</a></p>
EOD;

	// Print text using writeHTMLCell()
	$pdf->writeHTMLCell(0, 0, '', '', $html, 0, 1, 0, true, '', true);

	// ---------------------------------------------------------

	// Close and output PDF document
	// This method has several options, check the source code documentation for more information.
	$pdf->Output('example_001.pdf', 'I');


}

function create_rtf_file($doc) {
	require_once 'sites/default/libraries/PHPRtfLite/PHPRtfLite.php';

	$random = rand ( 11111 , 32000 );
	$filename = 'CRADA-'.$random.'.rtf';

	PHPRtfLite::registerAutoloader();
	PHPRtfLite_Unit::setGlobalUnit(PHPRtfLite_Unit::UNIT_INCH);  // inputs used as inches

	try {

		crada_log("Creating ".$filename);
		$font = 'Times New Roman';
		$fontColor = '#000000';

		// Set page properties
		$rtf = new PHPRtfLite();

		$rtf->setMargins(1, 1, 1, 1);
		$rtf->setPaperFormat(PHPRtfLite::PAPER_LETTER);
		// FONTS Defined
	  $fontHeader = new PHPRtfLite_Font(14, $font, $fontColor);
	  $fontHeaderBold = new PHPRtfLite_Font(14, $font, $fontColor);
	  $fontHeaderBold->setBold();
	  $fontBody = new PHPRtfLite_Font(12, $font, $fontColor);
	  $fontBodyBold = new PHPRtfLite_Font(12, $font, $fontColor);
	  $fontBodyBold->setBold();
	  $fontFooter = new PHPRtfLite_Font(10, $font, $fontColor);

	  // FORMAT Defined
	  $formatPara = new PHPRtfLite_ParFormat();
		//$formatPara->setSpaceAfter(1);
		//$formatPara->setSpaceBetweenLines(1);

	  $formatParaCenter = new PHPRtfLite_ParFormat(PHPRtfLite_ParFormat::TEXT_ALIGN_CENTER);
		//$formatParaCenter->setSpaceAfter(1);
		//$formatParaCenter->setSpaceBetweenLines(1);


/*
// section
$section = $rtf->addSection();
// add left header
$header = $section->addHeader(PHPRtfLite_Container_Header::TYPE_LEFT);
$header->writeText('left header');
// add right header
$header = $section->addHeader(PHPRtfLite_Container_Header::TYPE_RIGHT);
$header->writeText('right header');
*/


		// Add footer
		$footer = $rtf->addFooter();
		$alignFooter = new PHPRtfLite_ParFormat(PHPRtfLite_ParFormat::TEXT_ALIGN_JUSTIFY);
		//$footer->writeText('CRADA - <pagenum>',$fontFooter, $alignFooter);
		$footer->writeText('PHS ICT-CRADA                 Ref. No.<u>  ADA2014  </u>             MODEL ADOPTED June 18, 2009',$fontFooter, $alignFooter);
		$footer->writeText('Page <pagenum> of  <pagetotal>                         <i>Confidential</i>                          Revised August 14, 2013',$fontFooter, $alignFooter);
/*
PHS ICT-CRADA	Agreement Ref. No. _______	MODEL ADOPTED June 18, 2009
Page   of 26	Confidential	Revised August 14, 2013
*/
	/* Add Cover Page */
		$section = $rtf->addSection();
		$content = 'PUBLIC HEALTH SERVICE<br>';
		$section->writeText($content, $fontHeaderBold, $formatParaCenter);
		$section->writeText('<br>');
		$content = 'COOPERATIVE RESEARCH AND DEVELOPMENT AGREEMENT FOR INTRAMURAL-PHS CLINICAL RESEARCH';
		$section->writeText($content, $fontHeaderBold, $formatParaCenter);
		$section->writeText('<br>');
		$section->writeText('<br>');
		$section->writeText('<br>');

$content = 'This Agreement is based on the model Cooperative Research and Development Agreement ("CRADA”) adopted by the U.S. Public Health Service (“PHS”) Technology Transfer Policy Board for use by components of the National Institutes of Health (“NIH”), the Centers for Disease Control and Prevention (“CDC”), and the Food and Drug Administration (“FDA”), which are agencies of the PHS within the Department of Health and Human Services (“HHS”).';
	$section->writeText($content, $fontBody, $formatPara);
	$section->writeText('<br>');
	$content = 'This Cover Page identifies the Parties to this CRADA:';
	$section->writeText($content, $fontBody, $formatPara);
	$section->writeText('<br>');
	$content = 'The U.S. Department of Health and Human Services, as represented by';
	$section->writeText($content, $fontBody, $formatParaCenter);
	$content = 'National Institute of Health';
	$section->writeText($content, $fontBodyBold, $formatParaCenter);
	$content = 'an Institute, Center, or Division (hereinafter referred to as the “IC”) of the';
	$section->writeText($content, $fontBody, $formatParaCenter);
	$content = 'NIH';
	$section->writeText($content, $fontBodyBold, $formatParaCenter);
	$section->writeText('<br>');
	$content = 'and';
	$section->writeText($content, $fontBody, $formatParaCenter);
	$section->writeText('<br>');
	$content = '<b>LEIDOS</b>,';
	$section->writeText($content, $fontBody, $formatParaCenter);
	$content = 'hereinafter referred to as the “Collaborator”,';
	$section->writeText($content, $fontBody, $formatParaCenter);
	$content = 'having offices at 110 Main St., Rockville, MD 20850,';
	$section->writeText($content, $fontBody, $formatParaCenter);
	$content = 'created and operating under the laws of <b>United States of America</b>.';
	$section->writeText($content, $fontBody, $formatParaCenter);
	$section->insertPageBreak();


//		$section->insertPageBreak();


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
		/*
		$section = $rtf->addSection();
		$section->writeText($doc['title'], $fontBody, $formatH1);
		*/
		/*
		foreach ($doc['clauses'] as $key => $value) {
			$section->writeText($key." ".$doc['clauses'][$value]['text'], $fontP, $formatP);
		}
		*/
		for($i=0;$i<count($doc['clauses']);$i++) {
			$section->insertHangingIndent();
			$section->writeText($doc['clauses'][$i]['text'], $fontBody, $formatPara);
		}
		//while()
		//$section->writeText('Here is your new document.....', $fontH2, $formatH2);
		//$section->setNoBreak();

		//$section->writeText('<i>Add <u>your <strong>clause</strong> <b>here</b></u></i>', $fontP, $formatP);
		//end while()

		$rtf->sendRtf($filename);

	} catch(Exception $e) {
		$error = $e->getMessage();
		crada_log("RTF Document Error: ".$error);
		crada_log(serialize($e));
		return $error;
	}
}

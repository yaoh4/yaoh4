<?php

ini_set('max_execution_time', 90);

require_once 'crada_utilities.inc';
require_once 'crada_server.inc';
require_once 'sites/default/libraries/PHPRtfLite/PHPRtfLite.php';
require_once 'sites/default/libraries/tcpdf/tcpdf.php';

function download_document() {

	$doc = get_full_document();

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

	$filename = $doc['document']['filename'].'.rtf';

	/* Coversheet */
	$ci_short = "NIH";
	$ci_long = "Nation Institute of Health";
	$collaborators = ["Leidos", "Sprint", "Genesis Genetics"];
	$collaborators_address = ["210 Market St, Frederick, MD 20840",
														"1000 Elm Ave, Sterling, VA 30320",
														"10234 SW Manhattan Ave, New York, NY 76540"];
	$state_of_inc = ["Maryland", "Virginia", "New York"];

	/* Footer */
	$template_name = "NIH ICT-CRADA";
	$ref_number = "ADA2014";
	$adopted_date = 'June 18, 2009';
	$revised_date = 'August 14, 2013';
	$document_title = 'COOPERATIVE RESEARCH AND DEVELOPMENT AGREEMENT FOR INTRAMURAL-NIH CLINICAL RESEARCH';

	PHPRtfLite::registerAutoloader();
	PHPRtfLite_Unit::setGlobalUnit(PHPRtfLite_Unit::UNIT_INCH);  // inputs used as inches

	$rtf = new CradaRtf();
	$rtf->setCoversheet($document_title, $ci_short, $ci_long, $collaborators, $collaborators_address, $state_of_inc);
	$rtf->setBody($doc, $document_title);
	$rtf->setFooter($template_name, $ref_number, $adopted_date, $revised_date);
	$rtf->send($filename);

}


// Extend the TCPDF class to create custom Header and Footer
class CradaPdf extends TCPDF {

	private $footer_title = "Footer Title";

	function __construct() {
		parent::__construct();
		crada_log("In CradaPdf Class constructor");
		$this->SetFont('times', '', 12);

		$this->SetMargins(25.4, 25.4, 25.4); //PDF_MARGIN_LEFT, PDF_MARGIN_TOP, PDF_MARGIN_RIGHT
		$this->SetHeaderMargin(25.4); //PDF_MARGIN_HEADER
		$this->SetFooterMargin(25.4); //PDF_MARGIN_FOOTER

		// set auto page breaks
		$this->SetAutoPageBreak(TRUE, 25.4);
		$this->AddPage();  //Create document
	}

		public function Header () {

		}
    // Page footer
    public function Footer() {
        $this->SetFont('times', '', 10);
        // Position at 20 mm from bottom
        $this->SetY(-18);
        //$content = $doc['document']['title'];
        $content = $this->footer_title;
        //$content .= 'Ref. No.  ADA2014'.str_repeat(" ", 30);
        //$content .= 'MODEL ADOPTED June 18, 2009';
        $this->Cell(0, 10, $content, 0, false, 'L', 0, '', 0, false, 'T', 'M');
        // 2nd line
        $this->SetY(-15);
        $content = 'Page '.$this->getAliasNumPage().' of '.$this->getAliasNbPages();
        //$content .= 'Confidential'.str_repeat(" ", 50);
        //$content .= 'Revised August 14, 2013';
        $this->Cell(0, 10, $content, 0, false, 'L', 0, '', 0, false, 'T', 'M');
    }

    function setDocumentInfo($doc) {
			global $user;

			$this->footer_title = $doc['document']['footer_title'];

			$this->SetCreator('Agreement Builder');
			$this->SetAuthor($user->name);
			$this->SetTitle($doc['document']['title']);
			$this->SetSubject('COOPERATIVE RESEARCH AND DEVELOPMENT AGREEMENT');
			$this->SetKeywords('Aggreement Builder, CRADA, contract, agreement');
    }

    function setCoversheet($doc) {
		$this->Bookmark("Coversheet", 0, 0, '', '', array(0,0,0));

$html = <<<EOD
<div style="text-align:center;">PUBLIC HEALTH SERVICE</div>
<br>
EOD;
$html .='<div style="text-align:center;">';
$html .= strtoupper($doc['document']['title']);
$html .= "</div>";
$html .= <<<EOD

<br>
<br>
<div>This Agreement is based on the model Cooperative Research and Development Agreement ("CRADA”) adopted by the U.S. Public Health Service (“PHS”) Technology Transfer Policy Board for use by components of the National Institutes of Health (“NIH”), the Centers for Disease Control and Prevention (“CDC”), and the Food and Drug Administration (“FDA”), which are agencies of the PHS within the Department of Health and Human Services (“HHS”).</div>
<br>
EOD;
			$this->_writeHTML($html);
			$this->AddPage();

    }

    function setBody($doc) {
		/* Print Clauses */
		$section_number = 0;
		$current_section = "";
		for($i=0;$i<count($doc['clauses']);$i++) {
				if($doc['clauses'][$i]['section'] != $current_section) {
					//Add Section
					$section_number++;
					$current_section = $doc['clauses'][$i]['section'];

					$this->_writeHTML("<b>$section_number.  $current_section</b><br>");
					$y = $this->y;

					$this->Bookmark("$section_number.  $current_section", 0, $y-15, '', '', array(0,0,0));
					$current_subsection = 1;
				}

				//Add annotation if exists
				$public_annotation = $doc['clauses'][$i]['public_annotation'];
				if(strlen($public_annotation) > 0) {
					$x = $this->x;
					$y = $this->y;
					$this->Annotation(190, $y, 5, 5, $public_annotation,
							array('Subtype'=>'Text',
								'Name' => 'Comment',
								'T' =>  "Public Comment[".$section_number."-".$current_subsection."]",
								'Subj' => 'example',
								'C' => array(255, 255, 0)));
					//$this->_writeHTML("<b>***</b>".$public_annotation);
				}

				$clause = $doc['clauses'][$i]['text'];
				if($doc['clauses'][$i]['section'] != "Definitions") {
						$clause = $section_number.".".$current_subsection."   ".$clause;
				}
				$this->_writeHTML($clause."<br>");

				$current_subsection++;

		}

		if(($survivability_statement = getSurvivabilityStatement($doc)) != "") {
			$this->_writeHTML('<br><br>'.$survivability_statement);
		}

		$this->_writeHTML('<br><br><br><span style="text-align:center;">SIGNATURES BEGIN ON THE NEXT PAGE</span>');
		$this->AddPage();

		$this->_setSignature();

	}

	private function _writeHTML($html) {
		$this->writeHTMLCell(0, 0, '', '', $html, 0, 1, 0, true, '', true);
	}

	private function _setSignature() {
		$this->Bookmark("Signature Page", 0, 0, '', '', array(0,0,0));

		$content ='
<br>
<span style="text-align:center;">SIGNATURE PAGE</span><br>
<br>
<br>
<span style="text-align:center;"><b>ACCEPTED AND AGREED</b></span><br>
<br>
BY EXECUTING THIS AGREEMENT, EACH PARTY REPRESENTS THAT ALL STATEMENTS MADE HEREIN ARE TRUE, COMPLETE, AND ACCURATE TO THE BEST OF ITS KNOWLEDGE. COLLABORATOR ACKNOWLEDGES THAT IT MAY BE SUBJECT TO CRIMINAL, CIVIL, OR ADMINISTRATIVE PENALTIES FOR KNOWINGLY MAKING A FALSE, FICTITIOUS, OR FRAUDULENT STATEMENT OR CLAIM.<br>
<br>
<br>
FOR IC:<br>
<br>
<br>
<br>
____________________________________________&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;________________________<br>
Signature&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Date<br>
<br>
Typed Name:&nbsp;&nbsp;<b>[IC]</b><br>
Title:&nbsp;&nbsp;<b>[Title]</b><br>
<br>
<br>
<br>
<br>
FOR COLLABORATOR:<br>
<br>
<br>
<br>
____________________________________________&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;________________________<br>
Signature&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Date<br>
<br>
Typed Name:&nbsp;&nbsp;<b>[Collaborator]</b><br>
Title:&nbsp;&nbsp;<b>[Title]</b><br>
';
		$this->_writeHTML($content);

	}


}

class CradaRtf extends PHPRtfLite
{

	//public $random = rand ( 11111 , 32000 );
	//public $filename = 'CRADA-'.$random.'.rtf';
	//crada_log("Creating ".$filename);
	protected $_font = 'Times New Roman';
	private $fontColor = '#000000';
	private $fontBody;
	private $fontBodyBold;
	private $formatPara;
	private $bodySection;


	function __construct() {
		parent::__construct();
		crada_log("In CradaRtf SubClass constructor");
		$this->setMargins(1, 1, 1, 1);
		$this->setPaperFormat(PHPRtfLite::PAPER_LETTER);

		$this->fontBody = new PHPRtfLite_Font(12, $this->_font, $this->fontColor);
		$this->fontBodyBold = new PHPRtfLite_Font(12, $this->_font, $this->fontColor);
		$this->fontBodyBold->setBold();

		$this->formatPara = new PHPRtfLite_ParFormat();
	}

	function setFooter($template_name, $ref_number, $adopted_date, $revised_date) {
		$fontFooter = new PHPRtfLite_Font(10, $this->_font, $this->fontColor);
		$formatFooter = new PHPRtfLite_ParFormat(PHPRtfLite_ParFormat::TEXT_ALIGN_JUSTIFY);
		// Add footer
		$footer = $this->addFooter();
		/* Line 1 */
		$content = $template_name.str_repeat(" ", 17);
		$content .= 'Ref. No.<u>  '.$ref_number.'  </u>'.str_repeat(" ", 13);
		$content .= 'MODEL ADOPTED '.$adopted_date;
		$footer->writeText($content, $fontFooter, $formatFooter);
		/* Line 2 */
		$content = 'Page <pagenum> of <pagetotal>                          ';
		$content .= '<b><i>Confidential</i></b>                         ';
		$content .= 'Revised '.$revised_date;
		$footer->writeText($content, $fontFooter, $formatFooter);
	}

	function setCoversheet($document_title, $ci_short, $ci_long, $collaborators, $collaborators_address, $state_of_inc) {
		crada_log("setCoversheet");
		// FONTS Defined
		$fontHeader = new PHPRtfLite_Font(14, $this->_font, $this->fontColor);
		$fontHeaderBold = new PHPRtfLite_Font(14, $this->_font, $this->fontColor);
		// FORMAT Defined
		$formatPara = new PHPRtfLite_ParFormat();
		$formatParaCenter = new PHPRtfLite_ParFormat(PHPRtfLite_ParFormat::TEXT_ALIGN_CENTER);


	/* Add Cover Page */
		$section = $this->addSection();
		$content = 'PUBLIC HEALTH SERVICE<br>';
		$section->writeText($content, $fontHeaderBold, $formatParaCenter);
		$section->writeText('<br>');
		$content = $document_title;
		$section->writeText($content, $fontHeaderBold, $formatParaCenter);
		$section->writeText('<br>');
		$section->writeText('<br>');
		$section->writeText('<br>');

		$content = 'This Agreement is based on the model Cooperative Research and Development Agreement ("CRADA”) adopted by the U.S. Public Health Service (“PHS”) Technology Transfer Policy Board for use by components of the National Institutes of Health (“NIH”), the Centers for Disease Control and Prevention (“CDC”), and the Food and Drug Administration (“FDA”), which are agencies of the PHS within the Department of Health and Human Services (“HHS”).';
		$section->writeText($content, $this->fontBody, $formatPara);
		$section->writeText('<br>');
		$content = 'This Cover Page identifies the Parties to this CRADA:';
		$section->writeText($content, $this->fontBody, $formatPara);
		$section->writeText('<br>');
		$content = 'The U.S. Department of Health and Human Services, as represented by';
		$section->writeText($content, $this->fontBody, $formatParaCenter);
		$content = $ci_long;
		$section->writeText($content, $this->fontBodyBold, $formatParaCenter);
		$content = 'an Institute, Center, or Division (hereinafter referred to as the “IC”) of the';
		$section->writeText($content, $this->fontBody, $formatParaCenter);
		$content = $ci_short;
		$section->writeText($content, $this->fontBodyBold, $formatParaCenter);
		$section->writeText('<br>');
		$content = 'and';
		$section->writeText($content, $this->fontBody, $formatParaCenter);
		$section->writeText('<br>');
		$content = '<b>'.$this->explodeLegalArray($collaborators).'</b>,';
		$section->writeText($content, $this->fontBody, $formatParaCenter);
		$content = 'hereinafter referred to as the “Collaborator”,';
		$section->writeText($content, $this->fontBody, $formatParaCenter);
		$content = 'having offices at <b>'.$this->explodeLegalArray($collaborators_address, ";").'</b>,';
		$section->writeText($content, $this->fontBody, $formatParaCenter);
		$content = 'created and operating under the laws of <b>'.$this->explodeLegalArray($state_of_inc).'</b>.';
		$section->writeText($content, $this->fontBody, $formatParaCenter);
		//$section->insertPageBreak();
	}

	function setBody($doc, $title) {
		$this->bodySection = $this->addSection();
		$this->setTitle($title);
		$content = 'This CRADA between IC and Collaborator will be effective when signed by the Parties, which are identified on both the Cover Page and the Signature Page.  The official contacts for the Parties are identified on the Contacts Information Page.  Publicly available information regarding this CRADA appears on the Summary Page.  The research and development activities that will be undertaken by IC and Collaborator in the course of this CRADA are detailed in the Research Plan, attached as Appendix A.  The staffing, funding, and materials contributions of the Parties are set forth in Appendix B.  Any changes to the model CRADA are set forth in Appendix C';
		$this->setSection("1", "Introduction", $content);
		$content = 'The terms listed in this Article will carry the meanings indicated throughout the CRADA.  To the extent a definition of a term as provided in this Article is inconsistent with a corresponding definition in the applicable sections of either the United States Code (U.S.C.) or the Code of Federal Regulations (C.F.R.), the definition in the U.S.C. or C.F.R. will control.';
		$this->setSection("2", "Definitions", $content);

		/* Print each clause */
 //              \pard \li1440\ri1440\fi480\qj
		/* Print Definitions without section number */
		$formatDefinition = new PHPRtfLite_ParFormat(PHPRtfLite_ParFormat::TEXT_ALIGN_LEFT);
		$formatDefinition->setIndentFirstLine(-0.1);
		$formatDefinition->setIndentLeft(0.5);

		$formatSection = new PHPRtfLite_ParFormat(PHPRtfLite_ParFormat::TEXT_ALIGN_LEFT);

		$formatClause = new PHPRtfLite_ParFormat(PHPRtfLite_ParFormat::TEXT_ALIGN_LEFT);
		$formatClause->setIndentFirstLine(-0.5);
		$formatClause->setIndentLeft(0.5);
		//$formatDefinition->setSpaceAfter(2);
		//$formatDefinition->setIndentRight(0.5);

		//$code = '\pard \li500\fi-500\ql';
		//$this->bodySection->writeRtfCode($code, $this->fontBody, $this->formatPara);
		//$this->bodySection->writeText("<b>Hello</b>.  This is going to work...To the extent a definition of a term as provided in this Article is inconsistent with a corresponding definition in the applicable sections of either the United States Code (U.S.C.) or the Code of Federal Regulations", $this->fontBody, $formatDefinition);
/*
		$obj['text'] = $row->document_element_text;
		$obj['section'] = $row->section;
		$obj['confidential_annotation'] = trim($row->confidential_annotation);
		$obj['public_annotation'] = trim($row->public_annotation);
		$obj['document_version'] = $row->document_version;
		$obj['answer_changed'] = $row->answer_changed;*/

		/* Print Definitions */
		for($i=0;$i<count($doc['clauses']);$i++) {
			if($doc['clauses'][$i]['section'] == "Definitions") {
				$this->bodySection->writeText($doc['clauses'][$i]['text']."\n", $this->fontBody, $formatDefinition);
			}
		}
		/* Print Clauses */
		$section_number = 2;
		$current_section = "Definitions";
		for($i=0;$i<count($doc['clauses']);$i++) {
			if($doc['clauses'][$i]['section'] != "Definitions") {
				if($doc['clauses'][$i]['section'] != $current_section) {
					//Add Section
					$section_number++;
					$current_section = $doc['clauses'][$i]['section'];
					$this->bodySection->writeText("<b>Article ".$section_number.".  ".strtoupper($current_section)."</b>\n", $this->fontBody, $this->formatPara);
					$current_subsection = 1;
				}
				$this->bodySection->writeText($section_number.".".$current_subsection."   ".$doc['clauses'][$i]['text']."\n", $this->fontBody, $formatClause);
				$current_subsection++;
			}
		}
		$this->bodySection->writeText("\n\n\nSIGNATURES BEGIN ON THE NEXT PAGE", $this->fontBody, new PHPRtfLite_ParFormat(PHPRtfLite_ParFormat::TEXT_ALIGN_CENTER));
		$this->bodySection->insertPageBreak();

		$this->setSignature();


	}


	private function setSignature() {
		// FONTS Defined
		$fontHeader = new PHPRtfLite_Font(14, $this->_font, $this->fontColor);
		$fontHeaderBold = new PHPRtfLite_Font(14, $this->_font, $this->fontColor);
		// FORMAT Defined
		$formatPara = new PHPRtfLite_ParFormat();
		$formatParaCenter = new PHPRtfLite_ParFormat(PHPRtfLite_ParFormat::TEXT_ALIGN_CENTER);
		$content= 'SIGNATURE PAGE';
		$this->bodySection->writeText("\n".$content."\n\n", $fontHeader, $formatParaCenter);
		$content= '<b>ACCEPTED AND AGREED</b>';
		$this->bodySection->writeText($content."\n", $this->fontBody, $formatParaCenter);

		$content ='BY EXECUTING THIS AGREEMENT, EACH PARTY REPRESENTS THAT ALL STATEMENTS MADE HEREIN ARE TRUE, COMPLETE, AND ACCURATE TO THE BEST OF ITS KNOWLEDGE. COLLABORATOR ACKNOWLEDGES THAT IT MAY BE SUBJECT TO CRIMINAL, CIVIL, OR ADMINISTRATIVE PENALTIES FOR KNOWINGLY MAKING A FALSE, FICTITIOUS, OR FRAUDULENT STATEMENT OR CLAIM.


FOR IC:



_______________________________________________      ________________________
Signature                                              Date

Typed Name:
Title:




FOR COLLABORATOR:



_______________________________________________      ________________________
Signature                                              Date


Typed Name:
Title:
';
		$this->bodySection->writeText($content, $this->fontBody, $this->formatPara);

	}

	private function setTitle($title) {
		$formatParaCenter = new PHPRtfLite_ParFormat(PHPRtfLite_ParFormat::TEXT_ALIGN_CENTER);

		$this->bodySection->writeText($title, $this->fontBody, $formatParaCenter);
		$this->bodySection->writeText('<br>');
	}
	private function setSection($section_number, $title, $content) {
		$this->bodySection->writeText("<b>Article ".$section_number.".  ".strtoupper($title)."</b>", $this->fontBody, $this->formatPara);
		$this->bodySection->writeText('<br>');
		$this->bodySection->writeText($content, $this->fontBody, $this->formatPara);
		$this->bodySection->writeText('<br>');
	}

	private function setIntroduction() {
		//$formatParaCenter = new PHPRtfLite_ParFormat(PHPRtfLite_ParFormat::TEXT_ALIGN_CENTER);

			/* Add Cover Page */
		$section = $this->addSection();
		$section->writeText("1.\tabIntroduction", $this->fontBody);
		$section->writeText('<br>');
		$content ='This CRADA between IC and Collaborator will be effective when signed by the Parties, which are identified on both the Cover Page and the Signature Page.  The official contacts for the Parties are identified on the Contacts Information Page.  Publicly available information regarding this CRADA appears on the Summary Page.  The research and development activities that will be undertaken by IC and Collaborator in the course of this CRADA are detailed in the Research Plan, attached as Appendix A.  The staffing, funding, and materials contributions of the Parties are set forth in Appendix B.  Any changes to the model CRADA are set forth in Appendix C.';
		$section->writeText($content, $this->fontBody);
		$section->writeText('<br>');

	}

	function explodeLegalArray($arrayOfEntities, $delimiter=",") {
		$i = 0;
		$legal ="";

		if(count($arrayOfEntities) == 1){
			$legal = $arrayOfEntities[0];
		} else {
			foreach($arrayOfEntities as $value) {
				$i++;
				if($i == 1) {
					$legal = $value;
				} elseif($i == count($arrayOfEntities)) {
					$legal .= $delimiter." and ".$value;
				} else {
					$legal .= $delimiter." ".$value;
				}
			}
		}
		return $legal;
	}

	function send($filename) {
		$this->sendRtf($filename);
	}

// Redefine the parent method
	function displayVar()
	{
		echo "Extending class\n";
		parent::displayVar();
	}
}




function create_rtf_file_old($doc) {

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
	  $formatParaCenter = new PHPRtfLite_ParFormat(PHPRtfLite_ParFormat::TEXT_ALIGN_CENTER);
		$formatFooter = new PHPRtfLite_ParFormat(PHPRtfLite_ParFormat::TEXT_ALIGN_JUSTIFY);

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
		$content = 'PHS ICT-CRADA                  Ref. No.<u>  ADA2014  </u>             MODEL ADOPTED June 18, 2009';
		$footer->writeText($content, $fontFooter, $formatFooter);
		$content = 'Page <pagenum> of <pagetotal>                          <i>Confidential</i>                         Revised August 14, 2013';
		$footer->writeText($content, $fontFooter, $formatFooter);
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

function getSurvivabilityStatement($doc) {
	$output = array();

	$section_number = 0;
	$current_section = "";
	for($i=0;$i<count($doc['clauses']);$i++) {
			//crada_log("Current Section: $current_section");
			if($doc['clauses'][$i]['section'] != $current_section) {
				//crada_log("New Section: {$doc['clauses'][$i]['section']}");
				$current_section = $doc['clauses'][$i]['section'];
				//Add Section
				$section_number++;
				$current_subsection = 1;
			}
			if($doc['clauses'][$i]['survivable']) {
					//crada_log(print_r($doc['clauses'][$i], true));
					$output[] = $section_number."-".$current_subsection;
			}
			$current_subsection++;
	}

	$survivability_statement = "";

	if(count($output) > 0) {
		$label = (count($output) == 1) ? "Paragraph" : "Paragraphs";

		$survivability_statement = "<strong>Survivability</strong>.  ";
		$survivability_statement .= "The provisions of ".$label." ";
		$survivability_statement .= explodeLegalArray($output);
		$survivability_statement .= " will survive the expiration or early termination of this CRADA.";
	}

	return $survivability_statement;
}

function explodeLegalArray($arrayOfEntities, $delimiter=",") {
	$i = 0;
	$legal ="";

	if(count($arrayOfEntities) == 1){
		$legal = $arrayOfEntities[0];
	} else {
		foreach($arrayOfEntities as $value) {
			$i++;
			if($i == 1) {
				$legal = $value;
			} elseif($i == count($arrayOfEntities)) {
				$legal .= $delimiter." and ".$value;
			} else {
				$legal .= $delimiter." ".$value;
			}
		}
	}
	return $legal;
}


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

  PHPRtfLite::registerAutoloader();
  PHPRtfLite_Unit::setGlobalUnit(PHPRtfLite_Unit::UNIT_INCH);  // inputs used as inches

	$filename = $doc['document']['filename'].'.rtf';

	$rtf = new CradaRtf();
	$rtf->setCoversheet($doc);
	$rtf->setBody($doc);
	$rtf->setFooter($doc);
	$rtf->send($filename);

}

class CradaRtf extends PHPRtfLite {

  //public $random = rand ( 11111 , 32000 );
  //public $filename = 'CRADA-'.$random.'.rtf';
  //crada_log("Creating ".$filename);
  protected $_font = 'Times New Roman';
  protected $_fontSize = 15;
  protected $_fontSize_footer = 12;

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

    $this->fontBody = new PHPRtfLite_Font($this->_fontSize, $this->_font, $this->fontColor);
    $this->fontBodyBold = new PHPRtfLite_Font($this->_fontSize, $this->_font, $this->fontColor);
    $this->fontBodyBold->setBold();

    $this->formatPara = new PHPRtfLite_ParFormat();
  }

  function setFooter($doc) {
    $fontFooter = new PHPRtfLite_Font($this->_fontSize_footer, $this->_font, $this->fontColor);
    $formatFooter = new PHPRtfLite_ParFormat(PHPRtfLite_ParFormat::TEXT_ALIGN_JUSTIFY);
    // Add footer
    $footer = $this->addFooter();
    /* Line 1 */
    $content = $doc['document']['footer_title'];
    $footer->writeText($content, $fontFooter, $formatFooter);
    /* Line 2 */
    $content = 'Page <pagenum> of <pagetotal>';
    $footer->writeText($content, $fontFooter, $formatFooter);
  }

  function setCoversheet($doc) {
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
    $content = strtoupper($doc['document']['title']);
    $section->writeText($content, $fontHeaderBold, $formatParaCenter);
    $section->writeText('<br><br><br>');

    $content = 'This Agreement is based on the model Cooperative Research and Development Agreement ("CRADA”) adopted by the U.S. Public Health Service (“PHS”) Technology Transfer Policy Board for use by components of the National Institutes of Health (“NIH”), the Centers for Disease Control and Prevention (“CDC”), and the Food and Drug Administration (“FDA”), which are agencies of the PHS within the Department of Health and Human Services (“HHS”).';
    $section->writeText($content, $this->fontBody, $formatPara);
    //$section->insertPageBreak();
  }

  function setBody($doc) {
    $this->bodySection = $this->addSection();


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
    /*
    $section_number = 1;
    $this->setSection($section_number, "Definitions");
    for($i=0;$i<count($doc['clauses']);$i++) {
      if($doc['clauses'][$i]['section'] == "Definitions") {
        $this->bodySection->writeText($doc['clauses'][$i]['text']."\n", $this->fontBody, $formatDefinition);
      }
    }
    */
    /* Print Clauses */
    $current_section = "";
    $section_number = 0;
    for($i=0;$i<count($doc['clauses']);$i++) {
      //if($doc['clauses'][$i]['section'] != "Definitions") {
        if($doc['clauses'][$i]['section'] != $current_section) {
          //Add Section
          $section_number++;
          $current_section = $doc['clauses'][$i]['section'];
          $this->bodySection->writeText("<b>".$section_number.".  ".$current_section."</b>\n", $this->fontBody, $this->formatPara);
          $current_subsection = 1;
        }
        //
        //Remove <br> from text
        //
        $clause_text = preg_replace('#<br\s*/?>#i', "", $doc['clauses'][$i]['text']);

        if($current_section == "Definitions") {
          $this->bodySection->writeText($clause_text."\n", $this->fontBody, $formatClause);
        } else {
          $this->bodySection->writeText($section_number.".".$current_subsection."   ".$clause_text."\n", $this->fontBody, $formatClause);

        }
        /*
        if($section_number ==1 and $current_subsection ==7 ) {
          crada_log("Section 3-3");
          crada_log($doc['clauses'][$i]['text']);
          //Copy preg_replace from above.  This one wouldn't comment out properly.
          $clause = preg_replace('#<br\s * /?>#i', "", $doc['clauses'][$i]['text']);
          crada_log("new one");
          crada_log($clause);
        }
        */
        $current_subsection++;
      //}
    }
    //
    //Add survivability
    //
    if(($survivability_statement = getSurvivabilityStatement($doc)) != "") {
      $this->bodySection->writeText($survivability_statement."\n", $this->fontBody, $formatClause);
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



_______________________________________      _________________
Signature                                      Date

Typed Name: <b>[IC]</b>
Title: <b>[IC Title]</b>



FOR COLLABORATOR:



_______________________________________      _________________
Signature                                      Date


Typed Name: <b>[Collaborator]</b>
Title: <b>[Collaborator Title]</b>
';
    $this->bodySection->writeText($content, $this->fontBody, $this->formatPara);

  }

  private function setTitle($title) {
    $formatParaCenter = new PHPRtfLite_ParFormat(PHPRtfLite_ParFormat::TEXT_ALIGN_CENTER);

    $this->bodySection->writeText($title, $this->fontBody, $formatParaCenter);
    $this->bodySection->writeText('<br>');
  }

  private function setSection($section_number, $title) {
    $this->bodySection->writeText("<b>".$section_number.".  ".$title."</b>", $this->fontBody, $this->formatPara);
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

// Extend the TCPDF class to create custom Header and Footer
class CradaPdf extends TCPDF {

	private $footer_title = "Footer Title";
  private $_font = 'times';
  private $_fontSize = 15;
  private $_fontSize_footer = 12;

	function __construct() {
		parent::__construct();
		crada_log("In CradaPdf Class constructor");
    $this->SetFont($this->_font, '', $this->_fontSize);


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
        $this->SetFont($this->_font, '', $this->_fontSize_footer);
        // Position at 20 mm from bottom
        $this->SetY(-20);
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
    $this->SetFont($this->_font, '', $this->_fontSize);
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
			$this->_writeHTML('<br>'.$survivability_statement);
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
Title:&nbsp;&nbsp;<b>[IC Title]</b><br>
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
Title:&nbsp;&nbsp;<b>[Collaborator Title]</b><br>
';
		$this->_writeHTML($content);

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


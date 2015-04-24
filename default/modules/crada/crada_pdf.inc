<?php

require_once 'sites/default/libraries/tcpdf/tcpdf.php';

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
____________________________________&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;____________________<br>
Signature&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Date<br>
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
____________________________________&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;____________________<br>
Signature&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Date<br>
<br>
Typed Name:&nbsp;&nbsp;<b>[Collaborator]</b><br>
Title:&nbsp;&nbsp;<b>[Collaborator Title]</b><br>
';
    $this->_writeHTML($content);

  }

}

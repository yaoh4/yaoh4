<?php

require_once 'sites/default/libraries/PHPRtfLite/PHPRtfLite.php';

class CradaRtf extends PHPRtfLite {

  private $_fontFamily = 'Times New Roman';
  private $_fontSizeSmall = CRADA_FONT_SMALL;
  private $_fontSizeMedium = CRADA_FONT_MEDIUM;
  private $_fontSizeLarge = CRADA_FONT_LARGE;
  private $_fontColor = CRADA_FONT_COLOR;

  private $fontBody;
  private $fontBodyBold;
  private $fontLarge;
  private $fontLargeBold;
  private $fontNormal;
  private $fontNormalBold;
  private $formatPara;
  private $section;

  private $left;
  private $center;
  private $right;

  function __construct() {
    parent::__construct();
    crada_log("In CradaRtf SubClass constructor");
    $this->setMargins(1, 1, 1, 1);
    $this->setPaperFormat(PHPRtfLite::PAPER_LETTER);
    // FONTS Defined
    $this->fontSmall = new PHPRtfLite_Font($this->_fontSizeSmall, $this->_fontFamily, $this->_fontColor);
    $this->fontSmallBold = new PHPRtfLite_Font($this->_fontSizeSmall, $this->_fontFamily, $this->_fontColor);
    $this->fontSmallBold->setBold();

    $this->fontNormal = new PHPRtfLite_Font($this->_fontSizeMedium, $this->_fontFamily, $this->_fontColor);
    $this->fontNormalBold = new PHPRtfLite_Font($this->_fontSizeMedium, $this->_fontFamily, $this->_fontColor);
    $this->fontNormalBold->setBold();

    $this->fontLarge = new PHPRtfLite_Font($this->_fontSizeLarge, $this->_fontFamily, $this->_fontColor);
    $this->fontLargeBold = new PHPRtfLite_Font($this->_fontSizeLarge, $this->_fontFamily, $this->_fontColor);
    $this->fontLargeBold->setBold();

    $this->fontBody = new PHPRtfLite_Font($this->_fontSizeMedium, $this->_fontFamily, $this->_fontColor);
    $this->fontBodyBold = new PHPRtfLite_Font($this->_fontSizeMedium, $this->_fontFamily, $this->_fontColor);

    $this->formatPara = new PHPRtfLite_ParFormat();

    $this->left = new PHPRtfLite_ParFormat(PHPRtfLite_ParFormat::TEXT_ALIGN_LEFT);
    $this->center = new PHPRtfLite_ParFormat(PHPRtfLite_ParFormat::TEXT_ALIGN_CENTER);
    $this->right = new PHPRtfLite_ParFormat(PHPRtfLite_ParFormat::TEXT_ALIGN_RIGHT);
  }

  function setHeader($text) {
    $header = $this->addHeader();
    $content = "\scaps ".$text;
    $header->writeRtfCode($content, $this->fontNormalBold, $this->center);
  }

  function setFooter($document) {
    $footer = $this->addFooter();
    $table = $footer->addTable();
    $table->addRows(2);
    $table->addColumnsList(array(2.86, 2.86, 2.86));

    $content = $document['base_template'];
    $table->writeToCell(1, 1, $content, $this->fontSmall, $this->left);

    $table->writeToCell(1, 2, 'Ref. No. _______', $this->fontSmall, $this->center);
    $content = 'Page <pagenum> of <pagetotal>';
    $table->writeToCell(2, 1, $content, $this->fontSmall, $this->left);
    $table->writeToCell(2, 3, '<i>Confidential</i>', $this->fontSmall, $this->right);
  }

  private function setProperties() {
    $content = '\info{\title The Panda Thumb}{\author Stephen J Gould}{\keywords
science natural history }}';

  }
  private function setSectionTitle($section_title) {
    crada_log("SETTING ..." + $section_title);
    $this->section->writeText('<br>');
    $this->section->writeText($section_title, $this->fontLargeBold, $this->center);
  }

  function setCoversheet($page_title, $document) {

    crada_log("setCoversheet");
    // FORMAT Defined
    $formatPara = new PHPRtfLite_ParFormat();
    $formatParaCenter = new PHPRtfLite_ParFormat(PHPRtfLite_ParFormat::TEXT_ALIGN_CENTER);
    $formatParaLeft = new PHPRtfLite_ParFormat(PHPRtfLite_ParFormat::TEXT_ALIGN_LEFT);

  /* Add Cover Page */
    $this->section =  $this->addSection();
    $this->setSectionTitle($page_title);

    $content = "<br><br><br>";
    $this->section->writeText($content, $this->fontNormal, $this->left);
    $content = "This Cover Page identifies the Parties to this CRADA:<br>";
    $this->section->writeText($content, $this->fontNormal);
$content = "The U.S. Department of Health and Human Services, as represented by<br>";
    $this->section->writeText($content, $this->fontNormal, $this->center);
$content = "<b>[INSERT the full name of the ICD]</b><br>";
    $this->section->writeText($content, $this->fontNormal);
$content = "an Institute, Center, or Division (hereinafter referred to as the “ICD”) of the <br>";
    $this->section->writeText($content, $this->fontNormal);
$content = "<b>[INSERT as appropriate: NIH, CDC, or FDA]</b><br><br>";
    $this->section->writeText($content, $this->fontNormal);
$content = "and<br><br>";
    $this->section->writeText($content, $this->fontNormal);
$content = "<b>[INSERT Collaborator’s official name]</b>,<br>";
    $this->section->writeText($content, $this->fontNormal);
$content = 'hereinafter referred to as the “Collaborator”,<br>';
    $this->section->writeText($content, $this->fontNormal);
$content = "created and operating under the laws of <b>[INSERT State of Incorporation]</b>.<br>";
    $this->section->writeText($content, $this->fontNormal);
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
          $this->section->writeText("<b>".$section_number.".  ".$current_section."</b>\n", $this->fontBody, $this->formatPara);
          $current_subsection = 1;
        }
        //
        //Remove <br> from text
        //
        $clause_text = preg_replace('#<br\s*/?>#i', "", $doc['clauses'][$i]['text']);

        if($current_section == "Definitions") {
          $this->section->writeText($clause_text."\n", $this->fontBody, $formatClause);
        } else {
          $this->section->writeText($section_number.".".$current_subsection."   ".$clause_text."\n", $this->fontBody, $formatClause);

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
      $this->section->writeText($survivability_statement."\n", $this->fontBody, $formatClause);
    }

    $this->section->writeText("\n\n\nSIGNATURES BEGIN ON THE NEXT PAGE", $this->fontBody, new PHPRtfLite_ParFormat(PHPRtfLite_ParFormat::TEXT_ALIGN_CENTER));
    $this->section->insertPageBreak();

  }


  function setSignature($page_title, $section_title, $section_statement) {
    // FONTS Defined
    $this->section = $this->addSection();
    $this->setSectionTitle($title);

    $fontHeader = new PHPRtfLite_Font(14, $this->_font, $this->fontColor);
    $fontHeaderBold = new PHPRtfLite_Font(14, $this->_font, $this->fontColor);
    // FORMAT Defined
    $formatPara = new PHPRtfLite_ParFormat();
    $formatParaCenter = new PHPRtfLite_ParFormat(PHPRtfLite_ParFormat::TEXT_ALIGN_CENTER);
    $content= 'SIGNATURE PAGE';
    $this->section->writeText("\n".$content."\n\n", $fontHeader, $formatParaCenter);

    $content= '<b>'.$section_title.'</b>';
    $this->section->writeText($content."\n", $this->fontBody, $formatParaCenter);

    $content = "\scaps ".$section_statement;
    $this->section->writeRtfCode($content, $this->fontNormal, $this->left);

$content = '


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
    $this->section->writeText($content, $this->fontBody, $this->formatPara);
    $this->section->insertPageBreak();

  }
  function setContacts() {

  }
  function setSummary() {

  }
  function setAppendix() {

  }

  private function setTitle($title) {
    $formatParaCenter = new PHPRtfLite_ParFormat(PHPRtfLite_ParFormat::TEXT_ALIGN_CENTER);

    $this->section->writeText($title, $this->fontBody, $formatParaCenter);
    $this->section->writeText('<br>');
  }

  private function setSection($section_number, $title) {
    $this->section->writeText("<b>".$section_number.".  ".$title."</b>", $this->fontBody, $this->formatPara);
    $this->section->writeText('<br>');
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

  private function explodeLegalArray($arrayOfEntities, $delimiter=",") {
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


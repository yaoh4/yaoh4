-- phpMyAdmin SQL Dump
-- version 4.1.6
-- http://www.phpmyadmin.net
--
-- Host: 127.0.0.1
-- Generation Time: Sep 04, 2014 at 08:20 PM
-- Server version: 5.6.16
-- PHP Version: 5.5.9

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `nci_crada`
--


-- --------------------------------------------------------

--
-- Table structure for table `crada_demographics`
--

CREATE TABLE IF NOT EXISTS `crada_demographics` (
  `document_id` int(12) NOT NULL,
  `variable` varchar(50) NOT NULL,
  `question` text NOT NULL,
  `type` varchar(10) NOT NULL,
  PRIMARY KEY (`document_id`,`variable`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `crada_demographics_pulldown_options`
--

CREATE TABLE IF NOT EXISTS `crada_demographics_pulldown_options` (
  `document_id` int(12) NOT NULL,
  `variable` varchar(50) NOT NULL,
  `pulldown_option` varchar(50) NOT NULL,
  PRIMARY KEY (`document_id`,`variable`,`pulldown_option`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;


--
-- Table structure for table `crada_annotations`
--

CREATE TABLE IF NOT EXISTS `crada_annotations` (
  `annotation_id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `document_id` int(12) NOT NULL,
  `document_element_id` int(12) NOT NULL,
  `document_version` int(12) NOT NULL,
  `section` varchar(50) NOT NULL,
  `annotation_text` longtext NOT NULL,
  `updated_by` varchar(50) NOT NULL,
  `updated_date` datetime NOT NULL,
  PRIMARY KEY (`annotation_id`),
  UNIQUE KEY `document_id` (`document_id`,`document_element_id`,`document_version`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `crada_answer`
--

CREATE TABLE IF NOT EXISTS `crada_answer` (
  `document_id` int(12) NOT NULL,
  `question_id` int(12) NOT NULL,
  `answer_id` int(12) NOT NULL,
  `answer_text` text NOT NULL,
  PRIMARY KEY (`document_id`,`question_id`,`answer_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `crada_clause_group`
--

CREATE TABLE IF NOT EXISTS `crada_clause_group` (
  `group_id` int(12) NOT NULL AUTO_INCREMENT,
  `section` varchar(50) NOT NULL,
  `description` text NOT NULL,
  PRIMARY KEY (`group_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `crada_clause_group_to_clause`
--

CREATE TABLE IF NOT EXISTS `crada_clause_group_to_clause` (
  `clause_group_id` int(12) NOT NULL,
  `document_id` int(12) NOT NULL,
  `version` int(12) NOT NULL,
  `clause_id` int(12) NOT NULL,
  PRIMARY KEY (`clause_group_id`,`document_id`,`version`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `crada_definition`
--

CREATE TABLE IF NOT EXISTS `crada_definition` (
  `document_id` int(12) NOT NULL,
  `term` varchar(100) NOT NULL,
  `definition` text NOT NULL,
  PRIMARY KEY (`document_id`,`term`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `crada_definition_element_crosslink`
--

CREATE TABLE IF NOT EXISTS `crada_definition_element_crosslink` (
  `document_id` int(12) NOT NULL,
  `clause_id` int(12) NOT NULL,
  `term` varchar(100) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Table structure for table `crada_document`
--

CREATE TABLE IF NOT EXISTS `crada_document` (
  `document_id` int(12) NOT NULL AUTO_INCREMENT,
  `is_master` tinyint(1) NOT NULL,
  `document_name` varchar(50) NOT NULL,
  `title` text NOT NULL,
  PRIMARY KEY (`document_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `crada_document_element`
--

CREATE TABLE IF NOT EXISTS `crada_document_element` (
  `document_element_id` int(12) NOT NULL,
  `document_version` int(12) NOT NULL,
  `document_id` int(12) NOT NULL,
  `section` varchar(50) NOT NULL,
  `location` int(4) NOT NULL,
  `alternate_text_type` varchar(50) NOT NULL,
  `document_element_text` longtext NOT NULL,
  `survivable` tinyint(1) NOT NULL,
  `required` tinyint(1) NOT NULL,
  `updated_by` varchar(50) NOT NULL,
  `updated_date` datetime NOT NULL,
  PRIMARY KEY (`document_element_id`,`document_version`,`document_id`),
  KEY `idx_document_element_id` (`document_element_id`),
  KEY `idx_document_version` (`document_version`),
  KEY `idx_document_id` (`document_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `crada_document_version`
--

CREATE TABLE IF NOT EXISTS `crada_document_version` (
  `document_id` int(12) NOT NULL,
  `version` int(12) NOT NULL,
  `updated_by` varchar(50) NOT NULL,
  `updated_date` datetime NOT NULL,
  PRIMARY KEY (`document_id`,`version`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `crada_question`
--

CREATE TABLE IF NOT EXISTS `crada_question` (
  `document_id` int(12) NOT NULL,
  `question_id` int(12) NOT NULL AUTO_INCREMENT,
  `section` varchar(50) NOT NULL,
  `predecessor` int(12) NOT NULL,
  `question_text` text NOT NULL,
  PRIMARY KEY (`question_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `crada_section`
--

CREATE TABLE IF NOT EXISTS `crada_section` (
  `location` int(12) NOT NULL,
  `shorthand` varchar(50) NOT NULL,
  `name` text NOT NULL,
  PRIMARY KEY (`location`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `crada_valid_clause_group_by_answer`
--

CREATE TABLE IF NOT EXISTS `crada_valid_clause_group_by_answer` (
  `document_id` int(12) NOT NULL,
  `question_id` int(12) NOT NULL,
  `answer_id` int(12) NOT NULL,
  `clause_group_id` int(12) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;

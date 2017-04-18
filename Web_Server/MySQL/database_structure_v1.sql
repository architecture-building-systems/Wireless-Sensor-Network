-- phpMyAdmin SQL Dump
-- version 3.5.7
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Apr 18, 2017 at 02:38 PM
-- Server version: 5.1.73-0ubuntu0.10.04.1
-- PHP Version: 5.3.10-1ubuntu3.8

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `ddr`
--

-- --------------------------------------------------------

--
-- Table structure for table `wsn_input`
--

CREATE TABLE IF NOT EXISTS `wsn_input` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `time_unix` datetime NOT NULL,
  `node_id` float NOT NULL,
  `sensorModuleType` float NOT NULL,
  `value0` float NOT NULL,
  `value1` float NOT NULL,
  `value2` float NOT NULL,
  `value3` float NOT NULL,
  `value4` float NOT NULL,
  `ip` text COLLATE utf8_unicode_ci NOT NULL,
  `gateway_id` int(11) NOT NULL,
  `comment` text COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=2291115 ;

-- --------------------------------------------------------

--
-- Table structure for table `wsn_input_raw`
--

CREATE TABLE IF NOT EXISTS `wsn_input_raw` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `payload` text COLLATE utf8_unicode_ci NOT NULL,
  `comment` text COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=2291260 ;

-- --------------------------------------------------------

--
-- Table structure for table `wsn_measurement_campaign`
--

CREATE TABLE IF NOT EXISTS `wsn_measurement_campaign` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `description` text COLLATE utf8_unicode_ci NOT NULL,
  `start_date` datetime NOT NULL DEFAULT '0001-01-01 00:00:00',
  `end_date` datetime NOT NULL DEFAULT '0001-01-01 00:00:00',
  `gateway_id` int(11) NOT NULL DEFAULT '-1',
  `node_id_list` text COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=11 ;

-- --------------------------------------------------------

--
-- Table structure for table `wsn_sensor_module_type`
--

CREATE TABLE IF NOT EXISTS `wsn_sensor_module_type` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `description` text COLLATE utf8_unicode_ci NOT NULL,
  `number_of_values` int(11) NOT NULL,
  `unit` text COLLATE utf8_unicode_ci NOT NULL,
  `entity` text COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=13 ;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;

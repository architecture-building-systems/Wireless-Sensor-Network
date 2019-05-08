-- phpMyAdmin SQL Dump
-- version 4.8.5
-- https://www.phpmyadmin.net/
--
-- Host: localhost:3306
-- Generation Time: May 08, 2019 at 02:01 PM
-- Server version: 10.3.14-MariaDB-1:10.3.14+maria~bionic
-- PHP Version: 7.3.4-1+ubuntu18.04.1+deb.sury.org+3

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `ddr`
--
CREATE DATABASE IF NOT EXISTS `ddr` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
USE `ddr`;

-- --------------------------------------------------------

--
-- Table structure for table `wsn_access`
--

DROP TABLE IF EXISTS `wsn_access`;
CREATE TABLE `wsn_access` (
  `id` int(11) NOT NULL,
  `user` text COLLATE utf8_unicode_ci NOT NULL,
  `token` text COLLATE utf8_unicode_ci NOT NULL,
  `read` tinyint(1) NOT NULL,
  `insert` tinyint(1) NOT NULL,
  `campaign_id` text COLLATE utf8_unicode_ci NOT NULL,
  `comment` text COLLATE utf8_unicode_ci NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `wsn_input`
--

DROP TABLE IF EXISTS `wsn_input`;
CREATE TABLE `wsn_input` (
  `id` int(11) NOT NULL,
  `time` timestamp NOT NULL DEFAULT current_timestamp(),
  `node_id` int(11) NOT NULL,
  `sensorModuleType` int(11) NOT NULL,
  `value0` float NOT NULL DEFAULT 0,
  `value1` float NOT NULL DEFAULT 0,
  `value2` float NOT NULL DEFAULT 0,
  `value3` float NOT NULL DEFAULT 0,
  `value4` float NOT NULL DEFAULT 0,
  `ip` text COLLATE utf8_unicode_ci NOT NULL DEFAULT '-1',
  `gateway_id` int(11) NOT NULL DEFAULT -1,
  `comment` text COLLATE utf8_unicode_ci DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `wsn_input_raw`
--

DROP TABLE IF EXISTS `wsn_input_raw`;
CREATE TABLE `wsn_input_raw` (
  `id` int(11) NOT NULL,
  `time` timestamp NOT NULL DEFAULT current_timestamp(),
  `payload` text COLLATE utf8_unicode_ci NOT NULL,
  `comment` text COLLATE utf8_unicode_ci DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `wsn_measurement_campaign`
--

DROP TABLE IF EXISTS `wsn_measurement_campaign`;
CREATE TABLE `wsn_measurement_campaign` (
  `id` int(11) NOT NULL,
  `campaign_name` text COLLATE utf8_unicode_ci NOT NULL,
  `description` text COLLATE utf8_unicode_ci NOT NULL,
  `start_date` datetime NOT NULL DEFAULT '0001-01-01 00:00:00',
  `end_date` datetime NOT NULL DEFAULT '0001-01-01 00:00:00',
  `gateway_id` int(11) NOT NULL DEFAULT -1,
  `node_id_list` text COLLATE utf8_unicode_ci NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `wsn_sensor_module_type`
--

DROP TABLE IF EXISTS `wsn_sensor_module_type`;
CREATE TABLE `wsn_sensor_module_type` (
  `id` int(11) NOT NULL,
  `description` text COLLATE utf8_unicode_ci NOT NULL,
  `number_of_values` int(11) NOT NULL,
  `unit` text COLLATE utf8_unicode_ci NOT NULL,
  `entity` text COLLATE utf8_unicode_ci NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Indexes for dumped tables
--

--
-- Indexes for table `wsn_access`
--
ALTER TABLE `wsn_access`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `wsn_input`
--
ALTER TABLE `wsn_input`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `id` (`id`),
  ADD KEY `time` (`time`),
  ADD KEY `node_id` (`node_id`),
  ADD KEY `IDX_NODE_ID_AND_TIME` (`time`,`node_id`);

--
-- Indexes for table `wsn_input_raw`
--
ALTER TABLE `wsn_input_raw`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `id` (`id`);

--
-- Indexes for table `wsn_measurement_campaign`
--
ALTER TABLE `wsn_measurement_campaign`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `wsn_sensor_module_type`
--
ALTER TABLE `wsn_sensor_module_type`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `id` (`id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `wsn_access`
--
ALTER TABLE `wsn_access`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `wsn_input`
--
ALTER TABLE `wsn_input`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `wsn_input_raw`
--
ALTER TABLE `wsn_input_raw`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `wsn_measurement_campaign`
--
ALTER TABLE `wsn_measurement_campaign`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `wsn_sensor_module_type`
--
ALTER TABLE `wsn_sensor_module_type`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;

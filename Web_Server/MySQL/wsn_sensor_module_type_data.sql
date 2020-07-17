-- phpMyAdmin SQL Dump
-- version 4.9.4
-- https://www.phpmyadmin.net/
--
-- Host: localhost:3306
-- Generation Time: Jul 17, 2020 at 10:51 AM
-- Server version: 10.3.23-MariaDB-1:10.3.23+maria~bionic
-- PHP Version: 7.3.19-1+ubuntu18.04.1+deb.sury.org+1

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

--
-- Dumping data for table `wsn_sensor_module_type`
--

INSERT INTO `wsn_sensor_module_type` (`id`, `description`, `number_of_values`, `unit`, `entity`) VALUES
(1, 'test', 3, 'unit1, unit2, unit3', 'test1, test2, test3'),
(2, 'SHT31', 2, '°C,%', 'air temperature (indoor), relative humidity (indoor)'),
(3, 'SHT15', 2, '°C,%', 'air temperature (indoor), relative humidity (indoor)'),
(4, 'TSL2561', 1, 'Lux', 'luminosity'),
(5, 'Window', 1, 's', 'windows opening time'),
(6, 'S8 - CO2', 1, 'ppm', 'co_2 concentration'),
(7, 'Heat flux', 1, 'W/m2', 'heat flux'),
(8, '2xDS18B20', 2, '°C,°C', 'temperature, temperature'),
(9, 'Pulse [Wh]', 1, 'Wh', 'electricity consumption'),
(10, 'Pulse [liter]', 1, 'l', 'oil consumption'),
(17, 'Pulse [l]', 1, 'l', 'water flow'),
(-1, 'Low battery warning', 1, 'V', 'battery voltage'),
(12, 'U-Value', 4, 'W/m2,°C,°C,°C', 'heat flux, air temperature (indoor), temperature (outdoor), temperature (surface)'),
(-2, 'No Data', 0, 'no data', 'no data'),
(13, 'Low battery voltage warning', 1, 'V', 'battery voltage (warning)'),
(16, 'Pulse [m3]', 1, 'm3', 'gas consumption'),
(18, 'Analog in [l/min]', 1, 'l/min', 'water flow rate'),
(19, 'Door Opening', 1, '-', 'door opening state'),
(20, 'Heartbeat', 2, 'V, dBm', 'battery voltage, RSSI'),
(21, 'Analog input [mA]', 1, 'mA', 'current');
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;

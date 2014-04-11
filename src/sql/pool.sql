-- phpMyAdmin SQL Dump
-- version 3.5.8.1deb1
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Jan 21, 2014 at 01:52 PM
-- Server version: 5.5.34-0ubuntu0.13.04.1-log
-- PHP Version: 5.4.9-4ubuntu2.4

SET FOREIGN_KEY_CHECKS=0;
SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT=0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `pool`
--

-- --------------------------------------------------------

--
-- Table structure for table `blocks`
--

DROP TABLE IF EXISTS `blocks`;
CREATE TABLE IF NOT EXISTS `blocks` (
  `id` int(32) NOT NULL AUTO_INCREMENT,
  `hash` varchar(256) NOT NULL,
  `height` int(32) NOT NULL,
  `time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `value` double NOT NULL,
  `confirmations` int(11) NOT NULL DEFAULT '0',
  `payed` tinyint(1) NOT NULL DEFAULT '0',
  `orphan` tinyint(1) NOT NULL DEFAULT '0',
  `difficult` double NOT NULL,
  PRIMARY KEY (`id`),
  KEY `time` (`time`),
  KEY `hash` (`hash`(255)),
  KEY `hash_2` (`hash`(255)),
  KEY `confirmations` (`confirmations`),
  KEY `confirmations_2` (`confirmations`),
  KEY `height` (`height`),
  KEY `orphan` (`orphan`),
  KEY `payed` (`payed`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=16 ;

-- --------------------------------------------------------

--
-- Table structure for table `cointypes`
--

DROP TABLE IF EXISTS `cointypes`;
CREATE TABLE IF NOT EXISTS `cointypes` (
  `id` int(4) NOT NULL AUTO_INCREMENT,
  `coinname` varchar(32) NOT NULL,
  `coinshortname` varchar(5) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `id` (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=2 ;

-- --------------------------------------------------------

--
-- Table structure for table `fee`
--

DROP TABLE IF EXISTS `fee`;
CREATE TABLE IF NOT EXISTS `fee` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `block` int(11) NOT NULL,
  `amout` double NOT NULL,
  PRIMARY KEY (`id`),
  KEY `block` (`block`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Table structure for table `instant_payout_queue`
--

DROP TABLE IF EXISTS `instant_payout_queue`;
CREATE TABLE IF NOT EXISTS `instant_payout_queue` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `user` int(11) NOT NULL,
  `cointype` int(4) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `user` (`user`),
  KEY `cointype` (`cointype`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_bin AUTO_INCREMENT=33 ;

-- --------------------------------------------------------

--
-- Table structure for table `journal`
--

DROP TABLE IF EXISTS `journal`;
CREATE TABLE IF NOT EXISTS `journal` (
  `id` bigint(32) NOT NULL AUTO_INCREMENT,
  `block` int(32) DEFAULT NULL,
  `share` bigint(20) DEFAULT NULL,
  `user` int(32) DEFAULT NULL,
  `value` double NOT NULL,
  `time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `user` (`user`),
  KEY `FK_block` (`block`),
  KEY `FK_share` (`share`),
  KEY `user_2` (`user`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 COLLATE=utf8_bin AUTO_INCREMENT=353242 ;

-- --------------------------------------------------------

--
-- Table structure for table `shares`
--

DROP TABLE IF EXISTS `shares`;
CREATE TABLE IF NOT EXISTS `shares` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `share` varchar(128) NOT NULL,
  `share_length` int(3) NOT NULL,
  `share_value` double NOT NULL,
  `worker_id` int(32) DEFAULT NULL,
  `time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `user_id` int(32) NOT NULL,
  `cointype` int(4) NOT NULL,
  `blockheight` int(32) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `FK_usershare` (`user_id`),
  KEY `FK_workershare` (`worker_id`),
  KEY `fk_cointype` (`cointype`),
  KEY `share` (`share`),
  KEY `time` (`time`),
  KEY `user_id` (`user_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=112441 ;

-- --------------------------------------------------------

--
-- Table structure for table `users`
--

DROP TABLE IF EXISTS `users`;
CREATE TABLE IF NOT EXISTS `users` (
  `id` int(32) NOT NULL AUTO_INCREMENT,
  `name` varchar(32) NOT NULL,
  `password` char(64) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `id` (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=775 ;

-- --------------------------------------------------------

--
-- Table structure for table `wallet`
--

DROP TABLE IF EXISTS `wallet`;
CREATE TABLE IF NOT EXISTS `wallet` (
  `user_id` int(32) NOT NULL,
  `cointype` int(4) NOT NULL,
  `amount` double NOT NULL,
  `payoutadress` varchar(64) NOT NULL,
  UNIQUE KEY `user_id` (`user_id`),
  KEY `FK_cointocoin` (`cointype`),
  KEY `user_id_2` (`user_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `worker`
--

DROP TABLE IF EXISTS `worker`;
CREATE TABLE IF NOT EXISTS `worker` (
  `id` int(32) NOT NULL AUTO_INCREMENT,
  `user_id` int(32) NOT NULL,
  `name` varchar(32) NOT NULL,
  `cointype` int(4) NOT NULL,
  `value_per_hour` varchar(32) DEFAULT NULL,
  `pass` char(64) NOT NULL,
  `online` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`),
  KEY `fk_worker_to_coin` (`cointype`),
  KEY `fk_workeruser` (`user_id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1335 ;

--
-- Constraints for dumped tables
--

--
-- Constraints for table `fee`
--
ALTER TABLE `fee`
  ADD CONSTRAINT `fee_ibfk_1` FOREIGN KEY (`block`) REFERENCES `blocks` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `instant_payout_queue`
--
ALTER TABLE `instant_payout_queue`
  ADD CONSTRAINT `instant_payout_queue_ibfk_1` FOREIGN KEY (`user`) REFERENCES `users` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `instant_payout_queue_ibfk_2` FOREIGN KEY (`cointype`) REFERENCES `cointypes` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `journal`
--
ALTER TABLE `journal`
  ADD CONSTRAINT `FK_block` FOREIGN KEY (`block`) REFERENCES `blocks` (`id`) ON DELETE SET NULL ON UPDATE SET NULL,
  ADD CONSTRAINT `FK_share` FOREIGN KEY (`share`) REFERENCES `shares` (`id`) ON DELETE SET NULL ON UPDATE SET NULL,
  ADD CONSTRAINT `FK_user` FOREIGN KEY (`user`) REFERENCES `users` (`id`) ON DELETE SET NULL ON UPDATE SET NULL;

--
-- Constraints for table `shares`
--
ALTER TABLE `shares`
  ADD CONSTRAINT `fk_cointype` FOREIGN KEY (`cointype`) REFERENCES `cointypes` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `FK_usershare` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `FK_workershare` FOREIGN KEY (`worker_id`) REFERENCES `worker` (`id`) ON DELETE SET NULL ON UPDATE CASCADE;

--
-- Constraints for table `wallet`
--
ALTER TABLE `wallet`
  ADD CONSTRAINT `FK_cointocoin` FOREIGN KEY (`cointype`) REFERENCES `cointypes` (`id`) ON UPDATE CASCADE,
  ADD CONSTRAINT `FK_usertouser` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `worker`
--
ALTER TABLE `worker`
  ADD CONSTRAINT `fk_workeruser` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `fk_worker_to_coin` FOREIGN KEY (`cointype`) REFERENCES `cointypes` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;
SET FOREIGN_KEY_CHECKS=1;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;

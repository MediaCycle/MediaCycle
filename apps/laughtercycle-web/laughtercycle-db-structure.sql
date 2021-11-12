-- phpMyAdmin SQL Dump
-- version 2.11.8.1deb1
-- http://www.phpmyadmin.net
--
-- Serveur: localhost
-- Généré le : Ven 29 Mai 2009 à 17:06
-- Version du serveur: 5.0.67
-- Version de PHP: 5.2.6-2ubuntu4.2

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Base de données: `numediart`
--

-- --------------------------------------------------------

--
-- Structure de la table `comments`
--

CREATE TABLE IF NOT EXISTS `comments` (
  `file_id` int(10) unsigned NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  `comment` text collate utf8_unicode_ci,
  `note` int(10) unsigned default NULL,
  `time` bigint(20) default NULL,
  UNIQUE KEY `file_id` (`file_id`,`user_id`),
  KEY `note` (`note`),
  KEY `time` (`time`),
  FULLTEXT KEY `comment` (`comment`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Structure de la table `feedback`
--

CREATE TABLE IF NOT EXISTS `feedback` (
  `file_id` int(10) unsigned NOT NULL,
  `fb_file_id` int(10) unsigned NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  `feedback` int(11) NOT NULL,
  `time` bigint(20) NOT NULL,
  UNIQUE KEY `file_id` (`file_id`,`fb_file_id`,`user_id`),
  KEY `feedback` (`feedback`),
  KEY `time` (`time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Structure de la table `files`
--

CREATE TABLE IF NOT EXISTS `files` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `title` varchar(255) collate utf8_unicode_ci NOT NULL,
  `path` varchar(1024) collate utf8_unicode_ci default NULL,
  `type` varchar(255) collate utf8_unicode_ci default NULL,
  `recording` varchar(255) collate utf8_unicode_ci NOT NULL,
  `geotag` int(10) unsigned default NULL,
  `uploader` int(10) unsigned default NULL,
  `upload_date` bigint(20) default NULL,
  `quality` blob,
  `mean_note` float default NULL,
  PRIMARY KEY  (`id`),
  KEY `title` (`title`),
  KEY `path` (`path`(333)),
  KEY `type` (`type`),
  KEY `recording` (`recording`),
  KEY `geotag` (`geotag`),
  KEY `uploader` (`uploader`),
  KEY `upload_date` (`upload_date`),
  KEY `mean_note` (`mean_note`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Structure de la table `filetags`
--

CREATE TABLE IF NOT EXISTS `filetags` (
  `file_id` int(10) unsigned NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  `tag_id` int(10) unsigned NOT NULL,
  `time` bigint(20) default NULL,
  UNIQUE KEY `file_id` (`file_id`,`user_id`,`tag_id`),
  KEY `time` (`time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Structure de la table `friends`
--

CREATE TABLE IF NOT EXISTS `friends` (
  `user_id` int(10) unsigned NOT NULL,
  `friend_id` int(10) unsigned NOT NULL,
  `friendship_begin` bigint(20) default NULL,
  `friendship_end` bigint(20) default NULL,
  UNIQUE KEY `user_id` (`user_id`,`friend_id`),
  KEY `friendship` (`friendship_begin`,`friendship_end`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Structure de la table `geotags`
--

CREATE TABLE IF NOT EXISTS `geotags` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `lat` decimal(10,6) NOT NULL default '0.000000',
  `lon` decimal(10,6) NOT NULL default '0.000000',
  `description` varchar(255) collate utf8_unicode_ci NOT NULL default '',
  PRIMARY KEY  (`id`),
  KEY `description` (`description`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Structure de la table `groups`
--

CREATE TABLE IF NOT EXISTS `groups` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `name` varchar(255) collate utf8_unicode_ci NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`name`),
  KEY `user_id` (`user_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Structure de la table `history`
--

CREATE TABLE IF NOT EXISTS `history` (
  `file_id` int(10) unsigned NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  `time` bigint(20) NOT NULL,
  KEY `file_id` (`file_id`,`user_id`,`time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Structure de la table `login`
--

CREATE TABLE IF NOT EXISTS `login` (
  `user_id` int(10) unsigned NOT NULL,
  `type` varchar(255) collate utf8_unicode_ci NOT NULL,
  `time` bigint(20) NOT NULL,
  KEY `user_id` (`user_id`,`type`,`time`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Structure de la table `tags`
--

CREATE TABLE IF NOT EXISTS `tags` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `name` varchar(255) collate utf8_unicode_ci NOT NULL,
  PRIMARY KEY  (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Structure de la table `usergroups`
--

CREATE TABLE IF NOT EXISTS `usergroups` (
  `group_id` int(10) unsigned NOT NULL,
  `user_id` int(10) unsigned NOT NULL,
  `joining` bigint(20) default NULL,
  `leaving` bigint(20) default NULL,
  UNIQUE KEY `group_id` (`group_id`,`user_id`),
  KEY `dates` (`joining`,`leaving`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Structure de la table `users`
--

CREATE TABLE IF NOT EXISTS `users` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `name` varchar(255) collate utf8_unicode_ci NOT NULL,
  `password` char(40) collate utf8_unicode_ci NOT NULL,
  `salt` char(40) collate utf8_unicode_ci NOT NULL,
  `token` char(40) collate utf8_unicode_ci NOT NULL,
  `email` varchar(255) collate utf8_unicode_ci default NULL,
  `avatar` varchar(255) collate utf8_unicode_ci default NULL,
  `description` text collate utf8_unicode_ci,
  `language` char(16) collate utf8_unicode_ci default NULL,
  `registration_date` bigint(20) NOT NULL,
  `last_login` bigint(20) NOT NULL,
  `geotag` int(10) unsigned default NULL,
  `is_admin` tinyint(1) default '0',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `username` (`name`),
  KEY `language` (`language`),
  KEY `registration_date` (`registration_date`),
  KEY `geotag` (`geotag`),
  KEY `is_admin` (`is_admin`),
  FULLTEXT KEY `description` (`description`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=5 ;


DROP TABLE IF EXISTS `player`;
CREATE TABLE `player` (
  `id` int(11) unsigned NOT NULL,
  `name` varchar(255) NOT NULL,
  `lv` int(10) unsigned NOT NULL,
  `sex` int(11) NOT NULL,
  `data` blob NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id_UNIQUE` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS `chat`;
CREATE TABLE `chat` (
  `id` int(11) unsigned NOT NULL,
  `channel` tinyint(3) unsigned NOT NULL,
  `data` blob NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id_UNIQUE` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS `account`;
CREATE TABLE `account` (
	`id` int(11) unsigned NOT NULL AUTO_INCREMENT,
	`uid` varchar(255) NOT NULL,
	`channel` varchar(255) NOT NULL,
	`os` varchar(255) NOT NULL,
	`admin_lv` tinyint(3) NOT NULL,
	PRIMARY KEY (`id`),
	UNIQUE KEY `id_UNIQUE` (`id`),
	UNIQUE KEY `uid_UNIQUE` (`uid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
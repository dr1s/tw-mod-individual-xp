CREATE TABLE IF NOT EXISTS `individualxp` (
  `CharacterGUID` int(10) unsigned NOT NULL,
  `XPRate` float NOT NULL DEFAULT '1',
  PRIMARY KEY (`CharacterGUID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

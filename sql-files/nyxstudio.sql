--
-- Table structure for table `dropped_mvp_card_log` [Bad]
--

CREATE TABLE IF NOT EXISTS `dropped_mvp_card_log` (
`id` int NOT NULL UNIQUE AUTO_INCREMENT,
`account_id` int(11) NOT NULL,
`char_name` varchar(32) NOT NULL default '',
`mvp_id` int(11) NOT NULL,
`mvp_name` varchar(32) NOT NULL default '',
`card_id` int(11) NOT NULL,
`card_name` varchar(50) NOT NULL default '',
`drop_map` varchar(32) NOT NULL default '',
`drop_date` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
PRIMARY KEY (`id`)
) ENGINE=MyISAM;

--
-- Table structure for table `dropped_mini_boss_card_log` [Bad]
--

CREATE TABLE IF NOT EXISTS `dropped_mini_boss_card_log` (
`id` int NOT NULL UNIQUE AUTO_INCREMENT,
`account_id` int(11) NOT NULL,
`char_name` varchar(32) NOT NULL default '',
`mini_boss_id` int(11) NOT NULL,
`mini_boss_name` varchar(32) NOT NULL default '',
`card_id` int(11) NOT NULL,
`card_name` varchar(50) NOT NULL default '',
`drop_map` varchar(32) NOT NULL default '',
`drop_date` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
PRIMARY KEY (`id`)
) ENGINE=MyISAM;


--
----
--

CREATE TABLE `member_rank`  (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `account_id` int(20) NOT NULL,
  `member_point` int(11) NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 2 CHARACTER SET = tis620 COLLATE = tis620_thai_ci ROW_FORMAT = Dynamic;

SET FOREIGN_KEY_CHECKS = 1;

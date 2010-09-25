DELETE FROM `command` WHERE `name` = 'nameannounce';
INSERT INTO `command` VALUES
('nameannounce', 1, 'Syntax: .nameannounce $MessageToBroadcast\r\n\r\nSend a global message to all players online in chat log with sender\'s name.');

DELETE FROM `mangos_string` WHERE `entry` = 11001;
INSERT INTO `mangos_string` VALUES
(11001, '|cffff0000[%s]:|cffffff00 %s|r',NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);

DELETE FROM `module_string` WHERE `module` = 'tw-mod-individual-xp';

INSERT INTO `module_string` (`module`, `id`, `content_default`) VALUES
('tw-mod-individual-xp', 100000, 'This server is running the |cff4CFF00IndividualXpRate |rmodule.'),
('tw-mod-individual-xp', 100001, '[XP] The Individual XP module is deactivated.'),
('tw-mod-individual-xp', 100002, '|cffffffff[XP] Your current XP rate is %g.|r'),
('tw-mod-individual-xp', 100003, '|cffffffff[XP] The maximum rate limit is %g.|r'),
('tw-mod-individual-xp', 100004, '[XP] The minimum rate limit is 0.1.'),
('tw-mod-individual-xp', 100005, '[XP] You have updated your XP rate to %g.'),
('tw-mod-individual-xp', 100006, '[XP] You have restored your XP rate to the default value of %g.');

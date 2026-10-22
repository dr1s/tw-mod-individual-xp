DELETE FROM `module_string_locale` WHERE `module` = 'tw-mod-individual-xp';

INSERT INTO `module_string_locale` (`module`, `id`, `locale`, `content`) VALUES
('tw-mod-individual-xp', 100000, 6, 'Este servidor está ejecutando el módulo |cff4CFF00 mod-individual-xp.'),
('tw-mod-individual-xp', 100000, 7, 'Este servidor está ejecutando el módulo |cff4CFF00 mod-individual-xp.'),
('tw-mod-individual-xp', 100001, 6, '[XP] El módulo XP individual está desactivado.'),
('tw-mod-individual-xp', 100001, 7, '[XP] El módulo XP individual está desactivado.'),
('tw-mod-individual-xp', 100002, 6, '|cffffffff[XP] Su experiencia actual es %g.|r'),
('tw-mod-individual-xp', 100002, 7, '|cffffffff[XP] su experiencia actual es %g.|r'),
('tw-mod-individual-xp', 100003, 6, '|cffffffff[XP] El límite máximo de XP es %g.|r'),
('tw-mod-individual-xp', 100003, 7, '|cffffffff[XP] El límite máximo de XP es %g.|r'),
('tw-mod-individual-xp', 100004, 6, '[XP] El límite mínimo de XP es 0.1.'),
('tw-mod-individual-xp', 100004, 7, 'El límite mínimo de XP es 0.1.'),
('tw-mod-individual-xp', 100005, 6, '[XP] Has actualizado tu XP a %g.'),
('tw-mod-individual-xp', 100005, 7, '[XP] Has actualizado tu XP a %g.'),
('tw-mod-individual-xp', 100006, 6, '[XP] Has restaurado tu tasa de XP al valor por defecto de %g.'),
('tw-mod-individual-xp', 100006, 7, '[XP] Has restaurado tu tasa de XP al valor por defecto de %g.');

UPDATE `creature_template` SET `ScriptName`='boss_ignis' WHERE `entry`=33118;
UPDATE `creature_template` SET `ScriptName`='boss_xt002' WHERE `entry`=33293;
UPDATE `creature_template` SET `ScriptName`='boss_auriaya' WHERE `entry`=33515;
UPDATE `creature_template` SET `ScriptName`='boss_kologarn' WHERE `entry`=32930;
UPDATE `creature_template` SET `ScriptName`='boss_kologarn_left_arm' WHERE `entry`=32933;
UPDATE `creature_template` SET `ScriptName`='boss_kologarn_right_arm' WHERE `entry`=32934;
UPDATE `creature_template` SET `ScriptName`='boss_steelbreaker' WHERE `entry`=32867;
UPDATE `creature_template` SET `ScriptName`='boss_molgeim' WHERE `entry`=32927;
UPDATE `creature_template` SET `ScriptName`='boss_brundir' WHERE `entry`=32857;
UPDATE `creature_template` SET `ScriptName`='boss_thorim' WHERE `entry`=32865;
UPDATE `creature_template` SET `ScriptName`='boss_hodir' WHERE `entry`=32845;
UPDATE `creature_template` SET `ScriptName`='boss_freya' WHERE `entry`=32906;
UPDATE `creature_template` SET `ScriptName`='boss_mimiron' WHERE `entry`=33350;
UPDATE `creature_template` SET `ScriptName`='boss_vezax' WHERE `entry`=33271;
UPDATE `creature_template` SET `ScriptName`='boss_yogg_saron' WHERE `entry`=33288;
UPDATE `creature_template` SET `ScriptName`='boss_algalon' WHERE `entry`=32871;


insert into `creature_template` values ('99005','0','0','0','0','0','22448','0','22448','0','Ulduar teleporter','',NULL,'0','80','80','64200','64200','12300','12300','31200','35','35','1','1.48','0.5','0','3170','6182','0','33235','1','1400','1900','0','0','0','0','0','0','0','0','0','0','100','7','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','1','3','1','1','0','0','0','0','0','0','0','0','1','99003','0','0','ulduar_teleporter');
replace into `creature_template_addon` values ('99005','0','0','0','0','0','48143 0');
replace into `locales_creature` values ('99005','','','','','','','','Wowacadem Server',NULL,NULL,NULL,NULL,NULL,NULL,NULL,'');
insert into `creature` values
('500000','99005','571','1','1','0','99003','9025.76','-1179.12','1060.18','4.59386','25','0','0','64200','12300','0','0'),
('500001','99005','603','2','1','0','99003','1497.84','-23.9801','421.367','5.0242','25','0','0','64200','12300','0','0'),
('500002','99005','603','2','1','0','99003','131.248','-35.3802','410.204','0','25','0','0','64200','12300','0','0'),
('500003','99005','603','2','1','0','99003','553.233','-12.3247','410.079','0','25','0','0','64200','12300','0','0'),
('500004','99005','603','2','1','0','99003','926.292','-11.4635','418.995','0','25','0','0','64200','12300','0','0'),
('500005','99005','603','2','1','0','99003','2086.17','-24.3111','421.639','3.11803','25','0','0','64200','12300','0','0');

DELETE FROM script_texts WHERE entry BETWEEN -1603017 AND -1603000;
INSERT INTO script_texts VALUES
(-1603000, "New toys? For me? I promise I won't break them this time!", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 15724, 1, 0, 0, "XT-002 Aggro"),
(-1603001, "NO! NO! NO! NO! NO!", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 15727, 1, 0, 0, "XT-002 Tympanic Tantrum"),
(-1603002, "I... I think I broke it.", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 15728, 1, 0, 0, "XT-002 Slay 1"),
(-1603003, "I guess it doesn't bend that way.", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 15729, 1, 0, 0, "XT-002 Slay 2"),
(-1603004, "So tired. I will rest for just a moment!", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 15725, 1, 0, 0, "XT-002 Heart Opened"),
(-1603005, "I'm ready to play!", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 15726, 1, 0, 0, "XT-002 Heart Closed"),
(-1603006, "Time for a new game! My old toys will fight my new toys!", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 15732, 1, 0, 0, "XT-002 Adds"),
(-1603007, "I'm tired of these toys. I don't want to play anymore!", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 15730, 1, 0, 0, "XT-002 Berserk"),
(-1603008, "You are bad... Toys... Very... Baaaaad!", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 15731, 1, 0, 0, "XT-002 Death"),
(-1603009, "Hostile entities detected. Threat assessment protocol active. Primary target engaged. Time minus thirty seconds to re-evaluation.", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 15506, 1, 0, 0, "Flame Leviathan Aggro"),
(-1603010, "Total systems failure. Defense protocols breached. Leviathan Unit shutting down.", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 15520, 1, 0, 0, "Flame Leviathan Death"),
(-1603011, "Threat assessment routine modified. Current target threat level: zero. Acquiring new target.", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 15521, 1, 0, 0, "Flame Leviathan Slay"),
(-1603012, "You will suffer for this trespass!", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 15552, 1, 0, 0, "Hodir Aggro"),
(-1603013, "I... I am released from his grasp... at last.", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 15557, 1, 0, 0, "Hodir Death"),
(-1603014, "Tragic. To come so far, only to fail.", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 15553, 1, 0, 0, "Hodir Slay 1"),
(-1603015, "Welcome to the endless winter.", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 15554, 1, 0, 0, "Hodir Slay 2"),
(-1603016, "Winds of the north consume you!", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 15555, 1, 0, 0, "Hodir Flash Freeze"),
(-1603017, "<Furious Roar>", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 15556, 1, 0, 0, "Hodir Frozen Blows");

UPDATE creature_template SET mechanic_immune_mask=617299803, scriptname='boss_ignis' WHERE entry=33118;
UPDATE creature_template SET minlevel=80, maxlevel=80, faction_h=1925, faction_a=1925, scale=0.5, scriptname='mob_scorch_target' WHERE entry=33221;
UPDATE creature_template SET scriptname='mob_iron_construct' WHERE entry=33121;

-- razorscale
UPDATE creature_template SET mechanic_immune_mask=617299803, scriptname='boss_razorscale' WHERE entry=33186;
UPDATE creature_template SET scriptname='mob_devouring_flame_target' WHERE entry=34188;
UPDATE creature_template SET scriptname='mob_dark_rune_watcher' WHERE entry=33453;
UPDATE creature_template SET scriptname='mob_dark_rune_sentinel' WHERE entry=33846;
UPDATE creature_template SET scriptname='mob_dark_rune_guardian' WHERE entry=33388;
UPDATE creature_template SET scriptname='npc_expedition_commander' WHERE entry=33210;

-- xt002
UPDATE creature_template SET mechanic_immune_mask=617299803, scriptname='boss_xt002' WHERE entry=33293;
UPDATE creature_template SET scriptname='mob_pummeler' WHERE entry=33344;
UPDATE creature_template SET speed=0.5, faction_a=1925, faction_h=1925, scriptname='mob_boombot' WHERE entry=33346;
UPDATE creature_template SET speed=0.5 WHERE entry=33343;
UPDATE creature_template SET mechanic_immune_mask=652951551, scriptname='mob_xtheart' WHERE entry=33329;

-- iron council
UPDATE creature_template SET scriptname='mob_ulduar_lightning_elemental' WHERE entry=32958;
UPDATE creature_template SET scriptname='mob_rune_of_power' WHERE entry=33705;
UPDATE creature_template SET scriptname='mob_rune_of_summoning' WHERE entry=33051;
UPDATE creature_template SET mechanic_immune_mask=619395071, scriptname='boss_brundir' WHERE entry=32857;
UPDATE creature_template SET mechanic_immune_mask=617299803, scriptname='boss_molgeim' WHERE entry=32927;
UPDATE creature_template SET mechanic_immune_mask=617299803, scriptname='boss_steelbreaker' WHERE entry=32867;

-- kologarn
DELETE FROM creature WHERE id IN (32933, 32934);
INSERT INTO creature (id, map, spawnMask, phaseMask, modelid, equipment_id, position_x, position_y, position_z, orientation, spawntimesecs, spawndist, currentwaypoint, curhealth, curmana, DeathState, MovementType) VALUES (32933, 603, 3, 65535, 0, 0, 1799.68, -24.3599, 452.227, 3.14747, 604800, 0, 0, 543855, 0, 0, 0);
INSERT INTO creature (id, map, spawnMask, phaseMask, modelid, equipment_id, position_x, position_y, position_z, orientation, spawntimesecs, spawndist, currentwaypoint, curhealth, curmana, DeathState, MovementType) VALUES (32934, 603, 3, 65535, 0, 0, 1799.68, -24.3599, 452.227, 3.14747, 604800, 0, 0, 543855, 0, 0, 0);
UPDATE creature_model_info SET bounding_radius=15, combat_reach=15 WHERE modelid IN (28638, 28822, 28821);
UPDATE creature_template SET mechanic_immune_mask=652951551, scriptname='boss_right_arm' WHERE entry=32934;
UPDATE creature_template SET mechanic_immune_mask=652951551, scriptname='boss_left_arm' WHERE entry=32933;
UPDATE creature_template SET scriptname='mob_ulduar_rubble' WHERE entry=33768;
UPDATE creature_template SET mechanic_immune_mask=617299803, scriptname='boss_kologarn' WHERE entry=32930;

-- auriaya
UPDATE creature_template SET minlevel=80, maxlevel=80, faction_h=14, faction_a=14, scriptname='mob_seeping_feral_essence' WHERE entry=34098;
UPDATE creature_template SET scriptname='mob_sanctum_sentry' WHERE entry=34014;
UPDATE creature_template SET mechanic_immune_mask=619395071, scriptname='mob_feral_defender' WHERE entry=34035;
UPDATE creature_template SET mechanic_immune_mask=583745371, scriptname='boss_auriaya' WHERE entry=33515;

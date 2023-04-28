DELETE FROM `zone_difficulty_info`;
INSERT INTO `zone_difficulty_info` (`MapId`, `PhaseMask`, `HealingNerfValue`, `AbsorbNerfValue`, `MeleeDmgBuffValue`, `SpellDmgBuffValue`, `Enabled`, `Comment`) VALUES
-- Vanilla Raids
(531, 0, '0.50', '0.50', '1.50', '1.30', 1, 'AQ40 Healing 50% / Absorb 50% Nerf / 50% physical & 30% spell damage buff'),
(509, 0, '0.50', '0.50', '1.60', '1.30', 1, 'AQ20 Healing 50% / Absorb 50% Nerf / 60% physical & 30% spell damage buff'),
(309, 0, '0.50', '0.50', '1.50', '1.10', 0, 'ZG20 Healing 50% / Absorb 50% Nerf / 50% physical & 10% spell damage buff'),
(469, 0, '0.50', '0.50', '1.50', '1.50', 0, 'BWL Healing 50% / Absorb 50% Nerf / 50% physical & 50% spell damage buff'),
(249, 0, '0.50', '0.50', '1.40', '1.40', 0, 'ONY Healing 50% / Absorb 50% Nerf / 40% physical & 40% spell damage buff'),
(409, 0, '0.50', '0.50', '1.50', '1.50', 0, 'MC Healing 50% / Absorb 50% Nerf / 50% physical & 50% spell damage buff'),
-- TBC Raids
(565, 0, '0.80', '0.80', '1.20', '1.20', 1, 'Normal Mode Gruul Healing 80% / Absorb 80% Nerf / 20% physical & 20% spell damage buff'),
(565, 0, '0.70', '0.70', '1.30', '1.30', 64, 'Hard Mode Gruul Healing 70% / Absorb 70% Nerf / 30% physical & 30% spell damage buff'),
(544, 0, '0.80', '0.80', '1.20', '1.20', 1, 'Normal Mode Magtheridon Healing 80% / Absorb 80% Nerf / 20% physical & 20% spell damage buff'),
(544, 0, '0.70', '0.70', '1.30', '1.30', 64, 'Hard Mode Magtheridon Healing 70% / Absorb 70% Nerf / 30% physical & 30% spell damage buff'),
-- TBC Heroics
(269, 0, '0.80', '0.80', '1.20', '1.20', 1, 'Heroic Mode Black Morass Healing 80% / Absorb 80% Nerf / 20% physical & 20% spell damage buff'),
(269, 0, '0.70', '0.70', '1.30', '1.30', 64, 'Hard Mode Black Morass Healing 70% / Absorb 70% Nerf / 30% physical & 30% spell damage buff'),
(540, 0, '0.80', '0.80', '1.20', '1.20', 1, 'Heroic Mode Shattered Halls Healing 80% / Absorb 80% Nerf / 20% physical & 20% spell damage buff'),
(540, 0, '0.70', '0.70', '1.30', '1.30', 64, 'Hard Mode Shattered Halls Healing 70% / Absorb 70% Nerf / 30% physical & 30% spell damage buff'),
(542, 0, '0.80', '0.80', '1.20', '1.20', 1, 'Heroic Mode Blood Furnace Healing 80% / Absorb 80% Nerf / 20% physical & 20% spell damage buff'),
(542, 0, '0.70', '0.70', '1.30', '1.30', 64, 'Hard Mode Blood Furnace Healing 70% / Absorb 70% Nerf / 30% physical & 30% spell damage buff'),
(543, 0, '0.80', '0.80', '1.20', '1.20', 1, 'Heroic Mode Hellfire Ramparts Healing 80% / Absorb 80% Nerf / 20% physical & 20% spell damage buff'),
(543, 0, '0.70', '0.70', '1.30', '1.30', 64, 'Hard Mode Hellfire Ramparts Healing 70% / Absorb 70% Nerf / 30% physical & 30% spell damage buff'),
(545, 0, '0.80', '0.80', '1.20', '1.20', 1, 'Heroic Mode The Steamvault Healing 80% / Absorb 80% Nerf / 20% physical & 20% spell damage buff'),
(545, 0, '0.70', '0.70', '1.30', '1.30', 64, 'Hard Mode The Steamvault Healing 70% / Absorb 70% Nerf / 30% physical & 30% spell damage buff'),
(546, 0, '0.80', '0.80', '1.20', '1.20', 1, 'Heroic Mode The Underbog Healing 80% / Absorb 80% Nerf / 20% physical & 20% spell damage buff'),
(546, 0, '0.70', '0.70', '1.30', '1.30', 64, 'Hard Mode The Underbog Healing 70% / Absorb 70% Nerf / 30% physical & 30% spell damage buff'),
(547, 0, '0.80', '0.80', '1.20', '1.20', 1, 'Heroic Mode Slave Pens Healing 80% / Absorb 80% Nerf / 20% physical & 20% spell damage buff'),
(547, 0, '0.70', '0.70', '1.30', '1.30', 64, 'Hard Mode Slave Pens Healing 70% / Absorb 70% Nerf / 30% physical & 30% spell damage buff'),
(552, 0, '0.80', '0.80', '1.20', '1.20', 1, 'Heroic Mode The Arcatraz Healing 80% / Absorb 80% Nerf / 20% physical & 20% spell damage buff'),
(552, 0, '0.70', '0.70', '1.30', '1.30', 64, 'Hard Mode The Arcatraz Healing 70% / Absorb 70% Nerf / 30% physical & 30% spell damage buff'),
(553, 0, '0.80', '0.80', '1.20', '1.20', 1, 'Heroic Mode The Botanica Healing 80% / Absorb 80% Nerf / 20% physical & 20% spell damage buff'),
(553, 0, '0.70', '0.70', '1.30', '1.30', 64, 'Hard Mode The Botanica Healing 70% / Absorb 70% Nerf / 30% physical & 30% spell damage buff'),
(554, 0, '0.80', '0.80', '1.20', '1.20', 1, 'Heroic Mode The Mechanar Healing 80% / Absorb 80% Nerf / 20% physical & 20% spell damage buff'),
(554, 0, '0.70', '0.70', '1.30', '1.30', 64, 'Hard Mode The Mechanar Healing 70% / Absorb 70% Nerf / 30% physical & 30% spell damage buff'),
(555, 0, '0.80', '0.80', '1.20', '1.20', 1, 'Heroic Mode Shadow Labyrinth Healing 80% / Absorb 80% Nerf / 20% physical & 20% spell damage buff'),
(555, 0, '0.70', '0.70', '1.30', '1.30', 64, 'Hard Mode Shadow Labyrinth Healing 70% / Absorb 70% Nerf / 30% physical & 30% spell damage buff'),
(556, 0, '0.80', '0.80', '1.20', '1.20', 1, 'Heroic Mode Sethekk Halls Healing 80% / Absorb 80% Nerf / 20% physical & 20% spell damage buff'),
(556, 0, '0.70', '0.70', '1.30', '1.30', 64, 'Hard Mode Sethekk Halls Healing 70% / Absorb 70% Nerf / 30% physical & 30% spell damage buff'),
(557, 0, '0.80', '0.80', '1.20', '1.20', 1, 'Heroic Mode Mana Tombs Healing 80% / Absorb 80% Nerf / 20% physical & 20% spell damage buff'),
(557, 0, '0.70', '0.70', '1.30', '1.30', 64, 'Hard Mode Mana Tombs Healing 70% / Absorb 70% Nerf / 30% physical & 30% spell damage buff'),
(558, 0, '0.80', '0.80', '1.20', '1.20', 1, 'Heroic Mode Auchenai Crypts Healing 80% / Absorb 80% Nerf / 20% physical & 20% spell damage buff'),
(558, 0, '0.70', '0.70', '1.30', '1.30', 64, 'Hard Mode Auchenai Crypts Healing 70% / Absorb 70% Nerf / 30% physical & 30% spell damage buff'),
(560, 0, '0.80', '0.80', '1.20', '1.20', 1, 'Heroic Mode The Escape From Durnholde Healing 80% / Absorb 80% Nerf / 20% physical & 20% spell damage buff'),
(560, 0, '0.70', '0.70', '1.30', '1.30', 64, 'Hard Mode The Escape From Durnholde Healing 70% / Absorb 70% Nerf / 30% physical & 30% spell damage buff'),
(585, 0, '0.80', '0.80', '1.20', '1.20', 1, 'Heroic Mode Magister\'s Terrace Healing 80% / Absorb 80% Nerf / 20% physical & 20% spell damage buff'),
(585, 0, '0.70', '0.70', '1.30', '1.30', 64, 'Hard Mode Magister\'s Terrace Healing 70% / Absorb 70% Nerf / 30% physical & 30% spell damage buff'),
-- Battlegrounds
(30, 0, '0.80', '0.80', '0.70', '0.70', 1, 'AV Healing 20% / Absorb 20% / 30% physical & spell damage nerf'),
(489, 0, '0.80', '0.80', '0.70', '0.70', 1, 'WSG Healing 20% / Absorb 20% / 30% physical & spell damage nerf'),
(529, 0, '0.80', '0.80', '0.70', '0.70', 1, 'AB Healing 20% / Absorb 20% / 30% physical & spell damage nerf'),
(566, 0, '0.80', '0.80', '0.70', '0.70', 1, 'EotS Healing 20% / Absorb 20% / 30% physical & spell damage nerf'),
-- arenas
(559, 0, '0.80', '0.80', '0.70', '0.70', 1, 'Ring of Trials Healing 20% / Absorb 20% / 30% physical & spell damage nerf'),
(562, 0, '0.80', '0.80', '0.70', '0.70', 1, 'Blades Edge Arena  Healing 20% / Absorb 20% / 30% physical & spell damage nerf'),
(572, 0, '0.80', '0.80', '0.70', '0.70', 1, 'Ruins of Lordaeron Healing 20% / Absorb 20% / 30% physical & spell damage nerf'),
(617, 0, '0.80', '0.80', '0.70', '0.70', 1, 'Dalaran Arena Healing 20% / Absorb 20% / 30% physical & spell damage nerf'),
(618, 0, '0.80', '0.80', '0.70', '0.70', 1, 'Ring of Valor Healing 20% / Absorb 20% / 30% physical & spell damage nerf'),
-- Duels in Forbidding Sea (Wetlands)
(2147483647, 0, '0.80', '0.80', '0.70', '0.70', 1, 'Zone 2402 Duel Healing 20% / Absorb 20% Nerf / 30% physical & spell damage nerf');

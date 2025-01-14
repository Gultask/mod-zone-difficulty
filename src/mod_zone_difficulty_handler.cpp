/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "Config.h"
#include "Chat.h"
#include "GameTime.h"
#include "ItemTemplate.h"
#include "MapMgr.h"
#include "Pet.h"
#include "Player.h"
#include "PoolMgr.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "StringConvert.h"
#include "TaskScheduler.h"
#include "Tokenize.h"
#include "Unit.h"
#include "ZoneDifficulty.h"

ZoneDifficulty* ZoneDifficulty::instance()
{
    static ZoneDifficulty instance;
    return &instance;
}

void ZoneDifficulty::LoadMapDifficultySettings()
{
    if (!sZoneDifficulty->IsEnabled)
    {
        return;
    }

    sZoneDifficulty->Rewards.clear();
    sZoneDifficulty->HardmodeAI.clear();
    sZoneDifficulty->CreatureOverrides.clear();
    sZoneDifficulty->DailyHeroicQuests.clear();
    sZoneDifficulty->HardmodeLoot.clear();
    sZoneDifficulty->DisallowedBuffs.clear();
    sZoneDifficulty->SpellNerfOverrides.clear();
    sZoneDifficulty->NerfInfo.clear();

    // Default values for when there is no entry in the db for duels (index 0xFFFFFFFF)
    NerfInfo[DUEL_INDEX][0].HealingNerfPct = 1;
    NerfInfo[DUEL_INDEX][0].AbsorbNerfPct = 1;
    NerfInfo[DUEL_INDEX][0].MeleeDamageBuffPct = 1;
    NerfInfo[DUEL_INDEX][0].SpellDamageBuffPct = 1;

    // Heroic Quest -> MapId Translation
    HeroicTBCQuestMapList[542] = 11362; // Blood Furnace
    HeroicTBCQuestMapList[543] = 11354; // Hellfire Ramparts
    HeroicTBCQuestMapList[547] = 11368; // Slave Pens
    HeroicTBCQuestMapList[546] = 11369; // The Underbog
    HeroicTBCQuestMapList[557] = 11373; // Mana-Tombs
    HeroicTBCQuestMapList[558] = 11374; // Auchenai Crypts
    HeroicTBCQuestMapList[560] = 11378; // The Escape From Durnholde
    HeroicTBCQuestMapList[556] = 11372; // Sethekk Halls
    HeroicTBCQuestMapList[585] = 11499; // Magisters' Terrace
    HeroicTBCQuestMapList[555] = 11375; // Shadow Labyrinth
    HeroicTBCQuestMapList[540] = 11363; // Shattered Halls
    HeroicTBCQuestMapList[552] = 11388; // The Arcatraz
    HeroicTBCQuestMapList[269] = 11382; // The Black Morass
    HeroicTBCQuestMapList[553] = 11384; // The Botanica
    HeroicTBCQuestMapList[554] = 11386; // The Mechanar
    HeroicTBCQuestMapList[545] = 11370; // The Steamvault

    // Category 8
    EncounterCounter[542] = 3; // Blood Furnace
    EncounterCounter[543] = 3; // Hellfire Ramparts
    EncounterCounter[547] = 3; // Slave Pens
    EncounterCounter[546] = 4; // The Underbog
    EncounterCounter[557] = 4; // Mana-Tombs
    EncounterCounter[558] = 2; // Auchenai Crypts
    EncounterCounter[560] = 3; // The Escape From Durnholde
    EncounterCounter[556] = 3; // Sethekk Halls
    //EncounterCounter[585] = 4; // Magisters' Terrace
    EncounterCounter[555] = 4; // Shadow Labyrinth
    EncounterCounter[540] = 4; // Shattered Halls
    EncounterCounter[552] = 4; // The Arcatraz
    EncounterCounter[269] = 3; // The Black Morass
    EncounterCounter[553] = 5; // The Botanica
    EncounterCounter[554] = 3; // The Mechanar
    EncounterCounter[545] = 3; // The Steamvault

    // Category 9
    EncounterCounter[565] = 2; // Gruul's Lair
    EncounterCounter[544] = 1; // Magtheridon's Lair
    EncounterCounter[532] = 12; // Karazhan

    // Icons
    sZoneDifficulty->ItemIcons[ITEMTYPE_MISC] = "|TInterface\\icons\\inv_misc_cape_17:15|t |TInterface\\icons\\inv_misc_gem_topaz_02:15|t |TInterface\\icons\\inv_jewelry_ring_51naxxramas:15|t ";
    sZoneDifficulty->ItemIcons[ITEMTYPE_CLOTH] = "|TInterface\\icons\\inv_chest_cloth_42:15|t ";
    sZoneDifficulty->ItemIcons[ITEMTYPE_LEATHER] = "|TInterface\\icons\\inv_helmet_41:15|t ";
    sZoneDifficulty->ItemIcons[ITEMTYPE_MAIL] = "|TInterface\\icons\\inv_chest_chain_13:15|t ";
    sZoneDifficulty->ItemIcons[ITEMTYPE_PLATE] = "|TInterface\\icons\\inv_chest_plate12:15|t ";
    sZoneDifficulty->ItemIcons[ITEMTYPE_WEAPONS] = "|TInterface\\icons\\inv_mace_25:15|t |TInterface\\icons\\inv_shield_27:15|t |TInterface\\icons\\inv_weapon_crossbow_04:15|t ";

    if (QueryResult result = WorldDatabase.Query("SELECT * FROM zone_difficulty_info"))
    {
        do
        {
            uint32 mapId = (*result)[0].Get<uint32>();
            uint32 phaseMask = (*result)[1].Get<uint32>();
            ZoneDifficultyNerfData data;
            int8 mode = (*result)[6].Get<int8>();
            if (sZoneDifficulty->HasNormalMode(mode))
            {
                data.HealingNerfPct = (*result)[2].Get<float>();
                data.AbsorbNerfPct = (*result)[3].Get<float>();
                data.MeleeDamageBuffPct = (*result)[4].Get<float>();
                data.SpellDamageBuffPct = (*result)[5].Get<float>();
                data.Enabled = data.Enabled | mode;
                sZoneDifficulty->NerfInfo[mapId][phaseMask] = data;
            }
            if (sZoneDifficulty->HasHardMode(mode) && sZoneDifficulty->HardmodeEnable)
            {
                data.HealingNerfPctHard = (*result)[2].Get<float>();
                data.AbsorbNerfPctHard = (*result)[3].Get<float>();
                data.MeleeDamageBuffPctHard = (*result)[4].Get<float>();
                data.SpellDamageBuffPctHard = (*result)[5].Get<float>();
                data.Enabled = data.Enabled | mode;
                sZoneDifficulty->NerfInfo[mapId][phaseMask] = data;
            }
            if ((mode & MODE_HARD) != MODE_HARD && (mode & MODE_NORMAL) != MODE_NORMAL)
            {
                LOG_ERROR("module", "MOD-ZONE-DIFFICULTY: Invalid mode {} used in Enabled for mapId {}, ignored.", mode, mapId);
            }

            // duels do not check for phases. Only 0 is allowed.
            if (mapId == DUEL_INDEX && phaseMask != 0)
            {
                LOG_ERROR("module", "MOD-ZONE-DIFFICULTY: Table `zone_difficulty_info` for criteria (duel mapId: {}) has wrong value ({}), must be 0 for duels.", mapId, phaseMask);
            }

        } while (result->NextRow());
    }

    if (QueryResult result = WorldDatabase.Query("SELECT * FROM zone_difficulty_spelloverrides"))
    {
        do
        {
            if ((*result)[2].Get<bool>())
            {
                sZoneDifficulty->SpellNerfOverrides[(*result)[0].Get<uint32>()] = (*result)[1].Get<float>();
            }

        } while (result->NextRow());
    }

    if (QueryResult result = WorldDatabase.Query("SELECT * FROM zone_difficulty_disallowed_buffs"))
    {
        do
        {
            std::vector<uint32> debuffs;
            uint32 mapId;
            if ((*result)[2].Get<bool>())
            {
                std::string spellString = (*result)[1].Get<std::string>();
                std::vector<std::string_view> tokens = Acore::Tokenize(spellString, ' ', false);

                mapId = (*result)[0].Get<uint32>();
                for (auto token : tokens)
                {
                    if (token.empty())
                    {
                        continue;
                    }

                    uint32 spell;
                    if ((spell = Acore::StringTo<uint32>(token).value()))
                    {
                        debuffs.push_back(spell);
                    }
                    else
                    {
                        LOG_ERROR("module", "MOD-ZONE-DIFFICULTY: Disabling buffs for spell '{}' is invalid, skipped.", spell);
                    }
                }
                sZoneDifficulty->DisallowedBuffs[mapId] = debuffs;
            }
        } while (result->NextRow());
    }

    if (QueryResult result = WorldDatabase.Query("SELECT * FROM zone_difficulty_hardmode_instance_data"))
    {
        do
        {
            ZoneDifficultyHardmodeMapData data;
            uint32 MapID = (*result)[0].Get<uint32>();
            data.EncounterEntry = (*result)[1].Get<uint32>();
            data.Override = (*result)[2].Get<uint32>();
            data.RewardType = (*result)[3].Get<uint8>();

            sZoneDifficulty->HardmodeLoot[MapID].push_back(data);
            //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: New creature for map {} with entry: {}", MapID, data.EncounterEntry);

            Expansion[MapID] = data.RewardType;

        } while (result->NextRow());
    }
    else
    {
        LOG_ERROR("module", "MOD-ZONE-DIFFICULTY: Query failed: SELECT * FROM zone_difficulty_hardmode_instance_data");
    }

    if (QueryResult result = WorldDatabase.Query("SELECT entry FROM `pool_quest` WHERE `pool_entry`=356"))
    {
        do
        {
            sZoneDifficulty->DailyHeroicQuests.push_back((*result)[0].Get<uint32>());
            //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Adding daily heroic quest with id {}.", (*result)[0].Get<uint32>());
        } while (result->NextRow());
    }
    else
    {
        LOG_ERROR("module", "MOD-ZONE-DIFFICULTY: Query failed: SELECT entry FROM `pool_quest` WHERE `pool_entry`=356");
    }

    if (QueryResult result = WorldDatabase.Query("SELECT * FROM zone_difficulty_hardmode_creatureoverrides"))
    {
        do
        {
            uint32 creatureEntry = (*result)[0].Get<uint32>();
            float hpModifier = (*result)[1].Get<float>();
            bool enabled = (*result)[2].Get<bool>();

            if (enabled)
            {
                if (hpModifier != 0)
                {
                    sZoneDifficulty->CreatureOverrides[creatureEntry] = hpModifier;
                }
                //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: New creature with entry: {} has exception for hp: {}", creatureEntry, hpModifier);
            }
        } while (result->NextRow());
    }
    else
    {
        LOG_ERROR("module", "MOD-ZONE-DIFFICULTY: Query failed: SELECT * FROM zone_difficulty_hardmode_creatureoverrides");
    }

    if (QueryResult result = WorldDatabase.Query("SELECT * FROM zone_difficulty_hardmode_ai"))
    {
        do
        {
            bool enabled = (*result)[11].Get<bool>();

            if (enabled)
            {
                uint32 creatureEntry = (*result)[0].Get<uint32>();
                ZoneDifficultyHAI data;
                data.Chance = (*result)[1].Get<uint8>();
                data.Spell = (*result)[2].Get<uint32>();
                data.Spellbp0 = (*result)[3].Get<int32>();
                data.Spellbp1 = (*result)[4].Get<int32>();
                data.Spellbp2 = (*result)[5].Get<int32>();
                data.Target = (*result)[6].Get<uint8>();
                data.TargetArg = (*result)[7].Get<uint8>();
                data.Delay = (*result)[8].Get<Milliseconds>();
                data.Cooldown = (*result)[9].Get<Milliseconds>();
                data.Repetitions = (*result)[10].Get<uint8>();

                if (data.Chance != 0 && data.Spell != 0 && ((data.Target >= 1 && data.Target <= 6) || data.Target == 18))
                {
                    sZoneDifficulty->HardmodeAI[creatureEntry].push_back(data);
                    LOG_INFO("module", "MOD-ZONE-DIFFICULTY: New AI for entry {} with spell {}", creatureEntry, data.Spell);
                }
                else
                {
                    LOG_ERROR("module", "MOD-ZONE-DIFFICULTY: Unknown type for `Target`: {} in zone_difficulty_hardmode_ai", data.Target);
                }
                //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: New creature with entry: {} has exception for hp: {}", creatureEntry, hpModifier);
            }
        } while (result->NextRow());
    }
    else
    {
        LOG_ERROR("module", "MOD-ZONE-DIFFICULTY: Query failed: SELECT * FROM zone_difficulty_hardmode_ai");
    }

    //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Starting load of rewards.");
    if (QueryResult result = WorldDatabase.Query("SELECT ContentType, ItemType, Entry, Price, Enchant, EnchantSlot, Achievement, Enabled FROM zone_difficulty_hardmode_rewards"))
    {
        /* debug
         * uint32 i = 0;
         * end debug
         */
        do
        {
            /* debug
             * ++i;
             * end debug
             */
            ZoneDifficultyRewardData data;
            uint32 contentType = (*result)[0].Get<uint32>();
            uint32 itemType = (*result)[1].Get<uint32>();
            data.Entry = (*result)[2].Get<uint32>();
            data.Price = (*result)[3].Get<uint32>();
            data.Enchant = (*result)[4].Get<uint32>();
            data.EnchantSlot = (*result)[5].Get<uint8>();
            data.Achievement = (*result)[6].Get<int32>();
            bool enabled = (*result)[7].Get<bool>();

            if (enabled)
            {
                if (data.Achievement >= 0)
                {
                    sZoneDifficulty->Rewards[contentType][itemType].push_back(data);
                    //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Loading item with entry {} has enchant {} in slot {}. contentType: {} itemType: {}", data.Entry, data.Enchant, data.EnchantSlot, contentType, itemType);
                }
                else
                {
                    sZoneDifficulty->TierRewards[contentType] = data;
                    //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Loading tier reward with entry {} has enchant {} in slot {}. contentType: {} itemType: {}", data.Entry, data.Enchant, data.EnchantSlot, contentType, itemType);
                }
            }
            //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Total items in Rewards map: {}.", i);
        } while (result->NextRow());
    }
    else
    {
        LOG_ERROR("module", "MOD-ZONE-DIFFICULTY: Query failed: SELECT ContentType, ItemType, Entry, Price, Enchant, EnchantSlot, Achievement, Enabled FROM zone_difficulty_hardmode_rewards");
    }
}

/**
 *  @brief Loads the HardmodeInstanceData from the database. Fetch from zone_difficulty_instance_saves.
 *
 *  `InstanceID` INT NOT NULL DEFAULT 0,
 *  `HardmodeOn` TINYINT NOT NULL DEFAULT 0,
 *
 *  Exclude data not in the IDs stored in GetInstanceIDs() and delete
 *  zone_difficulty_instance_saves for instances that no longer exist.
 */
void ZoneDifficulty::LoadHardmodeInstanceData()
{
    std::vector<bool> instanceIDs = sMapMgr->GetInstanceIDs();
    /* debugging
    * for (int i = 0; i < int(instanceIDs.size()); i++)
    * {
    *   LOG_INFO("module", "MOD-ZONE-DIFFICULTY: ZoneDifficulty::LoadHardmodeInstanceData: id {} exists: {}:", i, instanceIDs[i]);
    * }
    * end debugging
    */
    if (QueryResult result = CharacterDatabase.Query("SELECT * FROM zone_difficulty_instance_saves"))
    {
        do
        {
            uint32 InstanceId = (*result)[0].Get<uint32>();
            bool HardmodeOn = (*result)[1].Get<bool>();

            if (instanceIDs[InstanceId])
            {
                LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Loading from DB for instanceId {}: HardmodeOn = {}", InstanceId, HardmodeOn);
                sZoneDifficulty->HardmodeInstanceData[InstanceId] = HardmodeOn;
            }
            else
            {
                CharacterDatabase.Execute("DELETE FROM zone_difficulty_instance_saves WHERE InstanceID = {}", InstanceId);
            }


        } while (result->NextRow());
    }
}

/**
 *  @brief Loads the score data and encounter logs from the database.
 *  Fetch from zone_difficulty_hardmode_score.
 *
 *  `CharacterGuid` INT NOT NULL DEFAULT 0,
 *  `Type` TINYINT NOT NULL DEFAULT 0,
 *  `Score` INT NOT NULL DEFAULT 0,
 *
 *  Fetch from zone_difficulty_encounter_logs.
 *  `InstanceId` INT NOT NULL DEFAULT 0,
 *  `TimestampStart` INT NOT NULL DEFAULT 0,
 *  `TimestampEnd` INT NOT NULL DEFAULT 0,
 *  `Map` INT NOT NULL DEFAULT 0,
 *  `BossId` INT NOT NULL DEFAULT 0,
 *  `PlayerGuid` INT NOT NULL DEFAULT 0,
 *  `Mode` INT NOT NULL DEFAULT 0,
 *
 */
void ZoneDifficulty::LoadHardmodeScoreData()
{
    if (QueryResult result = CharacterDatabase.Query("SELECT * FROM zone_difficulty_hardmode_score"))
    {
        do
        {
            uint32 GUID = (*result)[0].Get<uint32>();
            uint8 Type = (*result)[1].Get<uint8>();
            uint32 Score = (*result)[2].Get<uint32>();

            //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Loading from DB for player with GUID {}: Type = {}, Score = {}", GUID, Type, Score);
            sZoneDifficulty->HardmodeScore[GUID][Type] = Score;

        } while (result->NextRow());
    }
    if (QueryResult result = CharacterDatabase.Query("SELECT `Map`, `BossId`, `PlayerGuid` FROM zone_difficulty_encounter_logs WHERE `Mode` = 64"))
    {
        do
        {
            uint32 MapId = (*result)[0].Get<uint32>();
            uint8 BossID = (*result)[1].Get<uint32>();
            uint32 PlayerGuid = (*result)[2].Get<uint32>();

            // Set all BossID which aren't true to false for that mapID
            if (sZoneDifficulty->Logs[PlayerGuid].find(MapId) == sZoneDifficulty->Logs[PlayerGuid].end())
            {
                for (int i = 0; i < sZoneDifficulty->EncounterCounter[MapId]; ++i)
                {
                    LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Initializing player record for PlayerGuid {} in MapId {} for BossId {}: False", PlayerGuid, MapId, i);
                    sZoneDifficulty->Logs[PlayerGuid][MapId][i] = false;
                }
            }
            LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Setting player record for PlayerGuid {} in MapId {} for BossId {}: True", PlayerGuid, MapId, BossID);
            sZoneDifficulty->Logs[PlayerGuid][MapId][BossID] = true;
        } while (result->NextRow());
    }
}

/**
 *  @brief Sends a whisper to all members of the player's raid in the same instance as the creature.
 *
 *  @param message The message which should be sent to the <Player>.
 *  @param creature The creature who sends the whisper.
 *  @param player The object of the player, whose whole group should receive the message.
 */
void ZoneDifficulty::SendWhisperToRaid(std::string message, Creature* creature, Player* player)
{
    if (Map* map = creature->GetMap())
    {
        map->DoForAllPlayers([&, player, creature](Player* mapPlayer) {
            if (creature && player)
            {
                if (mapPlayer->IsInSameGroupWith(player))
                {
                    creature->Whisper(message, LANG_UNIVERSAL, mapPlayer);
                }
            }
        });
    }
}

std::string ZoneDifficulty::GetItemTypeString(uint32 type)
{
    std::string typestring;
    switch (type)
    {
    case ITEMTYPE_MISC:
        typestring = "Back, Finger, Neck, and Trinket";
        break;
    case ITEMTYPE_CLOTH:
        typestring = "Cloth";
        break;
    case ITEMTYPE_LEATHER:
        typestring = "Leather";
        break;
    case ITEMTYPE_MAIL:
        typestring = "Mail";
        break;
    case ITEMTYPE_PLATE:
        typestring = "Plate";
        break;
    case ITEMTYPE_WEAPONS:
        typestring = "Weapons and Shields";
        break;
    default:
        LOG_ERROR("module", "MOD-ZONE-DIFFICULTY: Unknown type {} in ZoneDifficulty::GetItemTypeString.", type);
    }
    return typestring;
}

std::string ZoneDifficulty::GetContentTypeString(uint32 type)
{
    std::string typestring;
    switch (type)
    {
    case TYPE_VANILLA:
        typestring = "for Vanilla dungeons.";
        break;
    case TYPE_RAID_MC:
        typestring = "for Molten Core.";
        break;
    case TYPE_RAID_ONY:
        typestring = "for Onyxia.";
        break;
    case TYPE_RAID_BWL:
        typestring = "for Blackwing Lair.";
        break;
    case TYPE_RAID_ZG:
        typestring = "for Zul Gurub.";
        break;
    case TYPE_RAID_AQ20:
        typestring = "for Ruins of Ahn'Qiraj.";
        break;
    case TYPE_RAID_AQ40:
        typestring = "for Temple of Ahn'Qiraj.";
        break;
    case TYPE_HEROIC_TBC:
        typestring = "for Heroic TBC dungeons.";
        break;
    case TYPE_RAID_T4:
        typestring = "for T4 Raids.";
        break;
    case TYPE_RAID_T5:
        typestring = "for T5 Raids.";
        break;
    case TYPE_RAID_T6:
        typestring = "for T6 Raids.";
        break;
    case TYPE_HEROIC_WOTLK:
        typestring = "for Heroic WotLK dungeons.";
        break;
    case TYPE_RAID_T7:
        typestring = "for T7 Raids.";
        break;
    case TYPE_RAID_T8:
        typestring = "for T8 Raids.";
        break;
    case TYPE_RAID_T9:
        typestring = "for T9 Raids.";
        break;
    case TYPE_RAID_T10:
        typestring = "for T10 Raids.";
        break;
    default:
        typestring = "-";
    }
    return typestring;
}

/**
 *  @brief Grants every player in the group one score for the hardmode.
 *
 *  @param map The map where the player is currently.
 *  @param type The type of instance the score is awarded for.
 */
void ZoneDifficulty::AddHardmodeScore(Map* map, uint32 type, uint32 score)
{
    if (!map)
    {
        LOG_ERROR("module", "MOD-ZONE-DIFFICULTY: No object for map in AddHardmodeScore.");
        return;
    }
    if (type > 255)
    {
        LOG_ERROR("module", "MOD-ZONE-DIFFICULTY: Wrong value for type: {} in AddHardmodeScore for map with id {}.", type, map->GetInstanceId());
        return;
    }
    //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Called AddHardmodeScore for map id: {} and type: {}", map->GetId(), type);
    Map::PlayerList const& PlayerList = map->GetPlayers();
    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
    {
        Player* player = i->GetSource();
        if (sZoneDifficulty->HardmodeScore.find(player->GetGUID().GetCounter()) == sZoneDifficulty->HardmodeScore.end())
        {
            sZoneDifficulty->HardmodeScore[player->GetGUID().GetCounter()][type] = score;
        }
        else if (sZoneDifficulty->HardmodeScore[player->GetGUID().GetCounter()].find(type) == sZoneDifficulty->HardmodeScore[player->GetGUID().GetCounter()].end())
        {
            sZoneDifficulty->HardmodeScore[player->GetGUID().GetCounter()][type] = score;
        }
        else
        {
            sZoneDifficulty->HardmodeScore[player->GetGUID().GetCounter()][type] = sZoneDifficulty->HardmodeScore[player->GetGUID().GetCounter()][type] + score;
        }

        //if (sZoneDifficulty->IsDebugInfoEnabled)
        //{
        //    LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Player {} new score: {}", player->GetName(), sZoneDifficulty->HardmodeScore[player->GetGUID().GetCounter()][type]);
        //}

        std::string typestring = sZoneDifficulty->GetContentTypeString(type);
        ChatHandler(player->GetSession()).PSendSysMessage("You have received hardmode score %s New score: %i", typestring, sZoneDifficulty->HardmodeScore[player->GetGUID().GetCounter()][type]);
        CharacterDatabase.Execute("REPLACE INTO zone_difficulty_hardmode_score VALUES({}, {}, {})", player->GetGUID().GetCounter(), type, sZoneDifficulty->HardmodeScore[player->GetGUID().GetCounter()][type]);
    }
}

/**
 *  @brief Reduce the score of players when they pay for rewards.
 *
 *  @param player The one who pays with their score.
 *  @param type The type of instance the score is deducted for.
 */
void ZoneDifficulty::DeductHardmodeScore(Player* player, uint32 type, uint32 score)
{
    // NULL check happens in the calling function
    if (sZoneDifficulty->IsDebugInfoEnabled)
    {
        LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Reducing score with type {} from player with guid {} by {}.", type, player->GetGUID().GetCounter(), score);
    }
    sZoneDifficulty->HardmodeScore[player->GetGUID().GetCounter()][type] = sZoneDifficulty->HardmodeScore[player->GetGUID().GetCounter()][type] - score;
    CharacterDatabase.Execute("REPLACE INTO zone_difficulty_hardmode_score VALUES({}, {}, {})", player->GetGUID().GetCounter(), type, sZoneDifficulty->HardmodeScore[player->GetGUID().GetCounter()][type]);
}

/**
 * @brief Send and item to the player using the data from sZoneDifficulty->Rewards.
 *
 * @param player The recipient of the mail.
 * @param category The content level e.g. TYPE_HEROIC_TBC.
 * @param itemType The type of the item e.g. ITEMTYPE_CLOTH.
 * @param id the id in the vector.
 */
void ZoneDifficulty::SendItem(Player* player, uint32 category, uint32 itemType, uint32 id)
{
    //Check if a full tier cleareance reward is meant (itemType 99)
    ItemTemplate const* itemTemplate;
    if (itemType == 99)
    {
        itemTemplate = sObjectMgr->GetItemTemplate(sZoneDifficulty->TierRewards[category].Entry);
    }
    else
    {
        itemTemplate = sObjectMgr->GetItemTemplate(sZoneDifficulty->Rewards[category][itemType][id].Entry);
    }

    if (!itemTemplate)
    {
        LOG_ERROR("module", "MOD-ZONE-DIFFICULTY: itemTemplate could not be constructed in sZoneDifficulty->SendItem for category {}, itemType {}, id {}.", category, itemType, id);
        return;
    }

    ObjectGuid::LowType senderGuid = player->GetGUID().GetCounter();

    // fill mail
    MailDraft draft(REWARD_MAIL_SUBJECT, REWARD_MAIL_BODY);
    MailSender sender(MAIL_NORMAL, senderGuid, MAIL_STATIONERY_GM);
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    if (itemType == 99)
    {
        if (Item* item = Item::CreateItem(sZoneDifficulty->TierRewards[category].Entry, 1, player))
        {
            if (sZoneDifficulty->TierRewards[category].EnchantSlot != 0 && sZoneDifficulty->TierRewards[category].Enchant != 0)
            {
                item->SetEnchantment(EnchantmentSlot(sZoneDifficulty->TierRewards[category].EnchantSlot), sZoneDifficulty->TierRewards[category].Enchant, 0, 0, player->GetGUID());
                player->ApplyEnchantment(item, EnchantmentSlot(sZoneDifficulty->TierRewards[category].EnchantSlot), true, true, true);
            }
            item->SaveToDB(trans); // save for prevent lost at next mail load, if send fail then item will deleted
            draft.AddItem(item);
        }
    }
    else
    {
        if (Item* item = Item::CreateItem(sZoneDifficulty->Rewards[category][itemType][id].Entry, 1, player))
        {
            if (sZoneDifficulty->Rewards[category][itemType][id].EnchantSlot != 0 && sZoneDifficulty->Rewards[category][itemType][id].Enchant != 0)
            {
                item->SetEnchantment(EnchantmentSlot(sZoneDifficulty->Rewards[category][itemType][id].EnchantSlot), sZoneDifficulty->Rewards[category][itemType][id].Enchant, 0, 0, player->GetGUID());
                player->ApplyEnchantment(item, EnchantmentSlot(sZoneDifficulty->Rewards[category][itemType][id].EnchantSlot), true, true, true);
            }
            item->SaveToDB(trans); // save for prevent lost at next mail load, if send fail then item will deleted
            draft.AddItem(item);
        }
    }
    draft.SendMailTo(trans, MailReceiver(player, senderGuid), sender);
    CharacterDatabase.CommitTransaction(trans);
}

/**
 *  @brief Check if the map has assigned any data to tune it.
 *
 *  @param map The ID of the <Map> to check.
 *  @return The result as bool.
 */
bool ZoneDifficulty::IsHardmodeMap(uint32 mapId)
{
    if (!sZoneDifficulty->HardmodeEnable)
    {
        return false;
    }
    if (sZoneDifficulty->HardmodeLoot.find(mapId) == sZoneDifficulty->HardmodeLoot.end())
    {
        return false;
    }
    return true;
}

/**
 *  @brief Check if the target is a player, a pet or a guardian.
 *
 * @param target The affected <Unit>
 * @return The result as bool. True for <Player>, <Pet> or <Guardian>.
 */
bool ZoneDifficulty::IsValidNerfTarget(Unit* target)
{
    return target->IsPlayer() || target->IsPet() || target->IsGuardian();
}

/**
 *  @brief Checks if the element is one of the uint32 values in the vector.
 *
 * @param vec A vector
 * @param element One element which can potentially be part of the values in the vector
 * @return The result as bool
 */
bool ZoneDifficulty::VectorContainsUint32(std::vector<uint32> vec, uint32 element)
{
    return find(vec.begin(), vec.end(), element) != vec.end();
}

/**
 *  @brief Checks if the target is in a duel while residing in the DUEL_AREA and their opponent is a valid object.
 *  Used to determine when the duel-specific nerfs should be applied.
 *
 * @param target The affected <Unit>
 * @return The result as bool
 */
bool ZoneDifficulty::ShouldNerfInDuels(Unit* target)
{
    if (target->GetAreaId() != DUEL_AREA)
    {
        return false;
    }

    if (target->ToTempSummon() && target->ToTempSummon()->GetSummoner())
    {
        target = target->ToTempSummon()->GetSummoner()->ToUnit();
    }

    if (!target->GetAffectingPlayer())
    {
        return false;
    }

    if (!target->GetAffectingPlayer()->duel)
    {
        return false;
    }

    if (target->GetAffectingPlayer()->duel->State != DUEL_STATE_IN_PROGRESS)
    {
        return false;
    }

    if (!target->GetAffectingPlayer()->duel->Opponent)
    {
        return false;
    }

    return true;
}

/**
 *  @brief Find the lowest phase for the target's mapId, which has a db entry for the target's map
 *  and at least partly matches the target's phase.
 *
 *  `mapId` can be the id of a map or `DUEL_INDEX` to use the duel specific settings.
 *  Return -1 if none found.
 *
 * @param mapId
 * @param phaseMask Bitmask of all phases where the unit is currently visible
 * @return the lowest phase which should be altered for this map and the unit is visible in
 */
int32 ZoneDifficulty::GetLowestMatchingPhase(uint32 mapId, uint32 phaseMask)
{
    // Check if there is an entry for the mapId at all
    if (sZoneDifficulty->NerfInfo.find(mapId) != sZoneDifficulty->NerfInfo.end())
    {

        // Check if 0 is assigned as a phase to cover all phases
        if (sZoneDifficulty->NerfInfo[mapId].find(0) != sZoneDifficulty->NerfInfo[mapId].end())
        {
            return 0;
        }

        // Check all $key in [mapId][$key] if they match the target's visible phases
        for (auto const& [key, value] : sZoneDifficulty->NerfInfo[mapId])
        {
            if (key & phaseMask)
            {
                return key;
            }
        }
    }
    return -1;
}

/**
 *  @brief Store the HardmodeInstanceData in the database for the given instance id.
 *  zone_difficulty_instance_saves is used to store the data.
 *
 *  @param InstanceID INT NOT NULL DEFAULT 0,
 *  @param HardmodeOn TINYINT NOT NULL DEFAULT 0,
 */
void ZoneDifficulty::SaveHardmodeInstanceData(uint32 instanceId)
{
    if (sZoneDifficulty->HardmodeInstanceData.find(instanceId) == sZoneDifficulty->HardmodeInstanceData.end())
    {
        //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: ZoneDifficulty::SaveHardmodeInstanceData: InstanceId {} not found in HardmodeInstanceData.", instanceId);
        return;
    }
    //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: ZoneDifficulty::SaveHardmodeInstanceData: Saving instanceId {} with HardmodeOn {}", instanceId, sZoneDifficulty->HardmodeInstanceData[instanceId]);
    CharacterDatabase.Execute("REPLACE INTO zone_difficulty_instance_saves (InstanceID, HardmodeOn) VALUES ({}, {})", instanceId, sZoneDifficulty->HardmodeInstanceData[instanceId]);
}

/**
 *  @brief Fetch all players on the [Unit]s threat list
 *
 *  @param unit The one to check for their threat list.
 *  @param entry Key value to access HardmodeAI map/vector data
 *  @param key Key value to access HardmodeAI map/vector
 */
std::list<Unit*> ZoneDifficulty::GetTargetList(Unit* unit, uint32 entry, uint32 key)
{
    auto const& threatlist = unit->GetThreatMgr().GetThreatList();
    std::list<Unit*> targetList;
    if (threatlist.empty())
    {
        //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: Threatlist is empty for unit {}", unit->GetName());
        return targetList;
    }

    for (auto itr = threatlist.begin(); itr != threatlist.end(); ++itr)
    {
        Unit* target = (*itr)->getTarget();
        if (!target)
        {
            continue;
        }
        if (target->GetTypeId() != TYPEID_PLAYER)
        {
            continue;
        }
        //Target chosen based on a distance give in TargetArg (for TARGET_PLAYER_DISTANCE)
        if (sZoneDifficulty->HardmodeAI[entry][key].Target == TARGET_PLAYER_DISTANCE)
        {
            if (!unit->IsWithinDist(target, sZoneDifficulty->HardmodeAI[entry][key].TargetArg))
            {
                continue;
            }
        }
        //Target chosen based on the min/max distance of the spell
        else
        {
            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(sZoneDifficulty->HardmodeAI[entry][key].Spell);
            if (spellInfo)
            {
                if (unit->IsWithinDist(target, spellInfo->GetMinRange()))
                {
                    continue;
                }
                if (!unit->IsWithinDist(target, spellInfo->GetMaxRange()))
                {
                    continue;
                }
            }
        }
        targetList.push_back(target);
    }
    return targetList;
}

void ZoneDifficulty::HardmodeEvent(Unit* unit, uint32 entry, uint32 key)
{
    //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: HardmodeEvent for entry {} with key {}", entry, key);
    if (unit && unit->IsAlive())
    {
        if (!unit->IsInCombat())
        {
            unit->m_Events.CancelEventGroup(EVENT_GROUP);
            return;
        }
        //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: HardmodeEvent IsInCombat for entry {} with key {}", entry, key);
        // Try again in 1s if the unit is currently casting
        if (unit->HasUnitState(UNIT_STATE_CASTING))
        {
            //LOG_INFO("module", "MOD-ZONE-DIFFICULTY: HardmodeEvent Re-schedule AI event in 1s because unit is casting for entry {} with key {}", entry, key);
            unit->m_Events.AddEventAtOffset([unit, entry, key]()
                {
                    sZoneDifficulty->HardmodeEvent(unit, entry, key);
                }, 1s, EVENT_GROUP);
            return;
        }

        //Re-schedule the event
        if (sZoneDifficulty->HardmodeAI[entry][key].Repetitions == 0)
        {
            unit->m_Events.AddEventAtOffset([unit, entry, key]()
                {
                    sZoneDifficulty->HardmodeEvent(unit, entry, key);
                }, sZoneDifficulty->HardmodeAI[entry][key].Cooldown, EVENT_GROUP);
        }

        // Select target. Default = TARGET_SELF
        Unit* target = unit;
        if (sZoneDifficulty->HardmodeAI[entry][key].Target == TARGET_VICTIM)
        {
            target = unit->GetVictim();
        }
        else if (sZoneDifficulty->HardmodeAI[entry][key].Target == TARGET_PLAYER_DISTANCE)
        {
            std::list<Unit*> targetList = sZoneDifficulty->GetTargetList(unit, entry, key);
            if (targetList.empty())
            {
                return;
            }

            bool has_bp0 = sZoneDifficulty->HardmodeAI[entry][key].Spellbp0;
            bool has_bp1 = sZoneDifficulty->HardmodeAI[entry][key].Spellbp1;
            bool has_bp2 = sZoneDifficulty->HardmodeAI[entry][key].Spellbp2;

            for (Unit* trg : targetList)
            {
                if (trg)
                {
                    LOG_INFO("module", "Creature casting HardmodeAI spell: {} at target {}", sZoneDifficulty->HardmodeAI[entry][key].Spell, trg->GetName());
                    if (!has_bp0 && !has_bp1 && !has_bp2)
                    {
                        unit->CastSpell(trg, sZoneDifficulty->HardmodeAI[entry][key].Spell, true);
                        LOG_INFO("module", "Creature casting HardmodeAI spell: {} at target {}", sZoneDifficulty->HardmodeAI[entry][key].Spell, trg->GetName());
                    }
                    else
                    {
                        unit->CastCustomSpell(trg, sZoneDifficulty->HardmodeAI[entry][key].Spell,
                            has_bp0 ? &sZoneDifficulty->HardmodeAI[entry][key].Spellbp0 : NULL,
                            has_bp1 ? &sZoneDifficulty->HardmodeAI[entry][key].Spellbp1 : NULL,
                            has_bp2 ? &sZoneDifficulty->HardmodeAI[entry][key].Spellbp2 : NULL,
                            true);
                        LOG_INFO("module", "Creature casting HardmodeAI spell: {} at target {} with custom values.", sZoneDifficulty->HardmodeAI[entry][key].Spell, trg->GetName());
                    }
                }
            }
            return;
        }
        else
        {
            std::list<Unit*> targetList = sZoneDifficulty->GetTargetList(unit, entry, key);
            if (targetList.empty())
            {
                return;
            }
            if (targetList.size() < 2)
            {
                target = unit->GetVictim();
            }
            else
            {
                uint8 counter = targetList.size() - 1;
                if (sZoneDifficulty->HardmodeAI[entry][key].TargetArg > counter)
                {
                    counter = sZoneDifficulty->HardmodeAI[entry][key].TargetArg;
                }

                switch (sZoneDifficulty->HardmodeAI[entry][key].Target)
                {
                case TARGET_HOSTILE_AGGRO_FROM_TOP:
                {
                    std::list<Unit*>::const_iterator itr = targetList.begin();
                    std::advance(itr, counter);
                    target = *itr;
                    break;
                }
                case TARGET_HOSTILE_AGGRO_FROM_BOTTOM:
                {
                    std::list<Unit*>::reverse_iterator ritr = targetList.rbegin();
                    std::advance(ritr, counter);
                    target = *ritr;
                    break;
                }
                case TARGET_HOSTILE_RANDOM:
                {
                    std::list<Unit*>::const_iterator itr = targetList.begin();
                    std::advance(itr, urand(0, targetList.size() - 1));
                    target = *itr;
                    break;
                }
                case TARGET_HOSTILE_RANDOM_NOT_TOP:
                {
                    std::list<Unit*>::const_iterator itr = targetList.begin();
                    std::advance(itr, urand(1, targetList.size() - 1));
                    target = *itr;
                    break;
                }
                default:
                {
                    LOG_ERROR("module", "MOD-ZONE-DIFFICULTY: Unknown type for Target: {} in zone_difficulty_hardmode_ai", sZoneDifficulty->HardmodeAI[entry][key].Target);
                }
                }
            }
        }

        if (target)
        {
            bool has_bp0 = sZoneDifficulty->HardmodeAI[entry][key].Spellbp0;
            bool has_bp1 = sZoneDifficulty->HardmodeAI[entry][key].Spellbp1;
            bool has_bp2 = sZoneDifficulty->HardmodeAI[entry][key].Spellbp2;
            LOG_INFO("module", "Creature casting HardmodeAI spell: {} at target {}", sZoneDifficulty->HardmodeAI[entry][key].Spell, target->GetName());
            if (!has_bp0 && !has_bp1 && !has_bp2)
            {
                unit->CastSpell(target, sZoneDifficulty->HardmodeAI[entry][key].Spell, true);
            }
            else
            {
                unit->CastCustomSpell(target, sZoneDifficulty->HardmodeAI[entry][key].Spell,
                    has_bp0 ? &sZoneDifficulty->HardmodeAI[entry][key].Spellbp0 : NULL,
                    has_bp1 ? &sZoneDifficulty->HardmodeAI[entry][key].Spellbp1 : NULL,
                    has_bp2 ? &sZoneDifficulty->HardmodeAI[entry][key].Spellbp2 : NULL,
                    true);
            }
        }
    }
}

bool ZoneDifficulty::HasCompletedFullTier(uint32 category, uint32 playerGuid)
{
    //LOG_INFO("module", "MOD-ZONE-DIFFCULTY: Executing HasCompletedFullTier for category {} playerGUID {}.", category, playerGuid);
    std::vector<uint32> MapList;
    switch (category)
    {
    case TYPE_HEROIC_TBC:
        //585 is Magister's Terrace. Only add when released.
        MapList = { 269, 540, 542, 543, 545, 547, 546, 552, 553, 554, 555, 556, 557, 558, 560/*, 585*/ };
        break;
    case TYPE_RAID_T4:
        MapList = { 532, 544, 565};
        break;
    default:
        LOG_ERROR("module", "MOD-ZONE-DIFFICULTY: Category without data requested in ZoneDifficulty::HasCompletedFullTier {}", category);
        return false;
        break;
    }

    for (uint32 mapId : MapList)
    {
        //LOG_INFO("module", "MOD-ZONE-DIFFCULTY: Checking HasCompletedFullTier for mapId {}.", mapId);
        if (sZoneDifficulty->EncounterCounter.find(mapId) == sZoneDifficulty->EncounterCounter.end())
        {
            LOG_ERROR("module", "MOD-ZONE-DIFFICULTY: Map without data requested in ZoneDifficulty::HasCompletedFullTier {}", mapId);
            return false;
        }
        for (uint8 i = 0; i < sZoneDifficulty->EncounterCounter[mapId]; ++i)
        {
            //LOG_INFO("module", "MOD-ZONE-DIFFCULTY: Checking HasCompletedFullTier for BossId {}: {}.", i, sZoneDifficulty->Logs[playerGuid][mapId][i]);
            if (!sZoneDifficulty->Logs[playerGuid][mapId][i])
            {
                return false;
            }
        }
    }
    return true;
}

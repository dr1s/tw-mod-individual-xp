#include "ScriptObjects.h"
#include "Config/Config.h"
#include "Database/DatabaseEnv.h"
#include "Chat.h"
#include "ObjectMgr.h"
#include "Player.h"

#include <cmath>
#include <cstdlib>
#include <map>

namespace
{
    struct IndividualXpModule
    {
        bool Enabled = true;
        bool AnnounceModule = true;
        bool AnnounceRatesOnLogin = true;
        float MaxRate = 10.0f;
        float DefaultRate = 1.0f;
    };

    IndividualXpModule s_individualXp;

    std::map<ObjectGuid, float> s_playerXpRates;

    enum IndividualXpString
    {
        INDIVIDUAL_XP_CREDIT = 100000,
        INDIVIDUAL_XP_MODULE_DISABLED,
        INDIVIDUAL_XP_COMMAND_VIEW,
        INDIVIDUAL_XP_MAX_RATE,
        INDIVIDUAL_XP_MIN_RATE,
        INDIVIDUAL_XP_COMMAND_SET,
        INDIVIDUAL_XP_COMMAND_DEFAULT
    };

    static const char* GetIndividualXpString(uint32 id, int localeIdx)
    {
        return sObjectMgr.GetModuleString("tw-mod-individual-xp", id, localeIdx);
    }

    static float* GetPlayerXpRate(Player* player, bool createDefault = false)
    {
        auto itr = s_playerXpRates.find(player->GetObjectGuid());
        if (itr != s_playerXpRates.end())
            return &itr->second;

        if (!createDefault)
            return nullptr;

        auto emplaceResult = s_playerXpRates.emplace(player->GetObjectGuid(), s_individualXp.DefaultRate);
        return &emplaceResult.first->second;
    }

    class IndividualXpWorldScript : public WorldScript
    {
    public:
        IndividualXpWorldScript()
            : WorldScript("IndividualXpWorldScript", { WORLDHOOK_ON_BEFORE_CONFIG_LOAD })
        {
        }

        void OnBeforeConfigLoad(bool /*reload*/) override
        {
            s_individualXp.Enabled = sConfig.GetBoolDefault("IndividualXp.Enabled", true);
            s_individualXp.AnnounceModule = sConfig.GetBoolDefault("IndividualXp.Announce", true);
            s_individualXp.AnnounceRatesOnLogin = sConfig.GetBoolDefault("IndividualXp.AnnounceRatesOnLogin", true);
            s_individualXp.MaxRate = sConfig.GetFloatDefault("IndividualXp.MaxXPRate", 10.0f);
            s_individualXp.DefaultRate = sConfig.GetFloatDefault("IndividualXp.DefaultXPRate", 1.0f);
        }
    };

    class IndividualXpPlayerScript : public PlayerScript
    {
    public:
        IndividualXpPlayerScript()
            : PlayerScript("IndividualXpPlayerScript", { PLAYERHOOK_ON_LOGIN, PLAYERHOOK_ON_LOGOUT, PLAYERHOOK_ON_GIVE_EXP })
        {
        }

        void OnLogin(Player* player) override
        {
            QueryResult* result = CharacterDatabase.PQuery(
                "SELECT `XPRate` FROM `individualxp` WHERE `CharacterGUID`=%u",
                player->GetGUIDLow());

            if (result)
            {
                s_playerXpRates[player->GetObjectGuid()] = result->Fetch()[0].GetFloat();
                delete result;
            }
            else
            {
                s_playerXpRates[player->GetObjectGuid()] = s_individualXp.DefaultRate;
            }

            if (!s_individualXp.Enabled)
                return;

            ChatHandler handler(player->GetSession());
            int localeIdx = player->GetSession()->GetSessionDbLocaleIndex();

            if (s_individualXp.AnnounceModule)
                handler.SendSysMessage(GetIndividualXpString(INDIVIDUAL_XP_CREDIT, localeIdx));

            if (!s_individualXp.AnnounceRatesOnLogin)
                return;

            float* rate = GetPlayerXpRate(player);
            if (rate)
            {
                handler.PSendSysMessage(GetIndividualXpString(INDIVIDUAL_XP_COMMAND_VIEW, localeIdx), *rate);
                handler.PSendSysMessage(GetIndividualXpString(INDIVIDUAL_XP_MAX_RATE, localeIdx), s_individualXp.MaxRate);
            }
        }

        void OnLogout(Player* player) override
        {
            auto itr = s_playerXpRates.find(player->GetObjectGuid());
            if (itr == s_playerXpRates.end())
                return;

            CharacterDatabase.DirectPExecute(
                "REPLACE INTO `individualxp` (`CharacterGUID`, `XPRate`) VALUES (%u, %f)",
                player->GetGUIDLow(), itr->second);

            s_playerXpRates.erase(itr);
        }

        void OnGiveXP(Player* player, uint32& amount, Unit* /*victim*/) override
        {
            if (!s_individualXp.Enabled || amount == 0)
                return;

            float* rate = GetPlayerXpRate(player);
            if (!rate)
                return;

            amount = static_cast<uint32>(std::round(static_cast<float>(amount) * *rate));
        }
    };

    class IndividualXpCommand : public CommandScript
    {
    public:
        IndividualXpCommand() : CommandScript("IndividualXpCommand") {}

        std::vector<ChatCommand> GetCommands() const override
        {
            static std::vector<ChatCommand> IndividualXpCommandTable = {
                { "view",    SEC_PLAYER, false, nullptr, "", nullptr, 0, "", 0, HandleViewCommand },
                { "set",     SEC_PLAYER, false, nullptr, "", nullptr, 0, "", 0, HandleSetCommand },
                { "default", SEC_PLAYER, false, nullptr, "", nullptr, 0, "", 0, HandleDefaultCommand },
                { nullptr,   0,          false, nullptr, "", nullptr, 0, "", 0, nullptr }
            };

            static std::vector<ChatCommand> IndividualXpBaseTable = {
                { "xprate", SEC_PLAYER, false, nullptr, "", IndividualXpCommandTable.data(), 0, "", 0, nullptr },
                { nullptr, 0, false, nullptr, "", nullptr, 0, "", 0, nullptr }
            };

            return IndividualXpBaseTable;
        }

        static bool HandleViewCommand(ChatHandler* handler, char* /*args*/)
        {
            if (!s_individualXp.Enabled)
            {
                handler->PSendSysMessage(GetIndividualXpString(INDIVIDUAL_XP_MODULE_DISABLED, handler->GetSession()->GetSessionDbLocaleIndex()));
                return false;
            }

            Player* player = handler->GetSession()->GetPlayer();
            if (!player)
                return false;

            float* rate = GetPlayerXpRate(player);
            int localeIdx = handler->GetSession()->GetSessionDbLocaleIndex();

            if (!rate)
            {
                handler->PSendSysMessage(GetIndividualXpString(INDIVIDUAL_XP_COMMAND_VIEW, localeIdx), s_individualXp.DefaultRate);
                handler->PSendSysMessage(GetIndividualXpString(INDIVIDUAL_XP_MAX_RATE, localeIdx), s_individualXp.MaxRate);
                return true;
            }

            handler->PSendSysMessage(GetIndividualXpString(INDIVIDUAL_XP_COMMAND_VIEW, localeIdx), *rate);
            handler->PSendSysMessage(GetIndividualXpString(INDIVIDUAL_XP_MAX_RATE, localeIdx), s_individualXp.MaxRate);
            return true;
        }

        static bool HandleSetCommand(ChatHandler* handler, char* args)
        {
            if (!s_individualXp.Enabled)
            {
                handler->PSendSysMessage(GetIndividualXpString(INDIVIDUAL_XP_MODULE_DISABLED, handler->GetSession()->GetSessionDbLocaleIndex()));
                return false;
            }

            if (!args || !*args)
                return false;

            float rate = static_cast<float>(atof(args));
            if (rate == 0.0f)
                return false;

            Player* player = handler->GetSession()->GetPlayer();
            if (!player)
                return false;

            int localeIdx = handler->GetSession()->GetSessionDbLocaleIndex();

            if (rate > s_individualXp.MaxRate)
            {
                handler->PSendSysMessage(GetIndividualXpString(INDIVIDUAL_XP_MAX_RATE, localeIdx), s_individualXp.MaxRate);
                return false;
            }

            if (rate < 0.1f)
            {
                handler->PSendSysMessage(GetIndividualXpString(INDIVIDUAL_XP_MIN_RATE, localeIdx));
                return false;
            }

            float* storedRate = GetPlayerXpRate(player, true);
            *storedRate = rate;
            handler->PSendSysMessage(GetIndividualXpString(INDIVIDUAL_XP_COMMAND_SET, localeIdx), rate);
            return true;
        }

        static bool HandleDefaultCommand(ChatHandler* handler, char* /*args*/)
        {
            if (!s_individualXp.Enabled)
            {
                handler->PSendSysMessage(GetIndividualXpString(INDIVIDUAL_XP_MODULE_DISABLED, handler->GetSession()->GetSessionDbLocaleIndex()));
                return false;
            }

            Player* player = handler->GetSession()->GetPlayer();
            if (!player)
                return false;

            float* rate = GetPlayerXpRate(player, true);
            int localeIdx = handler->GetSession()->GetSessionDbLocaleIndex();

            *rate = s_individualXp.DefaultRate;
            handler->PSendSysMessage(GetIndividualXpString(INDIVIDUAL_XP_COMMAND_DEFAULT, localeIdx), s_individualXp.DefaultRate);
            return true;
        }
    };
}

void AddIndividualXpScripts()
{
    new IndividualXpWorldScript();
    new IndividualXpPlayerScript();
    new IndividualXpCommand();
}

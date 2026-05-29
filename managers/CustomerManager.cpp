#include "CustomerManager.h"
#include "AssetManager.h"
#include "../classes/Customer.h"
#include "../classes/QTEEvent.h"
#include "raylib.h"

#include <vector>
#include <string>
#include <memory>

// =====================================================
// QTE SETY
// =====================================================

static std::vector<QTEEvent> MakeNormalQTE()
{
    return {
        {
            "normal_hello",
            "Zakaznik: Dobry den.",
            KEY_R,
            10.0f,
            "Ty: Dobry den!",
            "Zakaznik: No... tak nic.",
            2,
            -3,
            MOOD_ANY,
            "Pozdravit slusne",
            "Nabidnout tasku",
            "Rict pravidla obchodu",
            "Zacit rychle markovat"
        },
        {
            "normal_bag",
            "Zakaznik: Mate tasku?",
            KEY_T,
            10.0f,
            "Ty: Ano, hned vam ji dam.",
            "Zakaznik: Tak to si ji necham ujit.",
            2,
            -4,
            MOOD_ANY,
            "Omluvit se",
            "Nabidnout tasku",
            "Rict, ze tasky jsou placene",
            "Ignorovat a pokracovat"
        }
    };
}

static std::vector<QTEEvent> MakeRushingQTE()
{
    return {
        {
            "rushing_bus",
            "Zakaznik: Specham na autobus!",
            KEY_U,
            10.0f,
            "Ty: Hned to bude.",
            "Zakaznik: To snad neni mozny!",
            2,
            -8,
            MOOD_ANY,
            "Uklidnit zakaznika",
            "Nabidnout tasku",
            "Vysvetlit pravidla",
            "Zrychlit markovani"
        },
        {
            "rushing_fast",
            "Zakaznik: Muzete trochu rychleji?",
            KEY_U,
            10.0f,
            "Ty: Jasne, zrychlim.",
            "Zakaznik: Ja tu snad zestarne.",
            1,
            -6,
            MOOD_ANY,
            "Rict, ze to nejde",
            "Nabidnout pomoc",
            "Rict, ze je fronta",
            "Zrychlit"
        }
    };
}

static std::vector<QTEEvent> MakeAngryQTE()
{
    return {
        {
            "angry_complaint",
            "Zakaznik: Proc to tak dlouho trva?!",
            KEY_R,
            10.0f,
            "Ty: Omlouvam se, hned to bude.",
            "Zakaznik: To je otresny servis!",
            3,
            -8,
            MOOD_ANY,
            "Omluvit se",
            "Nabidnout pomoc",
            "Rict pravidla obchodu",
            "Ignorovat a zrychlit"
        },
        {
            "angry_price",
            "Zakaznik: To je nejaka drahota!",
            KEY_T,
            10.0f,
            "Ty: Muzeme zkontrolovat Clubcard cenu.",
            "Zakaznik: No jasne, vymluvy.",
            2,
            -5,
            MOOD_ANY,
            "Souhlasit a mlcet",
            "Nabidnout kontrolu ceny",
            "Rict, ze ceny neurcujes",
            "Rychle pokracovat"
        }
    };
}

static std::vector<QTEEvent> MakeTalkativeQTE()
{
    return {
        {
            "talkative_weather",
            "Zakaznik: To je dneska ale pocasi, co?",
            KEY_R,
            10.0f,
            "Ty: No jo, idealni den na smenu.",
            "Zakaznik: Vy jste teda moc ukecany ne.",
            2,
            -3,
            MOOD_ANY,
            "Mile odpovedet",
            "Nabidnout tasku",
            "Rict, ze mas praci",
            "Zrychlit markovani"
        },
        {
            "talkative_story",
            "Zakaznik: Ja sem chodim uz dvacet let.",
            KEY_R,
            10.0f,
            "Ty: Tak to jste verny zakaznik.",
            "Zakaznik: Dneska uz nikoho nic nezajima.",
            3,
            -4,
            MOOD_ANY,
            "Zareagovat mile",
            "Nabidnout Clubcard",
            "Rict pravidla",
            "Pokracovat bez reakce"
        }
    };
}

static std::vector<QTEEvent> MakeConfusedQTE()
{
    return {
        {
            "confused_clubcard",
            "Zakaznik: A ta Clubcard je co?",
            KEY_T,
            10.0f,
            "Ty: Muzete s ni mit slevy.",
            "Zakaznik: Ja tomu nerozumim.",
            2,
            -4,
            MOOD_ANY,
            "Rict, ze to nevis",
            "Vysvetlit Clubcard",
            "Rict obchodni podminky",
            "Zrychlit"
        },
        {
            "confused_payment",
            "Zakaznik: Kde se tady plati?",
            KEY_T,
            10.0f,
            "Ty: Tady u kasy, prosim.",
            "Zakaznik: To je nejaky slozity.",
            2,
            -5,
            MOOD_ANY,
            "Zasmát se",
            "Vysvetlit platbu",
            "Rict pravidla",
            "Ignorovat"
        }
    };
}

static std::vector<QTEEvent> MakeRegularNovakovaQTE()
{
    return {
        {
            "novakova_rohliky",
            "Pani Novakova: Mate dneska cerstve rohliky?",
            KEY_T,
            10.0f,
            "Ty: Ano, dneska rano dovezene.",
            "Pani Novakova: Tak to je bida.",
            3,
            -5,
            MOOD_ANY,
            "Rict, ze nevis",
            "Odpovedet vecne",
            "Rict pravidla pekarny",
            "Rychle markovat"
        },
        {
            "novakova_ceny",
            "Pani Novakova: Zase zdrazili maslo?",
            KEY_R,
            10.0f,
            "Ty: Bohuzel, ceny leti nahoru.",
            "Pani Novakova: Vy mladi tomu nerozumite.",
            2,
            -4,
            MOOD_ANY,
            "Empaticky odpovedet",
            "Nabidnout Clubcard cenu",
            "Rict, ze ceny neurcujes",
            "Ignorovat"
        }
    };
}

// =====================================================
// HLASKY PRI PRICHODU A ODCHODU
// =====================================================

static void AddDefaultLines(std::shared_ptr<Customer> customer)
{
    customer->arrivalLines = {
        "Dobry den.",
        "Zdravim.",
        "Tak jsem tady.",
        "Muzeme?",
        "Dobry."
    };

    customer->exitHappyLines = {
        "Diky, nashle!",
        "Hezky den.",
        "To slo krasne rychle.",
        "Super, dekuju."
    };

    customer->exitNeutralLines = {
        "Nashle.",
        "Tak jo.",
        "Mejte se.",
        "Diky."
    };

    customer->exitAnnoyedLines = {
        "No, priste rychleji.",
        "To teda trvalo.",
        "Hlavne ze to mam za sebou.",
        "Dneska nic moc."
    };

    customer->exitAngryLines = {
        "Sem uz nejdu!",
        "Tohle byl zazitek...",
        "Managera na vas!",
        "To si budu pamatovat."
    };
}

static void AddNovakovaLines(std::shared_ptr<Customer> customer)
{
    customer->arrivalLines = {
        "Pani Novakova: Dobry den, zase jsem tady.",
        "Pani Novakova: Snad mate dneska cerstve rohliky.",
        "Pani Novakova: Tak copak mate v akci?",
        "Pani Novakova: Dneska to snad pujde rychle."
    };

    customer->exitHappyLines = {
        "Pani Novakova: Dekuju, zlato.",
        "Pani Novakova: Dneska vam to slo.",
        "Pani Novakova: Prijdu zas."
    };

    customer->exitNeutralLines = {
        "Pani Novakova: Tak nashle.",
        "Pani Novakova: No dobre.",
        "Pani Novakova: Mejte se."
    };

    customer->exitAnnoyedLines = {
        "Pani Novakova: Dneska nic moc.",
        "Pani Novakova: To za nas nebyvalo.",
        "Pani Novakova: Priste se vic snazte."
    };

    customer->exitAngryLines = {
        "Pani Novakova: To si budu pamatovat.",
        "Pani Novakova: Tohle reknu sousedce.",
        "Pani Novakova: Ostuda!"
    };
}

// =====================================================
// CUSTOMER MANAGER
// =====================================================

std::shared_ptr<Customer> CustomerManager::CreateCustomer()
{
    // 30 % sance na pravidelneho zakaznika
    if (GetRandomValue(1, 100) <= 30) {
        return CreateRegularCustomer();
    }

    return CreateRandomCustomer();
}

std::shared_ptr<Customer> CustomerManager::CreateRegularCustomer()
{
    auto customer = std::make_shared<Customer>(
        "pani_novakova",
        "Pani Novakova",
        GetRandomValue(65, 82),
        true,
        true,
        ARCH_REGULAR,
        35
    );

    customer->favoriteItemIds = {
        "rohlik",
        "chleba",
        "maslo"
    };

    customer->qteSet = MakeRegularNovakovaQTE();
    AddNovakovaLines(customer);

    return customer;
}

std::shared_ptr<Customer> CustomerManager::CreateRandomCustomer()
{
    int archetypeRoll = GetRandomValue(1, 5);

    CustomerArchetype archetype = ARCH_NORMAL;
    std::string id = "random";
    std::string name = "Zakaznik";
    int patience = 25;

    if (archetypeRoll == 1) {
        archetype = ARCH_NORMAL;
        name = "Zakaznik";
        patience = 25;
    }
    else if (archetypeRoll == 2) {
        archetype = ARCH_RUSHING;
        name = "Spechajici zakaznik";
        patience = 18;
    }
    else if (archetypeRoll == 3) {
        archetype = ARCH_TALKATIVE;
        name = "Ukecany zakaznik";
        patience = 34;
    }
    else if (archetypeRoll == 4) {
        archetype = ARCH_ANGRY;
        name = "Nastvany zakaznik";
        patience = 20;
    }
    else {
        archetype = ARCH_CONFUSED;
        name = "Zmateny zakaznik";
        patience = 30;
    }

    auto customer = std::make_shared<Customer>(
        id,
        name,
        GetRandomValue(15, 80),
        GetRandomValue(1, 100) <= 75,
        false,
        archetype,
        patience
    );

    if (archetype == ARCH_RUSHING) {
        customer->favoriteItemIds = { "energy_drink", "pizza", "muffin" };
        customer->qteSet = MakeRushingQTE();
    }
    else if (archetype == ARCH_TALKATIVE) {
        customer->favoriteItemIds = { "rohlik", "chleba", "kava" };
        customer->qteSet = MakeTalkativeQTE();
    }
    else if (archetype == ARCH_ANGRY) {
        customer->favoriteItemIds = { "pivo", "rum", "chipsy" };
        customer->qteSet = MakeAngryQTE();
    }
    else if (archetype == ARCH_CONFUSED) {
        customer->favoriteItemIds = { "mleko", "chleba", "banan" };
        customer->qteSet = MakeConfusedQTE();
    }
    else {
        customer->favoriteItemIds = { "mleko", "rohlik", "jogurt" };
        customer->qteSet = MakeNormalQTE();
    }

    AddDefaultLines(customer);

    return customer;
}

ItemTemplate CustomerManager::PickItemForCustomer(const Customer& customer)
{

    if (!customer.favoriteItemIds.empty() && GetRandomValue(1, 100) <= 60) {
        std::string wantedId = customer.favoriteItemIds[
            GetRandomValue(0, (int)customer.favoriteItemIds.size() - 1)
        ];

        for (const ItemTemplate& item : AssetManager::itemDatabase) {
            if (item.id == wantedId) {
                return item;
            }
        }
    }

    return AssetManager::GetRandomItemTemplate();
}

QTEEvent CustomerManager::PickQTEForCustomer(const Customer& customer)
{
    std::vector<QTEEvent> possible;

    for (const QTEEvent& qte : customer.qteSet) {
        if (qte.requiredMood == MOOD_ANY || qte.requiredMood == customer.mood) {
            possible.push_back(qte);
        }
    }

    if (!possible.empty()) {
        return possible[GetRandomValue(0, (int)possible.size() - 1)];
    }

    if (!customer.qteSet.empty()) {
        return customer.qteSet[GetRandomValue(0, (int)customer.qteSet.size() - 1)];
    }

    return QTEEvent{};
}
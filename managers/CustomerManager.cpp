#include "CustomerManager.h"
#include "AssetManager.h"
#include "../classes/Customer.h"
#include "../classes/QTEEvent.h"
#include "raylib.h"

#include <vector>
#include <string>
#include <memory>

static Texture2D GetCustomerTexture(const std::string& customerId, CustomerArchetype archetype, bool isRegular);
static std::shared_ptr<Customer> CreateShrekCustomer();
static std::shared_ptr<Customer> CreateBarnsCustomer();
static std::shared_ptr<Customer> CreateDoctorCustomer();
static std::shared_ptr<Customer> CreateChloeCustomer();
static std::shared_ptr<Customer> CreateBigKrtkusCustomer();
static std::shared_ptr<Customer> CreateHonzaTunaCustomer();
static std::shared_ptr<Customer> CreateHonzaSpacekCustomer();
static std::shared_ptr<Customer> CreateSugarDennyCustomer();
static std::shared_ptr<Customer> CreateAlastorCustomer();

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
    int roll = GetRandomValue(1, 9);

    switch (roll) {
        case 1: return CreateShrekCustomer();
        case 2: return CreateBarnsCustomer();
        case 3: return CreateDoctorCustomer();
        case 4: return CreateChloeCustomer();
        case 5: return CreateBigKrtkusCustomer();
        case 6: return CreateHonzaTunaCustomer();
        case 7: return CreateHonzaSpacekCustomer();
        case 8: return CreateSugarDennyCustomer();
        case 9: return CreateAlastorCustomer();
        default: return CreateShrekCustomer();
    }
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
        patience,
        GetCustomerTexture(id, archetype, false)
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

static Texture2D GetCustomerTexture(const std::string& customerId, CustomerArchetype archetype, bool isRegular)
{
    static Texture2D normalTex = LoadTexture("ASSets/textures/customers/normal.png");
    static Texture2D rushingTex = LoadTexture("ASSets/textures/customers/rushing.png");
    static Texture2D talkativeTex = LoadTexture("ASSets/textures/customers/talkative.png");
    static Texture2D angryTex = LoadTexture("ASSets/textures/customers/angry.png");
    static Texture2D confusedTex = LoadTexture("ASSets/textures/customers/confused.png");

    static Texture2D shrekTex = LoadTexture("ASSets/textures/customers/shrek.png");
    static Texture2D barnsTex = LoadTexture("ASSets/textures/customers/barns_courtney.png");
    static Texture2D doctorTex = LoadTexture("ASSets/textures/customers/10th_doc.png");
    static Texture2D chloeTex = LoadTexture("ASSets/textures/customers/chloe.png");
    static Texture2D krtkusTex = LoadTexture("ASSets/textures/customers/krtkus.png");

    static Texture2D tunaTex = LoadTexture("ASSets/textures/customers/tuna.png");
    static Texture2D spacekTex = LoadTexture("ASSets/textures/customers/spacek.png");
    static Texture2D sugarTex = LoadTexture("ASSets/textures/customers/denny.png");
    static Texture2D alastorTex = LoadTexture("ASSets/textures/customers/alastor.png");

    if (isRegular) {
        if (customerId == "shrek") return shrekTex;
        if (customerId == "barns_courtney") return barnsTex;
        if (customerId == "doctor10") return doctorTex;
        if (customerId == "chloe_price") return chloeTex;
        if (customerId == "big_krtkus") return krtkusTex;
        if (customerId == "honza_tuna") return tunaTex;
        if (customerId == "honza_spacek") return spacekTex;
        if (customerId == "sugar_denny") return sugarTex;
        if (customerId == "alastor") return alastorTex;
    }

    if (archetype == ARCH_RUSHING) return rushingTex;
    if (archetype == ARCH_TALKATIVE) return talkativeTex;
    if (archetype == ARCH_ANGRY) return angryTex;
    if (archetype == ARCH_CONFUSED) return confusedTex;

    return normalTex;
}

//----------------------------

static std::shared_ptr<Customer> CreateShrekCustomer()
{
    auto customer = std::make_shared<Customer>(
        "shrek",
        "Shrek",
        GetRandomValue(30, 50),
        false,
        true,
        ARCH_REGULAR,
        40,
        GetCustomerTexture("shrek", ARCH_REGULAR, true)
    );

    customer->favoriteItemIds = {
        "cibule", "chleba", "mleko"
    };

    customer->qteSet = {
        {
            "shrek_bazina",
            "Shrek: Co delas v moji baz... teda v Tescu?!",
            KEY_R,
            10.0f,
            "Ty: Jen markuju nakup, pane.",
            "Shrek: Hm. Podezrely.",
            2,
            -5,
            MOOD_ANY,
            "Uklidnit ho",
            "Nabidnout tasku",
            "Rict pravidla",
            "Rychle markovat"
        },
        {
            "shrek_cibule",
            "Shrek: Mas cibule? Ogry jsou jako cibule.",
            KEY_T,
            10.0f,
            "Ty: Jasne, cibule jsou na pasu.",
            "Shrek: Tak tohle nema vrstvy.",
            3,
            -4,
            MOOD_ANY,
            "Rict, ze chapu",
            "Ukazat cibule",
            "Rict pravidla zeleniny",
            "Ignorovat"
        }
    };

    customer->arrivalLines = {
        "Shrek: Zdravim.",
        "Shrek: Kde mate cibule?",
        "Shrek: Doufam, ze to pujde rychle.",
        "Shrek: Nerad opoustim bazinu."
    };

    customer->exitHappyLines = {
        "Shrek: Dobry. To slo.",
        "Shrek: Hm. Spokojenost.",
        "Shrek: Cibule zachraneny."
    };

    customer->exitNeutralLines = {
        "Shrek: Tak jo.",
        "Shrek: Nashle.",
        "Shrek: Hm."
    };

    customer->exitAnnoyedLines = {
        "Shrek: To trvalo.",
        "Shrek: Priste rychleji.",
        "Shrek: Moje bazina ceka."
    };

    customer->exitAngryLines = {
        "Shrek: Tohle byl omyl.",
        "Shrek: Jdu zpatky do baziny.",
        "Shrek: Ogr nespokojen."
    };

    return customer;
}

static std::shared_ptr<Customer> CreateBarnsCustomer()
{
    auto customer = std::make_shared<Customer>(
        "barns_courtney",
        "Barns Courtney",
        GetRandomValue(28, 40),
        false,
        true,
        ARCH_REGULAR,
        26,
        GetCustomerTexture("barns_courtney", ARCH_REGULAR, true)
    );

    customer->favoriteItemIds = {
        "energy_drink", "pizza", "chipsy"
    };

    customer->qteSet = {
        {
            "barns_english_1",
            "Barns: Uh... hello? Sorry, I don't speak Czech.",
            KEY_R,
            10.0f,
            "Ty: Uuu... hello... yes?",
            "Barns: Eee... what?",
            2,
            -3,
            MOOD_ANY,
            "Uuu... hello?",
            "Taska? Bag? Eee?",
            "Pravidla... Czech only?",
            "Eee... markovat?"
        },
        {
            "barns_english_2",
            "Barns: Card? Cash? Tesco? I have no idea.",
            KEY_U,
            10.0f,
            "Ty: Uuu eee okay, I scan fast.",
            "Barns: This is getting worse.",
            1,
            -5,
            MOOD_ANY,
            "Yes yes?",
            "No no?",
            "Clubcard maybe?",
            "Uuu eee scan scan"
        }
    };

    customer->arrivalLines = {
        "Barns: Uh... hi.",
        "Barns: Hello... maybe?",
        "Barns: Tesco, yes.",
        "Barns: Uhhhhh."
    };

    customer->exitHappyLines = {
        "Barns: Nice. Thanks.",
        "Barns: Cool cool.",
        "Barns: Okay bye."
    };

    customer->exitNeutralLines = {
        "Barns: Bye.",
        "Barns: Alright.",
        "Barns: Yeah."
    };

    customer->exitAnnoyedLines = {
        "Barns: Eh... weird.",
        "Barns: Bit awkward.",
        "Barns: Hmm, okay."
    };

    customer->exitAngryLines = {
        "Barns: I have no words.",
        "Barns: Uhhh... no.",
        "Barns: Disaster."
    };

    return customer;
}


static std::shared_ptr<Customer> CreateDoctorCustomer()
{
    auto customer = std::make_shared<Customer>(
        "doctor10",
        "10th Doctor",
        GetRandomValue(30, 40),
        true,
        true,
        ARCH_REGULAR,
        38,
        GetCustomerTexture("doctor10", ARCH_REGULAR, true)
    );

    customer->favoriteItemIds = {
        "kava", "chipsy", "cola"
    };

    customer->qteSet = {
        {
            "doctor_cas",
            "Doktor: Rychle! Cas je velmi dulezity!",
            KEY_U,
            10.0f,
            "Ty: Jasne, hned to bude.",
            "Doktor: Ztracime drahocenne sekundy!",
            2,
            -6,
            MOOD_ANY,
            "Uklidnit ho",
            "Nabidnout pomoc",
            "Rict pravidla",
            "Zrychlit"
        },
        {
            "doctor_lidi",
            "Doktor: Lide. Vzdycky nakupuji v nejhorsi mozny moment.",
            KEY_R,
            10.0f,
            "Ty: To se stava.",
            "Doktor: Hm. Velmi lidske.",
            2,
            -4,
            MOOD_ANY,
            "Odpovedet mile",
            "Nabidnout tasku",
            "Ignorovat",
            "Zrychlit"
        }
    };

    customer->arrivalLines = {
        "Doktor: Vyborne!",
        "Doktor: Tak jdeme na to!",
        "Doktor: Zdravim!",
        "Doktor: Ooo, obchod!"
    };

    customer->exitHappyLines = {
        "Doktor: Fantasticke!",
        "Doktor: Skvela prace.",
        "Doktor: Allons-y!"
    };

    customer->exitNeutralLines = {
        "Doktor: Dobre.",
        "Doktor: Pokracuji dal.",
        "Doktor: Dekuji."
    };

    customer->exitAnnoyedLines = {
        "Doktor: Trochu pomale.",
        "Doktor: Mohlo to byt rychlejsi.",
        "Doktor: Hm."
    };

    customer->exitAngryLines = {
        "Doktor: Tak tohle bylo spatne.",
        "Doktor: Velmi spatne.",
        "Doktor: Ne. Proste ne."
    };

    return customer;
}

static std::shared_ptr<Customer> CreateChloeCustomer()
{
    auto customer = std::make_shared<Customer>(
        "chloe_price",
        "Chloe Price",
        GetRandomValue(19, 24),
        false,
        true,
        ARCH_REGULAR,
        22,
        GetCustomerTexture("chloe_price", ARCH_REGULAR, true)
    );

    customer->favoriteItemIds = {
        "energy_drink", "chipsy", "pizza"
    };

    customer->qteSet = {
        {
            "chloe_pomalu",
            "Chloe: Vazne? Tohle trva vecnost.",
            KEY_R,
            10.0f,
            "Ty: Sorry, hned to bude.",
            "Chloe: Jo, jasne.",
            2,
            -5,
            MOOD_ANY,
            "Omluvit se",
            "Nabidnout pomoc",
            "Rict pravidla",
            "Ignorovat"
        },
        {
            "chloe_cena",
            "Chloe: Pockat, tohle je ta cena?",
            KEY_T,
            10.0f,
            "Ty: Muzeme to zkontrolovat.",
            "Chloe: Super. Fakt super.",
            2,
            -4,
            MOOD_ANY,
            "Rict, ze nevis",
            "Zkontrolovat cenu",
            "Rict pravidla",
            "Pokracovat"
        }
    };

    customer->arrivalLines = {
        "Chloe: Cau.",
        "Chloe: Yo.",
        "Chloe: At to mame za sebou.",
        "Chloe: Sup."
    };

    customer->exitHappyLines = {
        "Chloe: Cool, diky.",
        "Chloe: Nice.",
        "Chloe: Tak zatim."
    };

    customer->exitNeutralLines = {
        "Chloe: Cau.",
        "Chloe: Okej.",
        "Chloe: Jasne."
    };

    customer->exitAnnoyedLines = {
        "Chloe: Meh.",
        "Chloe: Mohlo to byt horsi.",
        "Chloe: Whatever."
    };

    customer->exitAngryLines = {
        "Chloe: Tohle bylo hrozny.",
        "Chloe: Nope.",
        "Chloe: Mizim."
    };

    return customer;
}

static std::shared_ptr<Customer> CreateBigKrtkusCustomer()
{
    auto customer = std::make_shared<Customer>(
        "big_krtkus",
        "Big Krtkus",
        GetRandomValue(20, 99),
        false,
        true,
        ARCH_REGULAR,
        45,
        GetCustomerTexture("big_krtkus", ARCH_REGULAR, true)
    );

    customer->favoriteItemIds = {
        "rum", "pivo", "chipsy"
    };

    customer->qteSet = {
        {
            "krtkus_snack",
            "Big Krtkus: VELKY KRTKUS CHCE VELKY SNACK.",
            KEY_T,
            10.0f,
            "Ty: Rozumim, tady mate.",
            "Big Krtkus: KRTKUS SMUTNY.",
            2,
            -5,
            MOOD_ANY,
            "Uklidnit ho",
            "Nabidnout snack",
            "Rict pravidla",
            "Zrychlit"
        },
        {
            "krtkus_pomalu",
            "Big Krtkus: PROC TAK POMALU.",
            KEY_U,
            10.0f,
            "Ty: Hned to bude.",
            "Big Krtkus: NEPRIJATELNE.",
            2,
            -6,
            MOOD_ANY,
            "Mile odpovedet",
            "Nabidnout tasku",
            "Ignorovat",
            "Zrychlit"
        }
    };

    customer->arrivalLines = {
        "Big Krtkus: ZDRAVIM.",
        "Big Krtkus: CAS NA SNACK.",
        "Big Krtkus: KRTKUS PRISEL.",
        "Big Krtkus: VELKY DEN."
    };

    customer->exitHappyLines = {
        "Big Krtkus: DOBRE.",
        "Big Krtkus: KRTKUS SPOKOJEN.",
        "Big Krtkus: NICE."
    };

    customer->exitNeutralLines = {
        "Big Krtkus: NASHLE.",
        "Big Krtkus: HMM.",
        "Big Krtkus: OK."
    };

    customer->exitAnnoyedLines = {
        "Big Krtkus: MOC DLOUHO.",
        "Big Krtkus: KRTKUS CEKAL.",
        "Big Krtkus: HMPH."
    };

    customer->exitAngryLines = {
        "Big Krtkus: KRTKUS NASTVANY.",
        "Big Krtkus: UZ NIKDY.",
        "Big Krtkus: SPATNY OBCHOD."
    };

    return customer;
}

static std::shared_ptr<Customer> CreateHonzaTunaCustomer()
{
    auto customer = std::make_shared<Customer>(
        "honza_tuna",
        "Honza Tuna",
        GetRandomValue(45, 60),
        true,
        true,
        ARCH_REGULAR,
        32,
        GetCustomerTexture("honza_tuna", ARCH_REGULAR, true)
    );

    customer->favoriteItemIds = {
        "voda", "chleba", "mleko"
    };

    customer->qteSet = {
        {
            "tuna_kontrola_ceny",
            "Honza Tuna: Muzete mi vysvetlit, proc tahle cena nesedi?",
            KEY_T,
            10.0f,
            "Ty: Hned to zkontroluju v systemu.",
            "Honza Tuna: Tak tohle bude reportaz.",
            3,
            -6,
            MOOD_ANY,
            "Omluvit se",
            "Zkontrolovat cenu",
            "Rict, ze ceny neurcujes",
            "Rychle to namarkovat"
        },
        {
            "tuna_ucet",
            "Honza Tuna: A uctenka bude kompletni?",
            KEY_R,
            10.0f,
            "Ty: Ano, vsechno tam bude.",
            "Honza Tuna: To si jeste overime.",
            2,
            -4,
            MOOD_ANY,
            "Ujistit ho",
            "Nabidnout tasku",
            "Rict pravidla",
            "Mlcky pokracovat"
        }
    };

    customer->arrivalLines = {
        "Honza Tuna: Dobry den, dnes si to trochu proverime.",
        "Honza Tuna: Zajimaji me ceny a uctenka.",
        "Honza Tuna: Snad je vsechno v poradku.",
        "Honza Tuna: Jdeme na kontrolni nakup."
    };

    customer->exitHappyLines = {
        "Honza Tuna: Dneska bez problemu.",
        "Honza Tuna: Tohle vypada v poradku.",
        "Honza Tuna: Dekuji, profesionalni prace."
    };

    customer->exitNeutralLines = {
        "Honza Tuna: Dobre, uvidime.",
        "Honza Tuna: Beru na vedomi.",
        "Honza Tuna: Nashledanou."
    };

    customer->exitAnnoyedLines = {
        "Honza Tuna: Tady by to chtelo zlepseni.",
        "Honza Tuna: Tohle si jeste poznamename.",
        "Honza Tuna: Ne uplne idealni."
    };

    customer->exitAngryLines = {
        "Honza Tuna: Tohle uz je na reportaz.",
        "Honza Tuna: Tohle zakazniky opravdu nepotěsi.",
        "Honza Tuna: Dneska jste me nepresvedcili."
    };

    return customer;
}


static std::shared_ptr<Customer> CreateHonzaSpacekCustomer()
{
    auto customer = std::make_shared<Customer>(
        "honza_spacek",
        "Honza Spacek",
        GetRandomValue(25, 40),
        true,
        true,
        ARCH_REGULAR,
        35,
        GetCustomerTexture("honza_spacek", ARCH_REGULAR, true)
    );

    customer->favoriteItemIds = {
        "kava", "rohlik", "jogurt"
    };

    customer->qteSet = {
        {
            "spacek_prednaska",
            "Honza Spacek: Tohle je krasny priklad spatne optimalizovane fronty.",
            KEY_R,
            10.0f,
            "Ty: Chapu, pokusim se to zrychlit.",
            "Honza Spacek: Vidite, tady nastava problem.",
            2,
            -5,
            MOOD_ANY,
            "Prikynout",
            "Nabidnout tasku",
            "Rict pravidla",
            "Ignorovat prednasku"
        },
        {
            "spacek_algoritmus",
            "Honza Spacek: Kdybychom to popsali algoritmem...",
            KEY_U,
            10.0f,
            "Ty: Algoritmus zrychlen.",
            "Honza Spacek: To neni uplne korektni reseni.",
            2,
            -4,
            MOOD_ANY,
            "Zeptat se na slozitost",
            "Nabidnout Clubcard",
            "Rict pravidla",
            "Zrychlit markovani"
        }
    };

    customer->arrivalLines = {
        "Honza Spacek: Dobry den, analyzujme tento nakup.",
        "Honza Spacek: Fronta je zajimavy system.",
        "Honza Spacek: Dneska to bude male cviceni z efektivity.",
        "Honza Spacek: Zdravim, zacneme."
    };

    customer->exitHappyLines = {
        "Honza Spacek: Efektivni. Pekne.",
        "Honza Spacek: Tohle melo dobrou casovou slozitost.",
        "Honza Spacek: Spokojenost."
    };

    customer->exitNeutralLines = {
        "Honza Spacek: Dobre.",
        "Honza Spacek: Prijatelne.",
        "Honza Spacek: Nashledanou."
    };

    customer->exitAnnoyedLines = {
        "Honza Spacek: Tady by sla optimalizace.",
        "Honza Spacek: Zajimave, ale pomale.",
        "Honza Spacek: To nebylo uplne elegantni."
    };

    customer->exitAngryLines = {
        "Honza Spacek: Tohle je bug v procesu.",
        "Honza Spacek: Neefektivni reseni.",
        "Honza Spacek: System selhal."
    };

    return customer;
}

static std::shared_ptr<Customer> CreateSugarDennyCustomer()
{
    auto customer = std::make_shared<Customer>(
        "sugar_denny",
        "Sugar Denny",
        GetRandomValue(20, 30),
        false,
        true,
        ARCH_REGULAR,
        24,
        GetCustomerTexture("sugar_denny", ARCH_REGULAR, true)
    );

    customer->favoriteItemIds = {
        "energy_drink", "chipsy", "kava"
    };

    customer->qteSet = {
        {
            "sugar_drama",
            "Sugar Denny: Babe, proc je ten pas tak pomalej?",
            KEY_R,
            10.0f,
            "Ty: Sorry, hned to bude.",
            "Sugar Denny: Tohle je fakt drama.",
            2,
            -5,
            MOOD_ANY,
            "Omluvit se",
            "Nabidnout tasku",
            "Rict pravidla",
            "Ignorovat vibe"
        },
        {
            "sugar_clubcard",
            "Sugar Denny: Mam Clubcard? Ja uz ani nevim.",
            KEY_T,
            10.0f,
            "Ty: Muzeme to zkusit zkontrolovat.",
            "Sugar Denny: Tak to je chaos.",
            2,
            -4,
            MOOD_ANY,
            "Rict, ze taky nevis",
            "Zkontrolovat Clubcard",
            "Rict pravidla",
            "Rychle pokracovat"
        }
    };

    customer->arrivalLines = {
        "Sugar Denny: Cauky.",
        "Sugar Denny: Dneska mam fakt den.",
        "Sugar Denny: To bude rychly, ze jo?",
        "Sugar Denny: Potrebuju jen snack a klid."
    };

    customer->exitHappyLines = {
        "Sugar Denny: Slay, diky.",
        "Sugar Denny: To bylo cute.",
        "Sugar Denny: Okej, beru."
    };

    customer->exitNeutralLines = {
        "Sugar Denny: Tak jo.",
        "Sugar Denny: Bye.",
        "Sugar Denny: Okej."
    };

    customer->exitAnnoyedLines = {
        "Sugar Denny: Meh.",
        "Sugar Denny: Trochu chaos.",
        "Sugar Denny: Tohle nebyl vibe."
    };

    customer->exitAngryLines = {
        "Sugar Denny: Tohle byl anti-vibe.",
        "Sugar Denny: Ja nemam slov.",
        "Sugar Denny: Drama zdarma."
    };

    return customer;
}

static std::shared_ptr<Customer> CreateAlastorCustomer()
{
    auto customer = std::make_shared<Customer>(
        "alastor",
        "Alastor",
        GetRandomValue(30, 99),
        false,
        true,
        ARCH_REGULAR,
        42,
        GetCustomerTexture("alastor", ARCH_REGULAR, true)
    );

    customer->favoriteItemIds = {
        "kava", "rum", "chipsy"
    };

    customer->qteSet = {
        {
            "alastor_radio",
            "Alastor: Dobry vecer, vitejte v mem malem vysilani!",
            KEY_R,
            10.0f,
            "Ty: Dobry vecer, nakup bude hned.",
            "Alastor: Ach, tak malo showmanshipu.",
            3,
            -5,
            MOOD_ANY,
            "Hrat s nim divadlo",
            "Nabidnout tasku",
            "Rict pravidla",
            "Mlcky markovat"
        },
        {
            "alastor_usmev",
            "Alastor: Usmev, prosim. Bez nej je obsluha nudna.",
            KEY_R,
            10.0f,
            "Ty: Samozrejme. Usmev zdarma.",
            "Alastor: To zni zoufale.",
            2,
            -6,
            MOOD_ANY,
            "Usmat se",
            "Nabidnout Clubcard",
            "Rict pravidla",
            "Zrychlit"
        }
    };

    customer->arrivalLines = {
        "Alastor: Dobry den, jak rozkosne misto.",
        "Alastor: Ach, samoobsluha duse.",
        "Alastor: Dnes nakupuji s usmevem.",
        "Alastor: Tak co nam dnesni kasa zahraje?"
    };

    customer->exitHappyLines = {
        "Alastor: Vyborne, to melo styl.",
        "Alastor: Krasne predstaveni.",
        "Alastor: Budu vas doporucovat v eteru."
    };

    customer->exitNeutralLines = {
        "Alastor: Uchazejici.",
        "Alastor: Dekuji za obsluhu.",
        "Alastor: Pokracujme nekdy priste."
    };

    customer->exitAnnoyedLines = {
        "Alastor: Trochu bez energie.",
        "Alastor: Tohle chtelo lepsi tempo.",
        "Alastor: Publikum by se nudilo."
    };

    customer->exitAngryLines = {
        "Alastor: Tak tohle vysilani konci.",
        "Alastor: Velmi spatna epizoda.",
        "Alastor: Usmev zustal, trpelivost ne."
    };

    return customer;
}
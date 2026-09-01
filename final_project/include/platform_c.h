#ifndef ADAS_PLATFORM_C_H
#define ADAS_PLATFORM_C_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { ECU_STARTUP, ECU_RUN, ECU_POST_RUN, ECU_SHUTDOWN } EcuMode;
typedef enum { DEM_PASSED, DEM_FAILED } DemStatus;
typedef enum { EVENT_SENSOR_TIMEOUT, EVENT_BAD_COUNTER, EVENT_QUEUE_OVERFLOW, EVENT_COUNT } DemEvent;

typedef struct {
    uint32_t timestamp_ms;
    uint16_t distance_cm;
    int16_t relative_speed_cms;
    uint8_t counter;
    uint8_t sensor_id;
} SensorPdu;

typedef void (*SensorRxCallback)(void *context, const SensorPdu *pdu);

void Platform_Init(void);
void Platform_SetMode(EcuMode mode);
EcuMode Platform_GetMode(void);
void Platform_RegisterSensorCallback(SensorRxCallback callback, void *context);
bool Platform_InjectRxPdu(const uint8_t *payload, size_t length);
void Platform_ComSendAdasStatus(uint8_t risk, bool valid, uint16_t distance_cm);
void Platform_DemReport(DemEvent event, DemStatus status);
uint32_t Platform_DemFailureCount(DemEvent event);
void Platform_NvMWriteRisk(uint8_t risk);
bool Platform_NvMMainFunction(void);
uint8_t Platform_NvMReadRisk(void);

#ifdef __cplusplus
}
#endif
#endif

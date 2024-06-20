#ifndef MESH_CONFIG_H
#define MESH_CONFIG_H

#include <stdint.h>

#if (defined(PHY_AT86RF212B) || defined(PHY_AT86RF212))
#define APP_CHANNEL    0x01
#define APP_BAND       0x00
#define APP_MODULATION 0x24
#endif

#define SYS_SECURITY_MODE 0

#define NWK_BUFFERS_AMOUNT                 10
#define NWK_DUPLICATE_REJECTION_TABLE_SIZE 50
#define NWK_DUPLICATE_REJECTION_TTL        2000 /* ms */
#define NWK_ROUTE_TABLE_SIZE               100
#define NWK_ROUTE_DEFAULT_SCORE            3
#define NWK_ACK_WAIT_TIME                  1000 /* ms */
#define NWK_GROUPS_AMOUNT                  3
#define NWK_ROUTE_DISCOVERY_TABLE_SIZE     5
#define NWK_ROUTE_DISCOVERY_TIMEOUT        1000 /* ms */
#define APP_RX_BUF_SIZE                    20
#define NWK_ENABLE_ROUTING
#define NWK_ENABLE_SECURITY
#define NWK_ENABLE_ROUTE_DISCOVERY

#endif

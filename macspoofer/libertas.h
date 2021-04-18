#ifndef __LIBERTAS_H__
#define __LIBERTAS_H__

/* Command codes */
/* Todo: add the rest of commands */
#define LIBERTAS_CMD_GET_HW_SPEC			0x0003
#define LIBERTAS_CMD_EEPROM_UPDATE			0x0004
#define LIBERTAS_CMD_802_11_RESET			0x0005
#define LIBERTAS_CMD_802_11_SCAN			0x0006
#define LIBERTAS_CMD_MAC_MULTICAST_ADR		0x0010
#define LIBERTAS_CMD_802_11_AUTHENTICATE	0x0011
#define LIBERTAS_CMD_802_11_ASSOCIATE		0x0012
#define LIBERTAS_CMD_802_11_SNMP_MIB		0x0016
#define LIBERTAS_CMD_802_11_RADIO_CONTROL	0x001C
#define LIBERTAS_CMD_802_11_RSSI			0x001F
#define LIBERTAS_CMD_802_11_DEAUTHENTICATE	0x0024
#define LIBERTAS_CMD_802_11_DISASSOCIATE	0x0026
#define LIBERTAS_CMD_MAC_CONTROL			0x0028
#define LIBERTAS_CMD_802_11_ADHOC_START		0x002B
#define LIBERTAS_CMD_802_11_ADHOC_JOIN		0x002C

/* FwCap flags */
#define LIBERTAS_FWCAP_WPA				(1 << 0)
#define LIBERTAS_FWCAP_PS				(1 << 1)
// (1 << 2) reserved
/* Note: flags from here are not supported by magpie/voyager firmwares */
#define LIBERTAS_FWCAP_NOT_EEPROM_EXIT	(1 << 3)
#define LIBERTAS_FWCAP_TX_ANTENNA_CAP	((1 << 4) | (1 << 5))
#define LIBERTAS_FWCAP_RX_ANTENNA_CAP	((1 << 6) | (1 << 7))
#define LIBERTAS_FWCAP_802_11_B			(1 << 8)
#define LIBERTAS_FWCAP_802_11_G			(1 << 9)
#define LIBERTAS_FWCAP_802_11_A			(1 << 10)

/* Generic actions */
#define 	LIBERTAS_ACTION_GET	0
#define 	LIBERTAS_ACTION_SET	1

/* Actions for LIBERTAS_CMD_MAC_CONTROL */
#define LIBERTAS_ACTION_RX_ON				(1 << 0)
#define LIBERTAS_ACTION_TX_ON				(1 << 1)
#define LIBERTAS_ACTION_WEP_ON				(1 << 3)
#define LIBERTAS_ACTION_PROMISCUOS_ON		(1 << 7)
#define LIBERTAS_ACTION_MULTICAST_ON		(1 << 8)
#define LIBERTAS_ACTION_ENFORCE_PROTECTION	(1 << 10)
#define LIBERTAS_ACTION_ENABLE_WMM			(1 << 11)
#define LIBERTAS_ACTION_WEP_TYPE_104		(1 << 12)

/* Commands structs */
/* Todo: add the rest of structures */
typedef struct LIBERTAS_GET_HW_SPEC_COMMAND
{
	u16 HwIfVersion;
	u16 HwVersion;
	u16 NumOfWCB;
	u16 NumOfMCastAddr;
	u8  MacAddr[6];
	u16 RegionCode;
	u16 NumberOfAntenna;
	u32 FWReleaseNumber;
	u32 WcbBase;
	u32 RxPdRdPtr;
	u32 RxPdWrPtr;
	u32 FwCapInfo;
} __attribute__((packed)) LIBERTAS_GET_HW_SPEC_COMMAND;

typedef struct LIBERTAS_MAC_CONTROL_COMMAND
{
	u16 Action; 
	u16 Reserved;
} __attribute__((packed)) LIBERTAS_MAC_CONTROL_COMMAND;

typedef struct LIBERTAS_COMMAND
{
	u16 CmdCode;
	u16 Size;
	u16 SeqNum;
	u16 Result;

	union
	{
		LIBERTAS_GET_HW_SPEC_COMMAND	hwspec;
		LIBERTAS_MAC_CONTROL_COMMAND	maccontrol;
	};
} __attribute__((packed)) LIBERTAS_COMMAND;

#endif


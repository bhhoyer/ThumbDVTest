#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "ftd2xx.h"

static const char reset_packet[] = { 0x61, 0x00, 0x01, 0x00, 0x33 };
static const char reset_response[] = { 0x61, 0x00, 0x01, 0x00, 0x39 };
static const char product_id_packet[] = { 0x61, 0x00, 0x01, 0x00, 0x30 };
static const char version_packet[] = { 0x61, 0x00, 0x01, 0x00, 0x31 };
static const char set_ratep_packet[] = { 0x61, 0x00, 0x0d, 0x00, 0x0a, 0x01, 0x30, 0x07, 0x63, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48 };
static const char set_ratep_response[] = { 0x00, 0x0a, 0x00 };
static const char set_channel_format_packet[] = { 0x61, 0x00, 0x03, 0x00, 0x15, 0x00, 0x01 };
static const char set_channel_format_response[] = { 0x00, 0x15, 0x00 };
static const char silence_packet[] = { 0x61, 0x00, 0x0B, 0x01, 0x01, 0x48, 0xAC, 0xAA, 0x40, 0x20, 0x00, 0x44, 0x40, 0x80, 0x80 };
static const char silence_response[] = { 0x02, 0x00, 0xa0 };

int read_packet(FT_HANDLE handle, char *response)
{
	FT_STATUS status;
	char start_byte;
	uint16_t packet_length;
	DWORD bytes_read;

	status = FT_Read(handle, &start_byte, 1, &bytes_read);
	if (status != FT_OK) {
		fprintf(stderr, "cannot read start byte\n");
		return -1;
	}

	if (start_byte != 0x61) {
		fprintf(stderr, "Failed to get start byte\n");
		return -1;
	}

	status = FT_Read(handle, &packet_length, 2, &bytes_read);
	if (status != FT_OK) {
		fprintf(stderr, "cannot read packet length\n");
		return -1;
	}
	packet_length = ntohs(packet_length);

//	printf("Getting packet with %d size\n", packet_length);
	status = FT_Read(handle, response, packet_length + 1, &bytes_read);
	if (status != FT_OK) {
		fprintf(stderr, "cannot read rest of packet\n");
		return -1;
	}


	return bytes_read;
}

int main(int argc, char **argv)
{

	FT_STATUS status;
	FT_HANDLE thumbdv_handle;
	DWORD bytes_written, bytes_read;
	char read_buffer[256];
	char product_id[256];
	char version[256];

	status = FT_OpenEx("DA01PT7S", FT_OPEN_BY_SERIAL_NUMBER, &thumbdv_handle);
	if (status != FT_OK) {
		fprintf(stderr, "Couldn't open device\n");
		exit(1);
	}

	status = FT_SetBaudRate(thumbdv_handle, 460800);
	if (status != FT_OK) {
		fprintf(stderr, "Couldn't set baud rate\n");
		exit(1);
	}

	status = FT_SetDataCharacteristics(thumbdv_handle, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE);
	if (status != FT_OK) {
		fprintf(stderr, "Couldn't set data characteristics\n");
		exit(1);
	}

	//  Maybe FT_SetTimeouts here
	FT_SetTimeouts(thumbdv_handle, 1000, 0);
	if (status != FT_OK) {
		fprintf(stderr, "Couldn't set timeout\n");
		exit(1);
	}

	status = FT_SetFlowControl(thumbdv_handle, FT_FLOW_RTS_CTS, 0, 0);
	if (status != FT_OK) {
		fprintf(stderr, "Couldn't set flow control\n");
		exit(1);
	}

	//  Send the reset packet and make sure we get a response
	status = FT_Write(thumbdv_handle, (LPVOID) reset_packet, sizeof(reset_packet), &bytes_written);
	if (status != FT_OK) {
		fprintf(stderr, "Couldn't write reset packet\n");
		exit(1);
	}
	status = FT_Read(thumbdv_handle, read_buffer, sizeof(reset_response), &bytes_read);
	if (status != FT_OK) {
		fprintf(stderr, "Couldn't read reset packet\n");
		exit(1);
	}
	if(memcmp(read_buffer, reset_response, sizeof(reset_response)) != 0) {
		fprintf(stderr, "Reset response is invalid\n");
		exit(1);
	}

	//  Get the product ID for the chip
	status = FT_Write(thumbdv_handle, (LPVOID) product_id_packet, sizeof(product_id_packet), &bytes_written);
	if (status != FT_OK) {
		fprintf(stderr, "Couldn't write product_id packet\n");
		exit(1);
	}
	bytes_read = read_packet(thumbdv_handle, product_id);
	product_id[bytes_read] = '\0';
	printf("Product ID: %s\n", product_id + 2);

	//  Get the version of the chip
	status = FT_Write(thumbdv_handle, (LPVOID) version_packet, sizeof(version_packet), &bytes_written);
	if (status != FT_OK) {
		fprintf(stderr, "Couldn't write version packet\n");
		exit(1);
	}
	bytes_read = read_packet(thumbdv_handle, version);
	version[bytes_read] = '\0';
	printf("Version: %s\n", version + 2);

	//  Set the rate parameters
	status = FT_Write(thumbdv_handle, (LPVOID) set_ratep_packet, sizeof(set_ratep_packet), &bytes_written);
	if (status != FT_OK) {
		fprintf(stderr, "Couldn't write ratep packet\n");
		exit(1);
	}
	bytes_read = read_packet(thumbdv_handle, read_buffer);
//	for (int i = 0; i < bytes_read; ++i) {
//		printf("%x ", read_buffer[i]);
//	}
	if(memcmp(read_buffer, set_ratep_response, sizeof(set_ratep_response)) != 0) {
		fprintf(stderr, "Couldn't set retep word\n");
		exit(1);
	}

	//  Set the channel format
	status = FT_Write(thumbdv_handle, (LPVOID) set_channel_format_packet, sizeof(set_channel_format_packet), &bytes_written);
	if (status != FT_OK) {
		fprintf(stderr, "Couldn't write channel format packet\n");
		exit(1);
	}
	bytes_read = read_packet(thumbdv_handle, read_buffer);
//	for (int i = 0; i < bytes_read; ++i) {
//		printf("%x ", read_buffer[i]);
//	}
	if(memcmp(read_buffer, set_channel_format_response, sizeof(set_channel_format_response)) != 0) {
		fprintf(stderr, "Couldn't set channel_format\n");
		exit(1);
	}

	printf("Firing decode packets at chip...\n");
	//  Start firing decode packets at the chip
	status = FT_Write(thumbdv_handle, (LPVOID) silence_packet, sizeof(silence_packet), &bytes_written);
	if (status != FT_OK) {
		fprintf(stderr, "Couldn't write silence packet\n");
		exit(1);
	}
	while(1) {
		bytes_read = read_packet(thumbdv_handle, read_buffer);
		if(bytes_read != 323) {
			fprintf(stderr, "Received invalid packet!\n");
			exit(1);
		}
		if(memcmp(read_buffer, silence_response, sizeof(silence_response)) != 0) {
			fprintf(stderr, "Received invalid packet!\n");
			exit(1);
		}
		putchar('.');
		fflush(stdout);

//		for (int i = 0; i < bytes_read; ++i) {
//			printf("0x%02x ", read_buffer[i] & 0xff);
//		}
		status = FT_Write(thumbdv_handle, (LPVOID) silence_packet, sizeof(silence_packet), &bytes_written);
		if (status != FT_OK) {
			fprintf(stderr, "Couldn't write silence packet\n");
			exit(1);
		}
	}

	//  FT_GetQueueStatus will tell you whether there's stuff waiting.
	//  FT_SetEventNotification will set events to block on.
}

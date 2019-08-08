/* 
 * Copyright (C) 2012-2014 Chris McClelland
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <makestuff.h>
#include <libfpgalink.h>
#include <libbuffer.h>
#include <liberror.h>
#include <libdump.h>
#include <argtable2.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif
char ACK1[32] = "10011011011001010101110110001101";
char ACK2[32] = "01001101101000101011010100001101";
int CHANNEL,X,Y;
int T1 = 16;


struct coordata{
	int data[8][3];
	
};


bool sigIsRaised(void);
void sigRegisterHandler(void);
const uint8 *recvData;
char key[] = "11001100110011001100110011000001";

int* add(int arr[],int n){
	int ans=0;
	static int result[4];
	ans=arr[0]+arr[1]*2+arr[2]*4+arr[3]*8+n;
	for (int i = 0; i < 4; ++i)
	{
		result[i]=ans%2;
		ans=ans/2;
	}
	
	return result;
}
void encry(int P[],int B[32]){
	int N=0;
	int t[4];
	int K[32];
	int A[32];
	int *temp;
	for (int i = 0; i < 32; ++i)
	{
		K[i]=key[31-i];
		A[i]=P[31-i];
		N=N+key[31-i];
	}
	t[3]=K[31]^K[27]^K[23]^K[19]^K[15]^K[11]^K[7]^K[3];
	t[2]=K[30]^K[26]^K[22]^K[18]^K[14]^K[10]^K[6]^K[2];
	t[1]=K[29]^K[25]^K[21]^K[17]^K[13]^K[9]^K[5]^K[1];
	t[0]=K[28]^K[24]^K[20]^K[16]^K[12]^K[8]^K[4]^K[0];
	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			A[j]=A[j]^t[j];
			A[4+j]=A[4+j]^t[j];
			A[8+j]=A[8+j]^t[j];
			A[12+j]=A[12+j]^t[j];
			A[16+j]=A[16+j]^t[j];
			A[20+j]=A[20+j]^t[j];
			A[24+j]=A[24+j]^t[j];
			A[28+j]=A[28+j]^t[j];
		}
		temp=add(t,1);
		t[0]=temp[0];
		t[1]=temp[1];
		t[2]=temp[2];
		t[3]=temp[3];
	}
	for(int i=0;i<32;i++){
		B[i]=A[31-i];
	}

	return;
}
void decry(int c[],int B[32]){
	int N=0;
	//int* A = malloc(sizeof(int)*40);
	int t[4];
	int K[32];
	int A[32];
	int* temp;
	for (int i = 0; i < 32; ++i)
	{
		K[i]=key[31-i] - '0';
		A[i]=c[31-i];
		N=N+key[31-i] -'0';

	}
	t[3]=K[31]^K[27]^K[23]^K[19]^K[15]^K[11]^K[7]^K[3];
	t[2]=K[30]^K[26]^K[22]^K[18]^K[14]^K[10]^K[6]^K[2];
	t[1]=K[29]^K[25]^K[21]^K[17]^K[13]^K[9]^K[5]^K[1];
	t[0]=K[28]^K[24]^K[20]^K[16]^K[12]^K[8]^K[4]^K[0];
	
		temp=add(t,15);
		t[0]=temp[0];
		t[1]=temp[1];
		t[2]=temp[2];
		t[3]=temp[3];
	//	printf("%i\n",N);
	for (int i = 0; i < 32-N; ++i){
		for (int j = 0; j < 4; ++j){
			A[j]=A[j]^t[j];
			A[4+j]=A[4+j]^t[j];
			A[8+j]=A[8+j]^t[j];
			A[12+j]=A[12+j]^t[j];
			A[16+j]=A[16+j]^t[j];
			A[20+j]=A[20+j]^t[j];
			A[24+j]=A[24+j]^t[j];
			A[28+j]=A[28+j]^t[j];
		}
		temp=add(t,15);
		t[0]=temp[0];
		t[1]=temp[1];
		t[2]=temp[2];
		t[3]=temp[3];
	}
	for(int i=0;i<32;i++){
		B[i]=A[31-i];
	}
	return;
}


static const char *ptr;
static bool enableBenchmarking = false;

static bool isHexDigit(char ch) {
	return
		(ch >= '0' && ch <= '9') ||
		(ch >= 'a' && ch <= 'f') ||
		(ch >= 'A' && ch <= 'F');
}
void binary(int a,int answer[]){
	int ans=a;
	for (int i = 0; i < 8; ++i)
	{
		answer[7-i]=ans%2;
		ans=ans/2;
	}
	return;

}
char hexchange(int n){
	char ans;
	if (n==0) ans='0';
	else if(n==1)ans='1';
	else if(n==2)ans='2';
	else if(n==3)ans='3';
	else if(n==4)ans='4';
	else if(n==5)ans='5';
	else if(n==6)ans='6';
	else if(n==7)ans='7';
	else if(n==8)ans='8';
	else if(n==9)ans='9';
	else if(n==10)ans='a';
	else if(n==11)ans='b';
	else if(n==12)ans='c';
	else if(n==13)ans='d';
	else if(n==14)ans='e';
	else {ans='f';}


return ans;
}
int hextoint(char hexdigit){
	if(hexdigit=='A' || hexdigit=='a')return 10;
	else if(hexdigit=='B' || hexdigit=='b')return 11;
	else if(hexdigit=='C' || hexdigit=='c')return 12;
	else if(hexdigit=='D' || hexdigit=='d')return 13;
	else if(hexdigit=='E' || hexdigit=='e')return 14;
	else if(hexdigit=='F' || hexdigit=='f')return 15;
	else return hexdigit - '0';
}
char bintohex(int arr[4]){
	int ans = arr[0]*8+arr[1]*4+arr[2]*2+arr[3];
	return hexchange(ans);
}

int binarytoint(int data[8]){
	int ans = data[0]*128+data[1]*64+data[2]*32+data[3]*16+data[4]*8+data[5]*4+data[6]*2+data[7]; 
	return ans;
}
void hexadecimal(int arr[],char ans[],int length){
	//length must be a multiple of 4
	if(length % 4==0){
		int times = length/4;
		for(int i=0;i<times;i++){
			int temp[4];
			temp[0]=arr[4*i];
			temp[1]=arr[4*i+1];
			temp[2]=arr[4*i+2];
			temp[3]=arr[4*i+3];
			ans[i]=bintohex(temp);
		}
	}
	else{
		printf("Incorrect arguements. Length must be a multiple of 4\n");
	}

}



static uint16 calcChecksum(const uint8 *data, size_t length) {
	uint16 cksum = 0x0000;
	while ( length-- ) {
		cksum = (uint16)(cksum + *data++);
	}
	return cksum;
}

static bool getHexNibble(char hexDigit, uint8 *nibble) {
	if ( hexDigit >= '0' && hexDigit <= '9' ) {
		*nibble = (uint8)(hexDigit - '0');
		return false;
	} else if ( hexDigit >= 'a' && hexDigit <= 'f' ) {
		*nibble = (uint8)(hexDigit - 'a' + 10);
		return false;
	} else if ( hexDigit >= 'A' && hexDigit <= 'F' ) {
		*nibble = (uint8)(hexDigit - 'A' + 10);
		return false;
	} else {
		return true;
	}
}

static int getHexByte(uint8 *byte) {
	uint8 upperNibble;
	uint8 lowerNibble;
	if ( !getHexNibble(ptr[0], &upperNibble) && !getHexNibble(ptr[1], &lowerNibble) ) {
		*byte = (uint8)((upperNibble << 4) | lowerNibble);
		byte += 2;
		return 0;
	} else {
		return 1;
	}
}

static const char *const errMessages[] = {
	NULL,
	NULL,
	"Unparseable hex number",
	"Channel out of range",
	"Conduit out of range",
	"Illegal character",
	"Unterminated string",
	"No memory",
	"Empty string",
	"Odd number of digits",
	"Cannot load file",
	"Cannot save file",
	"Bad arguments"
};

typedef enum {
	FLP_SUCCESS,
	FLP_LIBERR,
	FLP_BAD_HEX,
	FLP_CHAN_RANGE,
	FLP_CONDUIT_RANGE,
	FLP_ILL_CHAR,
	FLP_UNTERM_STRING,
	FLP_NO_MEMORY,
	FLP_EMPTY_STRING,
	FLP_ODD_DIGITS,
	FLP_CANNOT_LOAD,
	FLP_CANNOT_SAVE,
	FLP_ARGS
} ReturnCode;

static ReturnCode doRead(
	struct FLContext *handle, uint8 chan, uint32 length, FILE *destFile, uint16 *checksum,
	const char **error)
{
	ReturnCode retVal = FLP_SUCCESS;
	uint32 bytesWritten;
	FLStatus fStatus;
	uint32 chunkSize;
	uint32 actualLength;
	const uint8 *ptr;
	// const uint8 hex;
	uint16 csVal = 0x0000;
	#define READ_MAX 65536
	// Read first chunk
	chunkSize = length >= READ_MAX ? READ_MAX : length;
	fStatus = flReadChannelAsyncSubmit(handle, chan, chunkSize, NULL, error);
	CHECK_STATUS(fStatus, FLP_LIBERR, cleanup, "doRead()");
	length = length - chunkSize;
	while ( length ) {
		// Read chunk N
		chunkSize = length >= READ_MAX ? READ_MAX : length;
		fStatus = flReadChannelAsyncSubmit(handle, chan, chunkSize, NULL, error);
		CHECK_STATUS(fStatus, FLP_LIBERR, cleanup, "doRead()");
		length = length - chunkSize;
		
		// Await chunk N-1
		fStatus = flReadChannelAsyncAwait(handle, &recvData, &actualLength, &actualLength, error);
		CHECK_STATUS(fStatus, FLP_LIBERR, cleanup, "doRead()");

		// Write chunk N-1 to file
		bytesWritten = (uint32)fwrite(recvData, 1, actualLength, destFile);
		CHECK_STATUS(bytesWritten != actualLength, FLP_CANNOT_SAVE, cleanup, "doRead()");

		// Checksum chunk N-1
		chunkSize = actualLength;
		ptr = recvData;
		while ( chunkSize-- ) {
			csVal = (uint16)(csVal + *ptr++);
		}

	}

	// Await last chunk
	fStatus = flReadChannelAsyncAwait(handle, &recvData, &actualLength, &actualLength, error);
	CHECK_STATUS(fStatus, FLP_LIBERR, cleanup, "doRead()");
	
	// Write last chunk to file
	bytesWritten = (uint32)fwrite(recvData, 1, actualLength, destFile);
	CHECK_STATUS(bytesWritten != actualLength, FLP_CANNOT_SAVE, cleanup, "doRead()");

	// Checksum last chunk
	chunkSize = actualLength;
	ptr = recvData;
	//printf("rece %d\n", recvData[0]);
	//hex = getHexByte(&recvData[0]);
	//printf("Hex data %d\n", hex);
	//printf("rece %d\n", recvData[1]);


	while ( chunkSize-- ) {
		csVal = (uint16)(csVal + *ptr++);
	}
	
	// Return checksum to caller
	*checksum = csVal;
cleanup:
	return retVal;
}

static ReturnCode doWrite(
	struct FLContext *handle, uint8 chan, FILE *srcFile, size_t *length, uint16 *checksum,
	const char **error)
{
	ReturnCode retVal = FLP_SUCCESS;
	size_t bytesRead, i;
	FLStatus fStatus;
	const uint8 *ptr;
	uint16 csVal = 0x0000;
	size_t lenVal = 0;
	#define WRITE_MAX (65536 - 5)
	uint8 buffer[WRITE_MAX];

	do {
		// Read Nth chunk
		bytesRead = fread(buffer, 1, WRITE_MAX, srcFile);
		if ( bytesRead ) {
			// Update running total
			lenVal = lenVal + bytesRead;

			// Submit Nth chunk
			fStatus = flWriteChannelAsync(handle, chan, bytesRead, buffer, error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup, "doWrite()");

			// Checksum Nth chunk
			i = bytesRead;
			ptr = buffer;
			while ( i-- ) {
				csVal = (uint16)(csVal + *ptr++);
			}
		}
	} while ( bytesRead == WRITE_MAX );

	// Wait for writes to be received. This is optional, but it's only fair if we're benchmarking to
	// actually wait for the work to be completed.
	fStatus = flAwaitAsyncWrites(handle, error);
	CHECK_STATUS(fStatus, FLP_LIBERR, cleanup, "doWrite()");

	// Return checksum & length to caller
	*checksum = csVal;
	*length = lenVal;
cleanup:
	return retVal;
}

static int parseLine(struct FLContext *handle, const char *line, const char **error) {
	ReturnCode retVal = FLP_SUCCESS, status;
	FLStatus fStatus;
	struct Buffer dataFromFPGA = {0,};
	BufferStatus bStatus;
	uint8 *data = NULL;
	char *fileName = NULL;
	FILE *file = NULL;
	double totalTime, speed;
	#ifdef WIN32
		LARGE_INTEGER tvStart, tvEnd, freq;
		DWORD_PTR mask = 1;
		SetThreadAffinityMask(GetCurrentThread(), mask);
		QueryPerformanceFrequency(&freq);
	#else
		struct timeval tvStart, tvEnd;
		long long startTime, endTime;
	#endif
	bStatus = bufInitialise(&dataFromFPGA, 1024, 0x00, error);
	CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
	ptr = line;
	do {
		while ( *ptr == ';' ) {
			ptr++;
		}
		switch ( *ptr ) {
		case 'r':{
			uint32 chan;
			uint32 length = 1;
			char *end;
			ptr++;
			
			// Get the channel to be read:
			errno = 0;
			chan = (uint32)strtoul(ptr, &end, 16);
			CHECK_STATUS(errno, FLP_BAD_HEX, cleanup);

			// Ensure that it's 0-127
			CHECK_STATUS(chan > 127, FLP_CHAN_RANGE, cleanup);
			ptr = end;

			// Only three valid chars at this point:
			CHECK_STATUS(*ptr != '\0' && *ptr != ';' && *ptr != ' ', FLP_ILL_CHAR, cleanup);

			if ( *ptr == ' ' ) {
				ptr++;

				// Get the read count:
				errno = 0;
				length = (uint32)strtoul(ptr, &end, 16);
				CHECK_STATUS(errno, FLP_BAD_HEX, cleanup);
				ptr = end;
				
				// Only three valid chars at this point:
				CHECK_STATUS(*ptr != '\0' && *ptr != ';' && *ptr != ' ', FLP_ILL_CHAR, cleanup);
				if ( *ptr == ' ' ) {
					const char *p;
					const char quoteChar = *++ptr;
					CHECK_STATUS(
						(quoteChar != '"' && quoteChar != '\''),
						FLP_ILL_CHAR, cleanup);
					
					// Get the file to write bytes to:
					ptr++;
					p = ptr;
					while ( *p != quoteChar && *p != '\0' ) {
						p++;
					}
					CHECK_STATUS(*p == '\0', FLP_UNTERM_STRING, cleanup);
					fileName = malloc((size_t)(p - ptr + 1));
					CHECK_STATUS(!fileName, FLP_NO_MEMORY, cleanup);
					CHECK_STATUS(p - ptr == 0, FLP_EMPTY_STRING, cleanup);
					strncpy(fileName, ptr, (size_t)(p - ptr));
					fileName[p - ptr] = '\0';
					ptr = p + 1;
				}
			}
			if ( fileName ) {
				uint16 checksum = 0x0000;

				// Open file for writing
				file = fopen(fileName, "wb");
				CHECK_STATUS(!file, FLP_CANNOT_SAVE, cleanup);
				free(fileName);
				fileName = NULL;

				#ifdef WIN32
					QueryPerformanceCounter(&tvStart);
					status = doRead(handle, (uint8)chan, length, file, &checksum, error);
					QueryPerformanceCounter(&tvEnd);
					totalTime = (double)(tvEnd.QuadPart - tvStart.QuadPart);
					totalTime /= freq.QuadPart;
					speed = (double)length / (1024*1024*totalTime);
				#else
					gettimeofday(&tvStart, NULL);
					status = doRead(handle, (uint8)chan, length, file, &checksum, error);
					gettimeofday(&tvEnd, NULL);
					startTime = tvStart.tv_sec;
					startTime *= 1000000;
					startTime += tvStart.tv_usec;
					endTime = tvEnd.tv_sec;
					endTime *= 1000000;
					endTime += tvEnd.tv_usec;
					totalTime = (double)(endTime - startTime);
					totalTime /= 1000000;  // convert from uS to S.
					speed = (double)length / (1024*1024*totalTime);
				#endif
				if ( enableBenchmarking ) {
					printf(
						"Read %d bytes (checksum 0x%04X) from channel %d at %f MiB/s\n",
						length, checksum, chan, speed);
				}
				CHECK_STATUS(status, status, cleanup);
				// Close the file
				fclose(file);
				file = NULL;
			} else {
				size_t oldLength = dataFromFPGA.length;
				bStatus = bufAppendConst(&dataFromFPGA, 0x00, length, error);
				CHECK_STATUS(bStatus, FLP_LIBERR, cleanup);
				#ifdef WIN32
					QueryPerformanceCounter(&tvStart);
					fStatus = flReadChannel(handle, (uint8)chan, length, dataFromFPGA.data + oldLength, error);
					QueryPerformanceCounter(&tvEnd);
					totalTime = (double)(tvEnd.QuadPart - tvStart.QuadPart);
					totalTime /= freq.QuadPart;
					speed = (double)length / (1024*1024*totalTime);
				#else
					gettimeofday(&tvStart, NULL);
					fStatus = flReadChannel(handle, (uint8)chan, length, dataFromFPGA.data + oldLength, error);
					gettimeofday(&tvEnd, NULL);
					startTime = tvStart.tv_sec;
					startTime *= 1000000;
					startTime += tvStart.tv_usec;
					endTime = tvEnd.tv_sec;
					endTime *= 1000000;
					endTime += tvEnd.tv_usec;
					totalTime = (double)(endTime - startTime);
					totalTime /= 1000000;  // convert from uS to S.
					speed = (double)length / (1024*1024*totalTime);
				#endif
				if ( enableBenchmarking ) {
					printf(
						"Read %d bytes (checksum 0x%04X) from channel %d at %f MiB/s\n",
						length, calcChecksum(dataFromFPGA.data + oldLength, length), chan, speed);
				}
				CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
				
			}
			break;
		}
		case 'w':{
			unsigned long int chan;
			size_t length = 1, i;
			char *end, ch;
			const char *p;
			ptr++;
			
			// Get the channel to be written:
			errno = 0;
			chan = strtoul(ptr, &end, 16);
			CHECK_STATUS(errno, FLP_BAD_HEX, cleanup);

			// Ensure that it's 0-127
			CHECK_STATUS(chan > 127, FLP_CHAN_RANGE, cleanup);
			ptr = end;

			// There must be a space now:
			CHECK_STATUS(*ptr != ' ', FLP_ILL_CHAR, cleanup);

			// Now either a quote or a hex digit
		   ch = *++ptr;
			if ( ch == '"' || ch == '\'' ) {
				uint16 checksum = 0x0000;

				// Get the file to read bytes from:
				ptr++;
				p = ptr;
				while ( *p != ch && *p != '\0' ) {
					p++;
				}
				CHECK_STATUS(*p == '\0', FLP_UNTERM_STRING, cleanup);
				fileName = malloc((size_t)(p - ptr + 1));
				CHECK_STATUS(!fileName, FLP_NO_MEMORY, cleanup);
				CHECK_STATUS(p - ptr == 0, FLP_EMPTY_STRING, cleanup);
				strncpy(fileName, ptr, (size_t)(p - ptr));
				fileName[p - ptr] = '\0';
				ptr = p + 1;  // skip over closing quote

				// Open file for reading
				file = fopen(fileName, "rb");
				CHECK_STATUS(!file, FLP_CANNOT_LOAD, cleanup);
				free(fileName);
				fileName = NULL;
				
				#ifdef WIN32
					QueryPerformanceCounter(&tvStart);
					status = doWrite(handle, (uint8)chan, file, &length, &checksum, error);
					QueryPerformanceCounter(&tvEnd);
					totalTime = (double)(tvEnd.QuadPart - tvStart.QuadPart);
					totalTime /= freq.QuadPart;
					speed = (double)length / (1024*1024*totalTime);
				#else
					gettimeofday(&tvStart, NULL);
					status = doWrite(handle, (uint8)chan, file, &length, &checksum, error);
					gettimeofday(&tvEnd, NULL);
					startTime = tvStart.tv_sec;
					startTime *= 1000000;
					startTime += tvStart.tv_usec;
					endTime = tvEnd.tv_sec;
					endTime *= 1000000;
					endTime += tvEnd.tv_usec;
					totalTime = (double)(endTime - startTime);
					totalTime /= 1000000;  // convert from uS to S.
					speed = (double)length / (1024*1024*totalTime);
				#endif
				if ( enableBenchmarking ) {
					printf(
						"Wrote "PFSZD" bytes (checksum 0x%04X) to channel %lu at %f MiB/s\n",
						length, checksum, chan, speed);
				}
				CHECK_STATUS(status, status, cleanup);

				// Close the file
				fclose(file);
				file = NULL;
			} else if ( isHexDigit(ch) ) {
				// Read a sequence of hex bytes to write
				uint8 *dataPtr;
				p = ptr + 1;
				while ( isHexDigit(*p) ) {
					p++;
				}
				CHECK_STATUS((p - ptr) & 1, FLP_ODD_DIGITS, cleanup);
				length = (size_t)(p - ptr) / 2;
				data = malloc(length);
				dataPtr = data;
				for ( i = 0; i < length; i++ ) {
					getHexByte(dataPtr++);
					ptr += 2;
				}
				#ifdef WIN32
					QueryPerformanceCounter(&tvStart);
					fStatus = flWriteChannel(handle, (uint8)chan, length, data, error);
					QueryPerformanceCounter(&tvEnd);
					totalTime = (double)(tvEnd.QuadPart - tvStart.QuadPart);
					totalTime /= freq.QuadPart;
					speed = (double)length / (1024*1024*totalTime);
				#else
					gettimeofday(&tvStart, NULL);

					fStatus = flWriteChannel(handle, (uint8)chan, length, data, error);
					gettimeofday(&tvEnd, NULL);
					startTime = tvStart.tv_sec;
					startTime *= 1000000;
					startTime += tvStart.tv_usec;
					endTime = tvEnd.tv_sec;
					endTime *= 1000000;
					endTime += tvEnd.tv_usec;
					totalTime = (double)(endTime - startTime);
					totalTime /= 1000000;  // convert from uS to S.
					speed = (double)length / (1024*1024*totalTime);
				#endif
				if ( enableBenchmarking ) {
					printf(
						"Wrote "PFSZD" bytes (checksum 0x%04X) to channel %lu at %f MiB/s\n",
						length, calcChecksum(data, length), chan, speed);
				}
				CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
				free(data);
				data = NULL;
			} else {
				FAIL(FLP_ILL_CHAR, cleanup);
			}
			break;
		}
		case '+':{
			uint32 conduit;
			char *end;
			ptr++;

			// Get the conduit
			errno = 0;
			conduit = (uint32)strtoul(ptr, &end, 16);
			CHECK_STATUS(errno, FLP_BAD_HEX, cleanup);

			// Ensure that it's 0-127
			CHECK_STATUS(conduit > 255, FLP_CONDUIT_RANGE, cleanup);
			ptr = end;

			// Only two valid chars at this point:
			CHECK_STATUS(*ptr != '\0' && *ptr != ';', FLP_ILL_CHAR, cleanup);

			fStatus = flSelectConduit(handle, (uint8)conduit, error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
			break;
		}
		default:
			FAIL(FLP_ILL_CHAR, cleanup);
		}
	} while ( *ptr == ';' );
	CHECK_STATUS(*ptr != '\0', FLP_ILL_CHAR, cleanup);

	dump(0x00000000, dataFromFPGA.data, dataFromFPGA.length);

cleanup:
	bufDestroy(&dataFromFPGA);
	if ( file ) {
		fclose(file);
	}
	free(fileName);
	free(data);
	if ( retVal > FLP_LIBERR ) {
		const int column = (int)(ptr - line);
		int i;
		fprintf(stderr, "%s at column %d\n  %s\n  ", errMessages[retVal], column, line);
		for ( i = 0; i < column; i++ ) {
			fprintf(stderr, " ");
		}
		fprintf(stderr, "^\n");
	}
	return retVal;
}

static const char *nibbles[] = {
	"0000",  // '0'
	"0001",  // '1'
	"0010",  // '2'
	"0011",  // '3'
	"0100",  // '4'
	"0101",  // '5'
	"0110",  // '6'
	"0111",  // '7'
	"1000",  // '8'
	"1001",  // '9'

	"XXXX",  // ':'
	"XXXX",  // ';'
	"XXXX",  // '<'
	"XXXX",  // '='
	"XXXX",  // '>'
	"XXXX",  // '?'
	"XXXX",  // '@'

	"1010",  // 'A'
	"1011",  // 'B'
	"1100",  // 'C'
	"1101",  // 'D'
	"1110",  // 'E'
	"1111"   // 'F'
};
ReturnCode readchannel(int data[32],int channel,struct FLContext *handle, const char **error){
	ReturnCode retVal = FLP_SUCCESS;
	FLStatus fStatus;
	uint32 chunkSize;
	uint32 actualLength;
	#define READ_MAX 65536
	// Read first chunk
	fStatus = flReadChannelAsyncSubmit(handle, channel, 4, NULL, error);
	//CHECK_STATUS(fStatus, FLP_LIBERR, cleanup, "doRead()");
	
	// Await last chunk
	fStatus = flReadChannelAsyncAwait(handle, &recvData, &actualLength, &actualLength, error);
	//CHECK_STATUS(fStatus, FLP_LIBERR, cleanup, "readchannel()");
		
	int tempdata[32];
	int temp[8];
	//convert received data into binary and populating coordinates array
	for(int j=0;j<4;j++){
		binary(recvData[j],temp);
		for (int i=0;i<8;i++){
			tempdata[8*j+i] = temp[i];					
		}
	}
	decry(tempdata,data);
	cleanup:
		return retVal;
		
}
void writechannel(int channel,int data[32],struct FLContext *handle, const char *error){
	//ReturnCode retVal = FLP_SUCCESS, pStatus;
	int encrypted[32];
	encry(data,encrypted);
	FLStatus fStatus;
	
	int temp[8];
	uint8* datatowrite;
	datatowrite=malloc(4);
	for(int i=0;i<4;i++){
		for(int j=0;j<8;j++){
			temp[j]=encrypted[8*i+j];
		}
		datatowrite[i]=binarytoint(temp);

	}
	fStatus = flWriteChannel(handle, (uint8)channel, 4, datatowrite, &error);

}

// A function to identify the channel for communication
void getChannel(struct FLContext *handle,const char *error){
	while (true) {
		for(int i=0;i<64;i++){
			printf("Reading Encrypted Coordinates from channel %d \n",2*i);
			
			int decrypted[32];
			readchannel(decrypted,2*i,handle,&error);
			
			//extracting X and Y Coordinates in binary form from decrypted data
			int binaryX[4],binaryY[4];
			for (int i=0;i<4;i++){
				binaryX[i]=decrypted[27-i];
				binaryY[i]=decrypted[31-i];
			}
			//converting X,Y coordinates in integer from binary
			int Xcord = binaryX[0]+binaryX[1]*2+binaryX[2]*4+binaryX[3]*8;
			int Ycord = binaryY[0]+binaryY[1]*2+binaryY[2]*4+binaryY[3]*8;

			printf("X-Coordinate = %d\n",Xcord );
			printf("Y-Coordinate = %d\n",Ycord );

			printf("Rewriting received Coodinates on channel %d\n",2*i+1);
			
			writechannel(2*i+1,decrypted,handle,error);

			printf("Rewriting Coordinates Completed. Waiting for ACK1 \n");
			sleep(1);

			printf("Reading Channel %d for ACK1\n",2*i);

			int ack1[32];
			readchannel(ack1,2*i,handle,&error);
			char hex1[9];
			hexadecimal(ack1, hex1, 32);
			hex1[8] = '\0';
			
			bool ackreceived=true;
			for(int i=0;i<32;i++){
				if(ack1[i]!=ACK1[i]-'0'){
					ackreceived=false;
					break;
				}
			}
			// 
			if(ackreceived){
				CHANNEL = 2*i;
				X=Xcord;
				Y=Ycord;
				printf("Writing ACK2 on channel %d\n\n",2*i+1);
				int ack2[32];
				for(int i=0;i<32;i++){
					ack2[i]=ACK2[i]-'0';
				}
				writechannel(2*i+1,ack2,handle,error);
				return;
			}
			else{
				sleep(5);
				printf("Reading Channel %d again after 5seconds for ACK1\n\n",2*i);
				readchannel(ack1,2*i,handle,&error);
				ackreceived=true;
				for(int i=0;i<32;i++){
					if(ack1[i]!=ACK1[i]-'0'){
						ackreceived=false;
						break;
					}
				}
				if(ackreceived){
					printf("Channel Identified successfully\n");

					CHANNEL = 2*i;
					printf("Channel=%d\n",CHANNEL );
					X=Xcord;
					Y=Ycord;
					printf("Writing ACK2 on channel %d\n\n",2*i+1);
					int ack2[32];
					for(int i=0;i<32;i++){
						ack2[i]=ACK2[i]-'0';
					}
					writechannel(2*i+1,ack2,handle,error);
					return;
				}
			}	
		
		}
	}
}

int main(int argc, char *argv[]) {
	ReturnCode retVal = FLP_SUCCESS, pStatus;
	struct arg_str *ivpOpt = arg_str0("i", "ivp", "<VID:PID>", "            vendor ID and product ID (e.g 04B4:8613)");
	struct arg_str *vpOpt = arg_str1("v", "vp", "<VID:PID[:DID]>", "       VID, PID and opt. dev ID (e.g 1D50:602B:0001)");
	struct arg_str *fwOpt = arg_str0("f", "fw", "<firmware.hex>", "        firmware to RAM-load (or use std fw)");
	struct arg_str *portOpt = arg_str0("d", "ports", "<bitCfg[,bitCfg]*>", " read/write digital ports (e.g B13+,C1-,B2?)");
	struct arg_str *queryOpt = arg_str0("q", "query", "<jtagBits>", "         query the JTAG chain");
	struct arg_str *progOpt = arg_str0("p", "program", "<config>", "         program a device");
	struct arg_uint *conOpt = arg_uint0("c", "conduit", "<conduit>", "        which comm conduit to choose (default 0x01)");
	struct arg_str *actOpt = arg_str0("a", "action", "<actionString>", "    a series of CommFPGA actions");
	struct arg_lit *shellOpt  = arg_lit0("s", "shell", "                    start up an interactive CommFPGA session");
	struct arg_lit *benOpt  = arg_lit0("b", "benchmark", "                enable benchmarking & checksumming");
	struct arg_lit *rstOpt  = arg_lit0("r", "reset", "                    reset the bulk endpoints");
	struct arg_str *dumpOpt = arg_str0("l", "dumploop", "<ch:file.bin>", "   write data from channel ch to file");
	struct arg_lit *helpOpt  = arg_lit0("h", "help", "                     print this help and exit");
	struct arg_str *eepromOpt  = arg_str0(NULL, "eeprom", "<std|fw.hex|fw.iic>", "   write firmware to FX2's EEPROM (!!)");
	struct arg_str *backupOpt  = arg_str0(NULL, "backup", "<kbitSize:fw.iic>", "     backup FX2's EEPROM (e.g 128:fw.iic)\n");
	struct arg_end *endOpt   = arg_end(20);
	struct arg_lit *sigOpt = arg_lit0("z", "signal", "                   Railway Signal Controller");

	void *argTable[] = {
		ivpOpt, vpOpt, fwOpt, portOpt, queryOpt, progOpt, conOpt, actOpt,
		shellOpt, benOpt, rstOpt, dumpOpt, helpOpt, eepromOpt, backupOpt, endOpt,sigOpt
	};
	const char *progName = "flcli";
	int numErrors;
	struct FLContext *handle = NULL;
	FLStatus fStatus;
	const char *error = NULL;
	const char *ivp = NULL;
	const char *vp = NULL;
	bool isNeroCapable, isCommCapable;
	uint32 numDevices, scanChain[16], i;
	const char *line = NULL;
	uint8 conduit = 0x01;

	if ( arg_nullcheck(argTable) != 0 ) {
		fprintf(stderr, "%s: insufficient memory\n", progName);
		FAIL(1, cleanup);
	}

	numErrors = arg_parse(argc, argv, argTable);

	if ( helpOpt->count > 0 ) {
		printf("FPGALink Command-Line Interface Copyright (C) 2012-2014 Chris McClelland\n\nUsage: %s", progName);
		arg_print_syntax(stdout, argTable, "\n");
		printf("\nInteract with an FPGALink device.\n\n");
		arg_print_glossary(stdout, argTable,"  %-10s %s\n");
		FAIL(FLP_SUCCESS, cleanup);
	}

	if ( numErrors > 0 ) {
		arg_print_errors(stdout, endOpt, progName);
		fprintf(stderr, "Try '%s --help' for more information.\n", progName);
		FAIL(FLP_ARGS, cleanup);
	}

	fStatus = flInitialise(0, &error);
	CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);

	vp = vpOpt->sval[0];

	printf("Attempting to open connection to FPGALink device %s...\n", vp);
	fStatus = flOpen(vp, &handle, NULL);
	if ( fStatus ) {
		if ( ivpOpt->count ) {
			int count = 60;
			uint8 flag;
			ivp = ivpOpt->sval[0];
			printf("Loading firmware into %s...\n", ivp);
			if ( fwOpt->count ) {
				fStatus = flLoadCustomFirmware(ivp, fwOpt->sval[0], &error);
			} else {
				fStatus = flLoadStandardFirmware(ivp, vp, &error);
			}
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
			
			printf("Awaiting renumeration");
			flSleep(1000);
			do {
				printf(".");
				fflush(stdout);
				fStatus = flIsDeviceAvailable(vp, &flag, &error);
				CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
				flSleep(250);
				count--;
			} while ( !flag && count );
			printf("\n");
			if ( !flag ) {
				fprintf(stderr, "FPGALink device did not renumerate properly as %s\n", vp);
				FAIL(FLP_LIBERR, cleanup);
			}

			printf("Attempting to open connection to FPGLink device %s again...\n", vp);
			fStatus = flOpen(vp, &handle, &error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
		} else {
			fprintf(stderr, "Could not open FPGALink device at %s and no initial VID:PID was supplied\n", vp);
			FAIL(FLP_ARGS, cleanup);
		}
	}

	printf(
		"Connected to FPGALink device %s (firmwareID: 0x%04X, firmwareVersion: 0x%08X)\n",
		vp, flGetFirmwareID(handle), flGetFirmwareVersion(handle)
	);

	if ( eepromOpt->count ) {
		if ( !strcmp("std", eepromOpt->sval[0]) ) {
			printf("Writing the standard FPGALink firmware to the FX2's EEPROM...\n");
			fStatus = flFlashStandardFirmware(handle, vp, &error);
		} else {
			printf("Writing custom FPGALink firmware from %s to the FX2's EEPROM...\n", eepromOpt->sval[0]);
			fStatus = flFlashCustomFirmware(handle, eepromOpt->sval[0], &error);
		}
		CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
	}

	if ( backupOpt->count ) {
		const char *fileName;
		const uint32 kbitSize = strtoul(backupOpt->sval[0], (char**)&fileName, 0);
		if ( *fileName != ':' ) {
			fprintf(stderr, "%s: invalid argument to option --backup=<kbitSize:fw.iic>\n", progName);
			FAIL(FLP_ARGS, cleanup);
		}
		fileName++;
		printf("Saving a backup of %d kbit from the FX2's EEPROM to %s...\n", kbitSize, fileName);
		fStatus = flSaveFirmware(handle, kbitSize, fileName, &error);
		CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
	}

	if ( rstOpt->count ) {
		// Reset the bulk endpoints (only needed in some virtualised environments)
		fStatus = flResetToggle(handle, &error);
		CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
	}

	if ( conOpt->count ) {
		conduit = (uint8)conOpt->ival[0];
	}

	isNeroCapable = flIsNeroCapable(handle);
	isCommCapable = flIsCommCapable(handle, conduit);

	if ( portOpt->count ) {
		uint32 readState;
		char hex[9];
		const uint8 *p = (const uint8 *)hex;
		printf("Configuring ports...\n");
		fStatus = flMultiBitPortAccess(handle, portOpt->sval[0], &readState, &error);
		CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
		sprintf(hex, "%08X", readState);
		printf("Readback:   28   24   20   16    12    8    4    0\n          %s", nibbles[*p++ - '0']);
		printf(" %s", nibbles[*p++ - '0']);
		printf(" %s", nibbles[*p++ - '0']);
		printf(" %s", nibbles[*p++ - '0']);
		printf("  %s", nibbles[*p++ - '0']);
		printf(" %s", nibbles[*p++ - '0']);
		printf(" %s", nibbles[*p++ - '0']);
		printf(" %s\n", nibbles[*p++ - '0']);
		flSleep(100);
	}

	if ( queryOpt->count ) {
		if ( isNeroCapable ) {
			fStatus = flSelectConduit(handle, 0x00, &error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
			fStatus = jtagScanChain(handle, queryOpt->sval[0], &numDevices, scanChain, 16, &error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
			if ( numDevices ) {
				printf("The FPGALink device at %s scanned its JTAG chain, yielding:\n", vp);
				for ( i = 0; i < numDevices; i++ ) {
					printf("  0x%08X\n", scanChain[i]);
				}
			} else {
				printf("The FPGALink device at %s scanned its JTAG chain but did not find any attached devices\n", vp);
			}
		} else {
			fprintf(stderr, "JTAG chain scan requested but FPGALink device at %s does not support NeroProg\n", vp);
			FAIL(FLP_ARGS, cleanup);
		}
	}

	if ( progOpt->count ) {
		printf("Programming device...\n");
		if ( isNeroCapable ) {
			fStatus = flSelectConduit(handle, 0x00, &error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
			fStatus = flProgram(handle, progOpt->sval[0], NULL, &error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
		} else {
			fprintf(stderr, "Program operation requested but device at %s does not support NeroProg\n", vp);
			FAIL(FLP_ARGS, cleanup);
		}
	}

	if ( benOpt->count ) {
		enableBenchmarking = true;
	}
	
	if ( actOpt->count ) {
		printf("Executing CommFPGA actions on FPGALink device %s...\n", vp);
		if ( isCommCapable ) {
			uint8 isRunning;
			fStatus = flSelectConduit(handle, conduit, &error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
			fStatus = flIsFPGARunning(handle, &isRunning, &error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
			if ( isRunning ) {
				/*FLStatus fStatus;
				uint32 actualLength;
				fStatus = flReadChannelAsyncSubmit(handle, 0, 1, NULL, error);
				CHECK_STATUS(fStatus, FLP_LIBERR, cleanup, "doRead()");

				fStatus = flReadChannelAsyncAwait(handle, &recvData, &actualLength, &actualLength, error);
				CHECK_STATUS(fStatus, FLP_LIBERR, cleanup, "doRead()");

				printf("%d\n",recvData[0]);

				fStatus = flReadChannelAsyncSubmit(handle, 0, 1, NULL, error);
				fStatus = flReadChannelAsyncAwait(handle, &recvData, &actualLength, &actualLength, error);

				printf("%d\n",recvData[0]);**/
				
				// Reading data from txt file and populating in data

				struct coordata data[8][8];
				for(int j=0;j<8;j++){
        			for(int k=0;k<8;k++){
           				 for(int l=0;l<8;l++){
               				data[j][k].data[l][0]=0; //exist
               				data[j][k].data[l][1]=0; //ok
               				data[j][k].data[l][2]=0; // nextsignal
            			}
        			}
   				}
   				printf("\n\n");
   				printf("Reading data from network.txt \n");
				char c;
    			FILE *fptr;
    			fptr = fopen("/home/sourabh/network.txt", "r");
			    if (fptr == NULL){
			        printf("Error! reading file");
			        // Program exits if file pointer returns NULL.
			        exit(1);         
			    }
			    int arr[5];
			    int eol = 0;
			    for (c = getc(fptr); c != EOF; c = getc(fptr)){
			        if(c=='\n'){
			            eol = 0;
			            data[arr[0]][arr[1]].data[arr[2]][0] = 1;
			            data[arr[0]][arr[1]].data[arr[2]][1] = arr[3];
			            data[arr[0]][arr[1]].data[arr[2]][2] = arr[4];
			        }
			        else if(c==','){
			        }
			        else{
			            arr[eol]=c - '0';
			            eol=eol+1;
			        }
			    }
			    fclose(fptr);
  

			    printf("Table read successfully \n\n\n");
				while(true){
					
					sleep(4);
				    printf("Trying to find channel for communication with FPGA Board\n\n");
				    getChannel(handle,error);
					
					
					
					
					printf("Searching the data in table to send \n");
					// preparing original data to write in binary
					int datatosend1[32];
					int datatosend2[32];
					for(int i=0;i<8;i++){
						int dir[8],nextsignal[8];
						binary(i,dir);
						binary(data[X][Y].data[i][2],nextsignal);	
						int binarydata[8];
					
						for(int j=0;j<3;j++){
							//printf("%d\n",dir[2-j] );
							binarydata[2+j] = dir[5+j];
						}
						for(int j=0;j<3;j++){
							binarydata[5+j] = nextsignal[5+j];
						}
						
						binarydata[0] = data[X][Y].data[i][0];
						binarydata[1] = data[X][Y].data[i][1];
						if(i<4){
							for(int j=0;j<8;j++){
								datatosend1[8*i+j]=binarydata[j];
							}
						}
						else{
							for(int j=0;j<8;j++){
								datatosend2[8*(i-4)+j]=binarydata[j];
							}
						}
						
					}
					printf("First 4 Bytes of Data : ");
					for(int i=0;i<32;i++){
						printf("%d",datatosend1[i] );
					}
					printf("\n");
					printf("Last 4 Bytes of Data : ");
					for(int i=0;i<32;i++){
						printf("%d",datatosend2[i] );
					}
					printf("\n");
					
					printf("Writing First 4 Bytes on channel%d \n",CHANNEL+1);
					writechannel(CHANNEL+1,datatosend1,handle,error);

					printf("Waiting For ACK1 \n");
					int ack[32];
					bool ACK =false;
					bool ackreceived;
					for(int j=0;j<256;j++){
						readchannel(ack,CHANNEL,handle,&error);
						ackreceived=true;
						for(int i=0;i<32;i++){
							if(ack[i]!=ACK1[i]-'0'){
								ackreceived=false;
								break;
							}
						}
						if(ackreceived){
							ACK=true;break;
						}
						sleep(1);
					}
					if(!ACK){
						continue;
					}
					else{
						printf("Writing Last 4 Bytes on channel%d \n",CHANNEL+1);
						writechannel(CHANNEL+1,datatosend2,handle,error);
						printf("Waiting For ACK1 \n");
						ACK =false;
						for(int j=0;j<256;j++){
							readchannel(ack,CHANNEL,handle,&error);
							ackreceived=true;
							for(int i=0;i<32;i++){
								if(ack[i]!=ACK1[i]-'0'){
									ackreceived=false;
									break;
								}
							}
							if(ackreceived){
								ACK=true;break;
							}
							sleep(1);
						}
						if(!ACK){
							continue;
						}
						else{
							printf("Writing ACK2 on channel %d\n",CHANNEL+1);
							int ack2[32];
							for(int i=0;i<32;i++){
								ack2[i]=ACK2[i]-'0';
							}
							writechannel(CHANNEL+1,ack2,handle,error);
						}
					}
					printf("Ack2 written\n");
					sleep(24);
					int dataupdate[32];
					//for(int i=0;i<32;i++)dataupdate[i]=-1;
					//for(int j=0;j<T1;j++){
						readchannel(dataupdate,CHANNEL,handle,&error);

						

						bool datareceived=false;
						for(int i=0;i<32;i++){
							if(dataupdate[i]!=1){
								datareceived=true;
								break;
							}
						}
						if(datareceived){
							printf("Table Update Requested\n");
							for(int i=0;i<32;i++){
								printf("%d",dataupdate[i] );
							}
							printf("\n");
							int direction = dataupdate[26]*4+dataupdate[27]*2+dataupdate[28];
							int nextsig = dataupdate[29]*4+dataupdate[30]*2+dataupdate[31];
							if(data[X][Y].data[direction][0] == 1){
								data[X][Y].data[direction][1]= dataupdate[25];
								data[X][Y].data[direction][2]= nextsig;
								printf("Table Updated Successful for direction %d nextsignal = %d ,Ok = %d\n",direction,nextsig,data[X][Y].data[direction][1]);

							}
							else{
								printf("Track does not exist\n");
							}
							//break;
						}
						else{
							sleep(16);
							printf("Table Update not requested\n");
						}
						//sleep(1);
					//}
					
					printf("\n\n\n\n");
					

					



				}
					
			} else {
				fprintf(stderr, "The FPGALink device at %s is not ready to talk - did you forget --program?\n", vp);
				FAIL(FLP_ARGS, cleanup);
			}
		} else {
			fprintf(stderr, "Action requested but device at %s does not support CommFPGA\n", vp);
			FAIL(FLP_ARGS, cleanup);
		}

	}

	if ( dumpOpt->count ) {
		const char *fileName;
		unsigned long chan = strtoul(dumpOpt->sval[0], (char**)&fileName, 10);
		FILE *file = NULL;
		const uint8 *recvData;
		uint32 actualLength;
		if ( *fileName != ':' ) {
			fprintf(stderr, "%s: invalid argument to option -l|--dumploop=<ch:file.bin>\n", progName);
			FAIL(FLP_ARGS, cleanup);
		}
		fileName++;
		printf("Copying from channel %lu to %s", chan, fileName);
		file = fopen(fileName, "wb");
		CHECK_STATUS(!file, FLP_CANNOT_SAVE, cleanup);
		sigRegisterHandler();
		fStatus = flSelectConduit(handle, conduit, &error);
		CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
		fStatus = flReadChannelAsyncSubmit(handle, (uint8)chan, 22528, NULL, &error);
		CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
		do {
			fStatus = flReadChannelAsyncSubmit(handle, (uint8)chan, 22528, NULL, &error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
			fStatus = flReadChannelAsyncAwait(handle, &recvData, &actualLength, &actualLength, &error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
			fwrite(recvData, 1, actualLength, file);
			printf(".");
		} while ( !sigIsRaised() );
		printf("\nCaught SIGINT, quitting...\n");
		fStatus = flReadChannelAsyncAwait(handle, &recvData, &actualLength, &actualLength, &error);
		CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
		fwrite(recvData, 1, actualLength, file);
		fclose(file);
	}

	if ( shellOpt->count ) {
		printf("\nEntering CommFPGA command-line mode:\n");
		if ( isCommCapable ) {
		   uint8 isRunning;
			fStatus = flSelectConduit(handle, conduit, &error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
			fStatus = flIsFPGARunning(handle, &isRunning, &error);
			CHECK_STATUS(fStatus, FLP_LIBERR, cleanup);
			if ( isRunning ) {
				do {
					do {
						line = readline("> ");
					} while ( line && !line[0] );
					if ( line && line[0] && line[0] != 'q' ) {
						add_history(line);
						pStatus = parseLine(handle, line, &error);
						CHECK_STATUS(pStatus, pStatus, cleanup);
						free((void*)line);
					}
				} while ( line && line[0] != 'q' );
			} else {
				fprintf(stderr, "The FPGALink device at %s is not ready to talk - did you forget --xsvf?\n", vp);
				FAIL(FLP_ARGS, cleanup);
			}
		} else {
			fprintf(stderr, "Shell requested but device at %s does not support CommFPGA\n", vp);
			FAIL(FLP_ARGS, cleanup);
		}
	}
	if(sigOpt->count){
		printf("Successful\n");
	}

	

cleanup:
	free((void*)line);
	flClose(handle);
	if ( error ) {
		fprintf(stderr, "%s\n", error);
		flFreeError(error);
	}
	return retVal;
}

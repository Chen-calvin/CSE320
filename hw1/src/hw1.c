#include "hw1.h"
#include <stdio.h>
// For your helper functions (you may add additional files also)
// DO NOT define a main function here!

char* HELPFLAG = "-h";
char* SUBFLAG = "-s";
char* TUTFLAG = "-t";
char* ENFLAG = "-e";
char* DEFLAG = "-d";
char DASH = '-';

int stringLength(char* str){
	int length = 0;
	char currChar = '*';
	for(length = 0; currChar != '\0'; length++)
		currChar = *(str+length);

	return length - 1;
}

int validIntString(char* str){
	char currChar = *str;
	int i = 1;
	for(i = 1; currChar != '\0'; i++){
		if((int)currChar > 57 || (int)currChar < 48)
			return FALSE;
		currChar = *(str+i);
	}

	return TRUE;
}

int stringToInt(char* str)
{
	int val = 0;

	int i = 0;
	char currChar = *str;
	for(i = 1; currChar != '\0'; i++){
		val = (val * 10) + (int)currChar - 48;
		currChar = *(str+i);
	}

	return val;
}

int checkFlag(char* str1, char* str2){
	int validFlag = FALSE;
	if((*str1 == *str2) && (*(str1+1) == *(str2+1)))
		validFlag = TRUE;

	return validFlag;
}

char validargs(int argc, char** argv, FILE** in, FILE** out) {
	char ret = 0;

	if(argc != 2 && argc != 5 && argc != 6){ //error if # of args < 2 or > 6
		printf("Wrong args \n");
		return 0;
	}

	char* arg1 = *(argv + 1);		//Help, sub, or tut
	if(checkFlag(arg1, HELPFLAG) && argc == 2)
		return 0x80;
	else if(checkFlag(arg1, SUBFLAG) && (argc == 5 || argc == 6)){
		ret = ret | 0x40;
		}
		else if(checkFlag(arg1, TUTFLAG) && argc == 5)
			ret = ret | 0x00;
			else
				return 0;

	char* arg2 = *(argv + 2);	//Encode or decode
	if(checkFlag(arg2, DEFLAG))
		ret = ret | 0x20;
	else if(checkFlag(arg2, ENFLAG))
		ret = ret | 0x00;
		else
			return 0;

	char* arg3 = *(argv + 3);	//input path
	if(*arg3 == DASH)
		*in = stdin;
	else
		*in = fopen(arg3, "r");

	char* arg4 = *(argv + 4);	//output path
	if(*arg4 == DASH)
		*out = stdout;
	else
		*out = fopen(arg4, "w");
	if(*out == NULL)
		return 0;

	char* arg5;
	int shiftVal;
	if(argc == 6){
		arg5 = *(argv+5);
		if(validIntString(arg5)){
			shiftVal = stringToInt(arg5) % stringLength(Alphabet);
		}
		else
			return 0;
	}
	else
		shiftVal = 320 % stringLength(Alphabet);

	ret = ret | shiftVal;
	return ret;
}

int charPosition(char* Alphabet, char letter){
	int pos = -1;
	char currChar = *Alphabet;
	for (int i = 0; currChar != '\0'; i++){
		currChar = *(Alphabet + i);
		if(currChar == letter)
			return i;
	}

	return pos;
}

char encodeChar(char* Alphabet, char letter, int n)
{
	int oldPos = charPosition(Alphabet, letter);
	if(oldPos == -1)
		return letter;

	int length = stringLength(Alphabet);

	int newPos = (oldPos + n)%length;
	return *(Alphabet + newPos);
}

void subEncode(FILE** in, FILE** out, int n){
	char currChar = (char)fgetc(*in);;
	char encodedChar;
	while(currChar != EOF)
	{
		if((int)currChar >= 97 && (int)currChar <= 122){
			currChar = (currChar - 32);
		}
		encodedChar = encodeChar(Alphabet, currChar, n);
		fprintf(*out, "%c", encodedChar);
		currChar = (char)fgetc(*in);
	}
}

char decodeChar(char* Alphabet, char letter, int n)
{
	int oldPos = charPosition(Alphabet, letter);
	if(oldPos == -1)
		return letter;

	int length = stringLength(Alphabet);

	int newPos = (oldPos - n)%length;
	if(newPos < 0)
		newPos = newPos + length;
	return *(Alphabet + newPos);
}

void subDecode(FILE** in, FILE** out, int n){
	char currChar = (char)fgetc(*in);
	char decodedChar;
	while(currChar != EOF)
	{
		decodedChar = decodeChar(Alphabet, currChar, n);
		fprintf(*out, "%c", decodedChar);
		currChar = (char)fgetc(*in);
	}
}

char* tutSyllable(char letter){
	char* currStr = *Tutnese;
	for(int i = 0; currStr != NULL; i++)
	{
		if(*currStr == letter)
			return currStr;
		currStr = *(Tutnese + i);
	}

	return NULL;
}

void tutEncode(FILE** in, FILE** out){
	char prevChar = (char)fgetc(*in);
	char currChar = (char)fgetc(*in);
	while(currChar != EOF && prevChar != EOF){
		for(int i = 0; i < 10; i++)
			*(buffer+i) = '\0';

		char lilPrevChar = prevChar;
		char lilCurrChar = currChar;
		if((int)prevChar > 65 && (int)prevChar < 90)	//If either char is capital, get a lowercase char
			lilPrevChar = (char)((int)prevChar + 32);
		if((int)currChar > 65 && (int)currChar < 90)
			lilCurrChar = (char)((int)currChar + 32);

		if(lilCurrChar == lilPrevChar){		//If both lowercase chars are the same, append Squa
			if(lilPrevChar == prevChar)		//If both lilPrevChar and prevChar are lowercase, append squa, else Squa
				*buffer = 's';
			else
				*buffer = 'S';

			if((int)lilPrevChar < 97 || (int)lilPrevChar > 122){
				*buffer = lilPrevChar;
				*(buffer+1) = lilPrevChar;
			}else{
				*(buffer+1) = 'q';
				*(buffer+2) = 'u';
				*(buffer+3) = 'a';
			}

			if(lilPrevChar == 'a' || lilPrevChar == 'e' || lilPrevChar == 'i' || lilPrevChar == 'o' || lilPrevChar == 'u'){		//If lilPrevChar is a vowel, append t
				*(buffer+4) = 't';
				*(buffer+5) = currChar;
			}
			else if(tutSyllable(lilCurrChar) == NULL)	//If theres no appropriate Tutnese syllable, append currChar
				*(buffer+4) = currChar;
				else{
					char* tut = tutSyllable(lilCurrChar);	//Else, append the Tutnese syllable
					char tmp = *tut + 1;
					*(buffer+4) = currChar;

					for(int i = 1; tmp != '\0'; i++){
						*(buffer + 4 + i) = tmp;
						tmp = *(tut + i);
					}
				}
			fprintf(*out, "%s", buffer);
			prevChar = (char)fgetc(*in);
			currChar = (char)fgetc(*in);
		}
		else{									//prevChar and currChar are not the same letter
			char* tut = tutSyllable(lilPrevChar);
			if(tut == NULL){	//If theres no appropriate Tutnese syllable, append prevChar
				*(buffer) = prevChar;
			}
			else{ 					//Else, append the Tutnese syllable
				char tmp = *(tut + 1);
				*(buffer) = prevChar;
				for(int i = 1; tmp != '\0'; i++){
					*(buffer + i) = tmp;
					tmp = *(tut + i + 1);
				}
			}
			fprintf(*out, "%s", buffer);
			prevChar = currChar;
			currChar = (char)fgetc(*in);
		}

	}
}

void tutDecode(FILE** in, FILE** out){
	char currChar = fgetc(*in);
	char lilChar;
	while(currChar != EOF){
		lilChar = currChar;
		if((int)currChar < 65 && (int)currChar > 90)
			lilChar = (char)((int)currChar + 32);
		char* tut = tutSyllable(lilChar);
		fprintf(*out,"%c", currChar);

		if(tut != NULL){
			int length = stringLength(tut);
			for(int i = 0; i < length; i++)
				currChar = fgetc(*in);
		}
		else
			currChar = fgetc(*in);
	}
}

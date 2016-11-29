#ifndef CONVERSION_H
#define CONVERSION_H


RC intToChar(int n, char* buffer)
{
	buffer[0] = n & (unsigned) 255;
	buffer[1] = (n >> 8) & (unsigned) 255;
	buffer[2] = (n >> 16) & (unsigned) 255;
	buffer[3] = (n >> 24) & (unsigned) 255;

//	printf("%i %i %i %i\n", buffer[0], buffer[1], buffer[2], buffer[3]);

	return 0;
}

RC charToInt(char* buffer, int& n)
{
	int x = 0;

	x |= (unsigned char) buffer[3];
	x = x << 8;
	x |= (unsigned char) buffer[2];
	x = x << 8;
	x = x | (unsigned char) buffer[1];
	x = x << 8;
	x = x | (unsigned char) buffer[0];	

	n = x;
	
//	printf("%i\n", x);

	return 0;
}

#endif // CONVERSION_H
//****************************************************************************
//
// Copyright SN Systems Ltd 2011
//
// Description: EndianSwap.c demonstrates the endianSwap functions available in
//  the scripting language.
//
//****************************************************************************

//  Note that the results for these swaps are iterative for the data buffer examples.
//  First SwapBuffer_2 is applied, then SwapBuffer_4 is applied to the output
//  of SwapBuffer_2.

int main(int argc, char ** argv)
{
	sn_uint16 smallInt = 22222;
	sn_uint32 biggerInt = 329847;
    sn_uint32 LotsOfNumbers[20];
	sn_uint32 i;

	for (i=0 ; i<20 ; i++)
	{
		LotsOfNumbers[i] = (13 << i) + (i*i);
		printf("LotsOfNumbers[%i] = %08x\n", i, LotsOfNumbers[i]);
	}

	printf("\n\nSwapped using SwapBuffer_2\n\n");

	EndianSwapBuffer_2(LotsOfNumbers, 80);

	for (i=0 ; i<20 ; i++)
	{
		printf("Swapped LotsOfNumbers[%i] = %08x\n", i, LotsOfNumbers[i]);
	}

	printf("\n\nSwapped using SwapBuffer_4\n\n");

	EndianSwapBuffer_4(LotsOfNumbers, 80);

	for (i=0 ; i<20 ; i++)
	{
		printf("Swapped LotsOfNumbers[%i] = %08x\n", i, LotsOfNumbers[i]);
	}


	printf("\nIndividual Swaps\n\nSmall = %04x \nBig = %08x\n", smallInt, biggerInt);

	smallInt = EndianSwap_2(smallInt);
	biggerInt = EndianSwap_4(biggerInt);

	printf("Swapped Small = %04x  \nSwapped Big = %08x\n", smallInt, biggerInt);

	return 0;
}


//****************************************************************************
//
// Copyright SN Systems Ltd 2011
//
// Description: Font.c shows how to retrieve and parse a font list from your
//  system.  The GraphicsSetFont command is then demonstrated and some example
//  text is written to the screen.
//
//****************************************************************************

int main (int argc, char ** argv)
{
	sn_uint32 nFontListSize;
	char * pFontList;
	char * pFontName;

	TextHome();
	nFontListSize = GraphicsGetFontList(NULL);

	if (nFontListSize > 0)
	{
		pFontList = malloc(nFontListSize);

		if (SN_SUCCEEDED(GraphicsGetFontList(pFontList)))
		{
			pFontName = pFontList;
			while (*pFontName != '\0')
			{
				printf("%s\n", pFontName);
				pFontName += strlen(pFontName) + 1;
			}
		}
		free(pFontList);
	}
	else
	{
		printf ("No Font list?\n");
	}

	
	GraphicsSetWindowSize(400, 200);
	GraphicsSetDrawMode(DF_GRAPHICS, DM_SRC_AND_DST);
	GraphicsSetForegroundColor(0,0,0);
	GraphicsSetBackgroundColor(200, 200, 200);
	
	GraphicsSetFont("Arial", 14);			//  Sets the graphical text output to 14 high Arial
	GraphicsClearWindow();
	GraphicsDrawText(0,0, "Arial font size 14");
	GraphicsSetFont("Arial", 16);
	GraphicsDrawText(0, 15, "Arial font size 16");
	GraphicsSetFont("System", 14);
	GraphicsDrawText(0, 30, "System font size 14");
	GraphicsUpdate();
	
	return 0;
}

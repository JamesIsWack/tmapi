//****************************************************************************
//
// Copyright SN Systems Ltd 2011
//
// Description: DrawModes.c is used to demonstrate all of the possible draw modes
//  available for combining graphical and text output to the screen.  The keydown
//  notification is used to step through the different modes.
//
//****************************************************************************

char * ModeStr(sn_int32 iMode)
{
	switch(iMode)
	{
	case DM_SRC:
		return "DM_SRC";
	case DM_SRC_OR_DST:
		return "DM_SRC_OR_DST";
	case DM_SRC_AND_DST:
		return "DM_SRC_AND_DST";
	case DM_SRC_XOR_DST:
		return "DM_SRC_XOR_DST";
	case DM_SRC_AND_NOT_DST:
		return "DM_SRC_AND_NOT_DST";
	case DM_NOT_SRC:
		return "DM_NOT_SRC";
	case DM_NOT_SRC_AND_NOT_DST:
		return "DM_NOT_SRC_AND_NOT_DST";
	case DM_NOT_SRC_OR_DST:
		return "DM_NOT_SRC_OR_DST";
	case DM_NOT_DST:
		return "DM_NOT_DST";
	default:
		return "UNKNOWN TYPE";
	}
}



void RunTestMode(sn_int32 iMode, sn_uint32 uWidth, sn_uint32 uHeight)
{
	sn_int32 i;		//  Loop
	sn_notify Notify;
	GraphicsSetDrawMode(DF_TEXT | DF_GRAPHICS, iMode);

	GraphicsSetForegroundColor(0, 0, 0);
	GraphicsDrawRectangle(0, 0, uWidth/3, uHeight);
	GraphicsSetForegroundColor(127, 127, 127);
	GraphicsDrawRectangle(uWidth/3, 0, uWidth*2/3, uHeight);

	
	for (i=0 ; i<30 ; i++)
	{
		printf("Testing mode: %s;   ", ModeStr(iMode));
		if (i%5==0)
			printf("\n");
	}

	GraphicsUpdate();
	GetNotification(NT_KEYDOWN, &Notify);
}
	

void DoTests(sn_uint32 uWidth, sn_uint32 uHeight)
{
	sn_notify Notify;
	
	RequestNotification(NT_KEYDOWN);			//  Use this to perform next test...
	GetNotification(NT_KEYDOWN, &Notify);

	RunTestMode(DM_SRC, uWidth, uHeight);
	RunTestMode(DM_SRC_OR_DST, uWidth, uHeight);
	RunTestMode(DM_SRC_AND_DST, uWidth, uHeight);
	RunTestMode(DM_SRC_XOR_DST, uWidth, uHeight);
	RunTestMode(DM_SRC_AND_NOT_DST, uWidth, uHeight);
	RunTestMode(DM_NOT_SRC, uWidth, uHeight);
	RunTestMode(DM_NOT_SRC_AND_NOT_DST, uWidth, uHeight);
	RunTestMode(DM_NOT_SRC_OR_DST, uWidth, uHeight);
	RunTestMode(DM_NOT_DST, uWidth, uHeight);
}


int main (int argc, char ** argv)
{
	sn_uint32 uWidth;
	sn_uint32 uHeight;
	
    TextClear();

	GraphicsSetDrawMode(DF_TEXT | DF_GRAPHICS, DM_SRC);
	GraphicsGetWindowSize(&uWidth, &uHeight);
	GraphicsSetWindowSize(uWidth, uHeight);

	GraphicsSetForegroundColor(0, 0, 0);
	GraphicsDrawRectangle(0, 0, uWidth/3, uHeight);
	GraphicsSetForegroundColor(127, 127, 127);
	GraphicsDrawRectangle(uWidth/3, 0, uWidth*2/3, uHeight);


	GraphicsUpdate();

	DoTests(uWidth, uHeight);
	GraphicsSetDrawMode(DF_TEXT, DM_SRC_AND_DST);
	printf("\n\nTEST COMPLETED\n");
	return 0;
}




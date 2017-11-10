//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <tchar.h> // Defines _tmain
#include <iostream>
#include <mmsystem.h> //timeSetEvent  (link to winmm.lib)
#include "SafeCollections.h"
#include "SmartPtr.h"

using namespace SafeCollections;
using namespace SmartPointer;

#define MAX_LOCAL_PLAYERS 4

typedef struct
{
	// Input Ticks
	// -----------
	UINT  Tick;

	// Non-user Specific Input
	// -----------------------
	BOOL ActivateMenu;
	BOOL ExitGame;
	// todo: add remaining input variables

	// Player Input
	// ------------
	FLOAT SteerLR[MAX_LOCAL_PLAYERS];
	BOOL  Fire[MAX_LOCAL_PLAYERS];
	// todo: add remaining input variables

	// Must define release in order to make it a smart node
	void Release()
	{
		delete this;
	}
}Input;

// Describe a reference counting smart pointer of type PlayerInput
// ---------------------------------------------------------------
typedef SmartNode<Input> SmartInput;

class TimedEvent
{
public:
	virtual void Event()=0;
};

const UINT HZ_32 = 1000/32;
class Timer32Hz
{
public:

	Timer32Hz(TimedEvent * userEvent) : tick32Hz(0)
	{
		timedEvent = userEvent;

		timerID = timeSetEvent( HZ_32, HZ_32/2, TimerProcHelper, (DWORD_PTR)this, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
	}
	~Timer32Hz()
	{
		timeKillEvent(timerID);
	}
	// Static method calls back into Timer32Hz class object
	//
	static void CALLBACK
	TimerProcHelper(UINT timerHandle, UINT uMsg, DWORD_PTR userData, DWORD dw1, DWORD dw2)
	{
		Timer32Hz * timer = (Timer32Hz*)(userData);
		timer->TimerProc();
	}

	// In object timer event processing
	//
	void TimerProc()
	{
		// Call the passed in event handler
		timedEvent->Event();

		// Update timers
		// -------------
		++tick32Hz;
	}

	volatile UINT tick32Hz;
private:
	TimedEvent * timedEvent;
	volatile UINT timerID;
};

class InputMgr : public TimedEvent
{
public:
	SafeQ<SmartInput> q;

	// Input DInput
	InputMgr()
	{
		ResetCounter();
	}
	void SetNumPlayers(unsigned players)
	{
		numPlayers = players;
	}
	void ResetCounter()
	{
		tick = 0;
	}
	// Polls user Input
	void Event()
	{
		// Must create "new" object to place in queue
		// ------------------------------------------
		Input * input = new Input;

		input->Tick = tick;

		// Placeholder input.
		input->ActivateMenu = FALSE;
		input->ExitGame = FALSE;

		for (UINT i=0; i<numPlayers; i++)
		{
			// Placeholder input.
			input->Fire[i] = rand() & 1;
			input->SteerLR[i] = (FLOAT)rand();
		}
		SmartInput data(input);
		q.Push(data);
		++tick;

		//test
		//if (q.Count() > 0)
		//{
		//	SmartInput inp = q.Pop();
		//	inp->
		//}
	}

private:
	// The number to times input was polled
	volatile UINT tick;
	// Number of players input is polled for
	UINT numPlayers;
};

int _tmain(int argc, _TCHAR* argv[])
{

	InputMgr * inputMgr = new InputMgr();
	inputMgr->SetNumPlayers(2);

	Timer32Hz *timer = new Timer32Hz(inputMgr);

	for (int i=0; i<32; i++)
	{
		// Stop timer after 32 ticks so we can step through this sample
		// in the debugger without filling the queue too big.
		if (timer && timer->tick32Hz > 32)
		{
			delete timer;
			timer = NULL;
		}

		while(!inputMgr->q.Count())
		{
			Sleep(0);  // wait for input
		}
		SmartInput input = inputMgr->q.Pop();
		input->Tick;
	}
	return 0;
}


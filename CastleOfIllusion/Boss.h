#ifndef _BOSS_INCLUDE
#define _BOSS_INCLUDE

#include "Entity.h"

// The game's boss
class Boss : public Entity 
{
public:

private:

	// Idea: abuse the hell out of the TimedEvent system to perform all patterns 
	//   -> pretty compatible with the blocky boss idea

	// TODO: get an sprite and some minor animations for it
	// It needs some breakable blocks that deal damage
};

#endif // _BOSS_INCLUDE

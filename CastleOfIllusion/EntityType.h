#ifndef _ENTITY_TYPE_INCLUDE
#define _ENTITY_TYPE_INCLUDE

#include <ostream>
#include <string>

enum class EntityType 
{
	Unknown,
	Player,
	Enemy,
	Boss,
	Projectile,
	Coin,
	Cake,
	Platform
};

inline std::string toString(EntityType type) 
{
	switch (type) 
	{
	case EntityType::Player:
		return "Player";
	case EntityType::Enemy:
		return "Enemy";
	default:
		return "Unknown";
	}
}

// Prints the type
inline std::ostream& operator<<(std::ostream& os, EntityType const& type) 
{	
	os << toString(type);
	return os;
}

#endif // _ENTITY_TYPE_INCLUDE
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
	Platform,
	ThrowableTile,
	Void,
	Gem
};

inline std::string toString(EntityType type) 
{
	switch (type) 
	{
	case EntityType::Player:
		return "Player";
	case EntityType::Enemy:
		return "Enemy";
	case EntityType::Boss:
		return "Boss";
	case EntityType::Projectile:
		return "Projectile";
	case EntityType::Coin:
		return "Coin";
	case EntityType::Platform:
		return "Platform";
	case EntityType::ThrowableTile:
		return "ThrowableTile";
	case EntityType::Void:
		return "Void";
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
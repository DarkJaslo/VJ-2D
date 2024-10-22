#include "Enemy.h"
#include "Player.h"

void Enemy::collideWithEntity(Collision collision) 
{
	switch (collision.entity->getType()) 
	{
	case EntityType::Player:
		auto player = static_cast<Player*>(collision.entity);
		if (player->isAttacking()) 
		{
			// Die
		}
		break;
	default:
		break;
	}
}
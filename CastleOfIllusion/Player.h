#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE

#include "Sprite.h"
#include "TileMap.h"
#include "Entity.h"
#include "ThrowableTile.h"
#include "UI.h"

enum class PlayerState
{
    Idle,
    Moving,
    Sliding,
    Jumping,
    Falling,
    Crouching,
    AttackingUp,
    AttackingDown,
    HoldIdle,
    HoldMoving,
    HoldJumping,
    HoldFalling,
    Hurt,
};

// Represents a player.

class Player : public Entity
{

public:
    Player(glm::vec2 const& pos, 
           std::shared_ptr<TileMap> tilemap, 
           std::shared_ptr<UI> ui,
           glm::ivec2 const& tilemap_pos,
           glm::ivec2 const& sprite_size,
           glm::ivec2 const& collision_box_size,
           std::shared_ptr<ShaderProgram> shader_program);
    
    // Updates the player
    virtual void update(int delta_time) final override;

    // Called when the player collides with something
    virtual void collideWithEntity(Collision collision) final override;

    // Gets the state the player is currently in
    PlayerState getPlayerState() const;

    // Returns true iff the player is attacking
    bool isAttacking() const;

    // Returns the type of entity the player is
    virtual EntityType getType() const override { return EntityType::Player; }
    	
private:
    // Takes a hit from a damage source, losing 1 power and losing the "try" if no power is left
	void takeHit();

	// Gains power from eating cake
	void gainPower(unsigned int gain);

    // Gains points from coins or defeating enemies
    void gainPoints(unsigned int gain);
      
    // Calculates the velocity needed for the player to jump to height
    float calculateJumpVelocity(float height, float gravity) const;

    // Called when the player falls off the level
    void onFallOff();

    // Creates and configures the player animations
    void configureAnimations();
    
    // The player's state
    PlayerState m_state;

    glm::ivec2 m_tilemap_displ;

    // The points the player currently has
    int m_points = 0;

    // The power (or health) the player currently has
    int m_power;

    // The maximum (and initial) amount of power. In the original game, can be increased up to 5
    int m_max_power = 3;

    // The number of tries the player currently has
    int m_tries = 3;

    // The speed at which the player bounces up after attacking some entities like enemies
    static constexpr float S_BOUNCE_SPEED = -1.8f;
    
    // Pointer to the object the player is holding
    ThrowableTile* m_throwable_obj;

    // Returns true iff the player is holding an object
    bool m_has_object = false;

    // True iff the player is looking to the right
    bool m_looking_right = true;

    // true iff the player can currently grab an object
    bool m_can_grab = true;

    // Pointer to the game's UI
    std::shared_ptr<UI> m_ui;

    // True iff the player is currently invulnerable
    // it can't take damage from enemies, but it can still die if it falls oustide the level
    bool m_invulnerable = true;

    // The time (ms) the player is invulnerable after being hit
    int m_invulnerability_time = 2000;

    // True iff the player has to play the animation for being hurt
    bool m_hurt = false;
};

#endif // _PLAYER_INCLUDE

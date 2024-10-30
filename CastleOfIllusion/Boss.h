#ifndef _BOSS_INCLUDE
#define _BOSS_INCLUDE

#include "Entity.h"
#include "Camera.h"
#include "Gem.h"

class BossBlock : public Entity 
{
public:
	BossBlock(std::shared_ptr<TileMap> tilemap,
		std::shared_ptr<ShaderProgram> shader_program);

	virtual void update(int delta_time) override;

	virtual void render() override;

	void move(glm::ivec2 target_pos, int target_ms);

	void setNotMoved() { m_moved = false; }
	
	bool moved() const { return m_moved; }

	// Attaches the face to this block
	void attachFace(std::shared_ptr<Sprite> face) { m_face = face; }

	// Detaches the face from this block
	void detachFace() { m_face.reset(); }

	virtual EntityType getType() const override { return EntityType::Projectile; }

	virtual void collideWithEntity(Collision collision) override;

	void enableGravityAndDrag();

private:
	// The face that appears on blocks, set if it is in this block right now
	std::shared_ptr<Sprite> m_face;

	// True iff this block is moving
	bool m_moving = false;

	// True iff the block has moved
	bool m_moved = false;

	// The int this block is moving to, if moving
	int m_target_x;
};

// The game's boss
class Boss : public Entity 
{
public:
	Boss(glm::ivec2 spawn_pos, 
		glm::ivec2 other_pos, 
		glm::ivec2 quad_size,
		std::shared_ptr<TileMap> tilemap,
		std::shared_ptr<ShaderProgram> shader_program,
		std::string const& boss_texture_path,
		std::string const& blocks_texture_path,
		std::shared_ptr<Camera> camera);

	virtual void update(int delta_time) override;

	virtual void render() override;

	virtual EntityType getType() const override { return EntityType::Boss; }

	virtual void setPosition(glm::ivec2 new_position) override;

	virtual void changePosition(glm::ivec2 change) override;

	virtual void collideWithEntity(Collision collision) override;

	virtual void setEnabled(bool enabled) override;

	void addObject(std::shared_ptr<ThrowableTile> object) { m_objects.push_back(object); }

	void setGem(std::shared_ptr<Gem> gem) { m_gem = gem; }

	// Returns the boss' blocks
	std::vector<std::shared_ptr<BossBlock>> getBlocks() const { return m_blocks; }

private:

	void initBlock(int i);

	void takeHit();

	enum class State 
	{
		Vulnerable, // Can be hit, waits
		Jump, // Jumps to make blocks fall
		Wait, // Waits for a little bit before becoming invulnerable
		Move, // Blocks move one by one from pos A to pos B
		TakeHit, // Taking a hit
		Dying // For the dying animation
	};

	enum Animation 
	{
		Default = 0,
		TakingDamage,
		Count
	};

	State m_state = State::Vulnerable;

	// The boss only has two positions and alternates between them
	glm::ivec2 m_first_pos;
	glm::ivec2 m_other_pos;

	// The camera, which will be used to make it vibrate
	std::shared_ptr<Camera> m_camera;

	// The little face that appears on blocks when they move from pos A to pos B
	std::shared_ptr<Sprite> m_miniface;

	// The offset of the face with respect to the position
	static int S_FACE_OFFSET;

	// The boss' blocks
	// 0 1 2
	// 3 4 5
	// 6 7 8
	std::vector<std::shared_ptr<BossBlock>> m_blocks;

	// Each block's offset with respect to the position
	// 0 1 2
	// 3 4 5
	// 6 7 8
	std::vector<glm::ivec2> m_block_offsets;

	std::vector<std::shared_ptr<ThrowableTile>> m_objects;

	std::shared_ptr<Gem> m_gem;

	// True iff hits with objects can hurt the boss
	bool m_vulnerable = true;

	// Helps jump between states
	bool m_sent_event = false;

	// The block that is currently being sent
	int m_currently_sending = -1;

	// The last block to be sent (depends on the random sequence)
	int m_last_to_send;

	// True iff currently in the first pos, thus not in other_pos
	bool m_currently_first_pos = true;

	// The amount of health the boss has
	int m_life = 3;
};

#endif // _BOSS_INCLUDE
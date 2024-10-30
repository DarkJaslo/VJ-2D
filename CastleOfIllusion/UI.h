#ifndef _UI_INCLUDE
#define _UI_INCLUDE

#include <functional>

#include "Sprite.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "Text.h"
#include <vector>

enum class Screen;

class UI
{
public:
	UI() = default;

	void init(std::shared_ptr<ShaderProgram> shader_program, Screen mode);

	// Renders the UI
	void render();

	// Updates the UI values
	void update(int delta_time);

	// Sets the UI's position (bottom center)
	void setPosition(glm::vec2 pos);

	// Sets the power displayed in the UI
	void setPower(int power);

	// Sets the number of tries displayed in the UI
	void setTries(int tries);

	// Sets the score displayed in the UI
	void setScore(int score);

	// Sets the time displayed in the UI
	void setTime(int time);

	void setChangeSceneCallback(std::function<void(Screen)> callback);

	void changeScene(Screen scene_id);

	glm::ivec2 getSize() const { return m_base_sprite->getQuadSize(); }

private:

	std::function<void(Screen)> m_change_scene_callback;

	// The coordinates of the midpoint in the base the UI
	glm::vec2 m_pos;

	Screen m_current_mode;

	// The spritesheet for the static base
	std::shared_ptr<Texture> m_base_spritesheet;

	// The sprite for the static base
	std::shared_ptr<Sprite> m_base_sprite;

	//// Level UI ////

	// The spritesheet of the power stars
	std::shared_ptr<Texture> m_power_spritesheet;

	// A vector holding one sprite for each power star
	std::vector<std::shared_ptr<Sprite>> m_power_sprite;

	int m_power;
	int m_tries;
	int m_score = 0;

	int m_start_level_time = 400;
	double m_start_application_time;
	int m_time_left;

	// The text that displays the number of tries left
	std::shared_ptr<Text> m_tries_text;

	// The text that displays the player's score
	std::shared_ptr<Text> m_score_text;
	
	// The text that displays the time left
	std::shared_ptr<Text> m_time_text;

	//// Start screen UI ////

	std::shared_ptr<Texture> m_startscreen_buttons_spritesheet;

	std::vector < std::shared_ptr<Sprite> > m_startscreen_buttons;

	std::shared_ptr<Texture> m_selection_arrow_spritesheet;

	std::shared_ptr<Sprite> m_selection_arrow;

	int m_selected_button = 0;
};

#endif // UI_INCLUDE
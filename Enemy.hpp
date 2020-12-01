#pragma once

/*
 * This defines an Enemy class. You shouldn't instantiate an enemy here--
 * go through an EnemyManager, which will keep iterators to each enemy,
 * and allow you to loop through them based on type.
 */

#include <glm/glm.hpp>
#include <string>

class Enemy {
	public:


		/* Note:
		 * Ideally, you should never use get_type to give a certain enemy type
		 * special behavior. Instead, a flag for that special behavior can
		 * be added to the enemies.yaml file. That flag will be parsed,
		 * and can be fetched with a "get_flag" function.
		 */

		float              get_flag_numerical(const std::string &str) const;
		const std::string &get_flag_string(const std::string &str) const;
		glm::vec2          get_position();


		friend class EnemyManager;

	protected:

		glm::vec2 position;
		void update(float elapsed);




};

#pragma once

#include "Bullet.hpp"
#include "Numeric.hpp"
#include <list>

class BulletSequencer {


	// time: the time before the previous bullet to shoot this bullet.
	// velocity: the velocity of the bullet, as a multiple of the global bullet velocity
	// angle: the ange of the bullet's trajectory, offset from the vector from the attacker to the player
	// color: <0.5: enemy color, >0.5: not enemy color. done like this so randomness can be used
	// number: amount of bullets of this type
	// sequence: if shooting multiple bullets, whether to start them all at the same time (parallel) or one after the other (serial)
	// dispatch: whether to shoot this at the same time as the last bullet (with) or after the last bullet is shot (after)
	public:

		struct AbstractBulletInfo {
			Numeric time;
			Numeric velocity;
			Numeric angle;
			Numeric color;
			Numeric number;
			bool parallel;
			bool with_previous;
		};

		struct ConcreteBulletInfo {
			float         time;
			float         velocity;
			float         angle;
			bool          your_color;
		};

		BulletSequencer(const std::list<AbstractBulletInfo> &bullets_) : bullets(bullets_) {}
		~BulletSequencer(){}

		std::list<ConcreteBulletInfo> get_firing_pattern() const;

	private:

		std::list<AbstractBulletInfo> bullets;

};

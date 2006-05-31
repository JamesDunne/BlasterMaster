// C Source File
// Created 12/15/2003; 5:04:14 PM
#include "common.h"
#include "bm.h"
#include "entflags.h"
#include "entities.h"
#include "newmap.h"

entity_t	**entities;
int			last_entity, level_changed, e_lastfound;
unsigned long	uid = 1;

entity	e_spawn(class_type class) {
	int	i;

	// Hasn't been designed yet?
	if (class_properties[class] == NULL) return NULL;

	for (i=0;i<MAX_ENTITIES;++i) {
		if (entities[i] == NULL) {
			if (i > last_entity) last_entity = i;
			entities[i] = malloc(sizeof(entity_t));
			// Zero-memory the entity:
			memset(entities[i], 0, sizeof(entity_t));
			// Set the properties:
			entities[i]->index = i;
			entities[i]->class = class;
			entities[i]->uniqueID = uid++;
			// Call initialization function for the class:
			call_process_func(entities[i], init);
			return entities[i];
		}
	}

	return NULL;
}

entity	e_spawnat(class_type class, fixed x, fixed y) {
	entity e = e_spawn(class);
	if (e == NULL) return NULL;
	e->x = screen_mx + x;
	e->y = screen_my + y;
	e->savex = e->x;
	e->savey = e->y;
	return e;
}

entity	e_spawnatmap(class_type class, fixed mx, fixed my) {
	entity e = e_spawn(class);
	if (e == NULL) return NULL;
	e->x = mx * 32.0;
	e->y = my * 32.0;
	e->savex = e->x;
	e->savey = e->y;
	return e;
}

// Mark the entity for deletion:
void e_kill(entity e) {
	e->killed = 1;
}

entity e_findfirst() {
	int	i;
	for (i=0; i<=last_entity; ++i) if (entities[i] != NULL) {
		e_lastfound = i;
		return entities[i];
	}

	e_lastfound = -1;
	return NULL;
}

entity e_findnext() {
	int	i;
	for (i=e_lastfound+1; i<=last_entity; ++i) if (entities[i] != NULL) {
		e_lastfound = i;
		return entities[i];
	}

	return NULL;
}

// Free all the entities marked for deletion (killed == 1):
void free_killed() {
	int i;

	for (i=0; i<=last_entity; ++i) if (entities[i] != NULL) {
		if (entities[i]->killed) {
			free(entities[i]);
			entities[i] = NULL;
		}
	}

	// Reset the last_entity marker:
	for (i=last_entity; i>=0; --i)
		if (entities[i] != NULL) {
			last_entity = i;
			break;
		}
}

// Convert a pointer to a uniqueID:
void e_ptr2uid(entity_t **p) {
	/*
	fprintf(stderr, "0x%08X (ptr)  ->  ", *p);
	if (*p == NULL) {
		(unsigned long)(*p) = 0;
	} else {
		(unsigned long)(*p) = (unsigned long)((*p)->uniqueID);
	}

	fprintf(stderr, "0x%08X (uid)\n", *p);
	*/
	p;
}

// Convert a uniqueID to a pointer:
void e_uid2ptr(entity_t **p) {
	/*
	int	i;
	fprintf(stderr, "0x%08X (uid)  ->  ", *p);
	// Probably want to optimize this with a mapping table:
	for (i=0; i<=last_entity; ++i) if (entities[i] != NULL) {
		if (entities[i]->uniqueID == (unsigned long)(*p)) {
			*p = entities[i];
			fprintf(stderr, "0x%08X (ptr)\n", *p);
			return;
		}
	}
	*p = NULL;
	fprintf(stderr, "0x%08X (ptr)\n", *p);
	*/
	p;
}

// Convert all entity pointers into unique IDs:
void e_default_store(entity e) {
	e_ptr2uid( &(e->owner) );
}

void e_default_restore(entity e) {
	e_uid2ptr( &(e->owner) );
}

// When switching control of players, clear the control_keys bits
// so keys aren't stuck.
void control_switch(entity e) {
	if (player) {
		player->control_keys = 0;
		player->old_control_keys = 0;
	}
	player = (controlled_entity) e;
}

int boxtest(fixed ax1, fixed ay1, fixed ax2, fixed ay2, fixed bx1, fixed by1, fixed bx2, fixed by2) {
	// If any one of these tests passes, then the collision is true.
	// However, we cannot assume false until we have tested all
	// possibilities.

	// Top-left corner
	if ((bx1 >= ax1) && (bx1 <= ax2) && (by1 >= ay1) && (by1 <= ay2)) return 0;
	if ((bx2 >= ax1) && (bx2 <= ax2) && (by2 >= ay1) && (by2 <= ay2)) return 0;
	// Bottom-right corner
	if ((ax1 >= bx1) && (ax1 <= bx2) && (ay1 >= by1) && (ay1 <= by2)) return 0;
	if ((ax2 >= bx1) && (ax2 <= bx2) && (ay2 >= by1) && (ay2 <= by2)) return 0;
	// Bottom-left corner
	if ((bx1 >= ax1) && (bx1 <= ax2) && (by2 >= ay1) && (by2 <= ay2)) return 0;
	if ((bx2 >= ax1) && (bx2 <= ax2) && (by1 >= ay1) && (by1 <= ay2)) return 0;
	// Top-right corner
	if ((ax1 >= bx1) && (ax1 <= bx2) && (ay2 >= by1) && (ay2 <= by2)) return 0;
	if ((ax2 >= bx1) && (ax2 <= bx2) && (ay1 >= by1) && (ay1 <= by2)) return 0;

	// If no corners intersect, then test if the lines intersect:
	// TODO:  Complete this section.  (not very likely that execution will reach this point)

	// We assume they do not collide then:
	return -1;
}

// Returns 0 if two entities have collided, -1 if not.
int entity_collision(entity a, entity b) {
	// Get box coordinates set up for testing:
	fixed	ax1 = a->x + a->ecrx1, ay1 = a->y + a->ecry1;
	fixed	ax2 = a->x + a->ecrx2, ay2 = a->y + a->ecry2;
	fixed	bx1 = b->x + b->ecrx1, by1 = b->y + b->ecry1;
	fixed	bx2 = b->x + b->ecrx2, by2 = b->y + b->ecry2;

	// Now {ax1,ay1}-{ax2,ay2} and {bx1,by1}-{bx2,by2} are the boxes to test.
	return boxtest(ax1, ay1, ax2, ay2, bx1, by1, bx2, by2);
}

void bounds_check(entity self) {
	int	i;
	fixed	ex, ey, edx, edy, x, y;
	unsigned char m, m2;
	entity	o;

	// Apply gravity:
	if (!(self->flags & FLAG_NOGRAVITY) && !(self->flags & FLAG_ONGROUND))
		self->dy += map.gravity;

	// Check max speed in horizontal:
	if (self->dx >  self->max_dx) self->dx =  self->max_dx;
	if (self->dx < -self->max_dx) self->dx = -self->max_dx;

	// Test against other entities:
	for (i=0; i<=last_entity; ++i)
		if (((o = entities[i]) != NULL) && (entities[i] != self) && (entities[i]->killed == 0)) {
			// Only against other entities on the screen!
			/*if ( (ABS(o->x) >> 15 >= screen_mx - 32) && (ABS(o->x) >> 15 <= screen_mx + screen_w + 32) &&
				 (ABS(o->y) >> 15 >= screen_my - 32) && (ABS(o->y) >> 15 <= screen_my + screen_h + 32) )*/
			{
				// Test if the two rectangles have collided:
				if (entity_collision(self, o) == 0) {
					// Run the 'Touched' function for both entities that
					// collided:
					call_interact_func(self, o, touched);
					call_interact_func(o, self, touched);
				}
			}
		}

	// Map collision:
	ex = self->x;
	ey = self->y;
	edx = (self->x + self->dx);
	edy = (self->y + self->dy);

	if (self->flags & FLAG_THRUDOOR) {
		if ((self->face_dir == 2) || (self->face_dir == 6)) {
			// Travelling thru a door:
			m  = gettileat(wrap_map_coord_x(edx + self->mcrx1), wrap_map_coord_y(ey));
			m2 = gettileat(wrap_map_coord_x(edx + self->mcrx2), wrap_map_coord_y(ey));
			if (!(map.mapflags[m] & MAPFLAG_SOLID) && !(map.mapflags[m2] & MAPFLAG_SOLID)) {
				// Ended running thru the door:
				call_interact_func(self, world, touched);
			}
		} else if ((self->face_dir == 0) || (self->face_dir == 4)) {
			// Travelling thru a door:
			m  = gettileat(wrap_map_coord_x(ex), wrap_map_coord_y(edy + self->mcry1));
			m2 = gettileat(wrap_map_coord_x(ex), wrap_map_coord_y(edy + self->mcry2));
			if (!(map.mapflags[m] & MAPFLAG_SOLID) && !(map.mapflags[m2] & MAPFLAG_SOLID)) {
				// Ended running thru the door:
				call_interact_func(self, world, touched);
			}
		}
	} else if (!(self->flags & FLAG_NOMAPCOLLISION)) {
		int	test;

		// Damage the entity if they're in a harmful area:
		if (map.mapflags[gettileat(ex, ey)] & MAPFLAG_HARMFUL)
			e_damage(self, world, 64);

		self->collide_flags = 0;

#if 0
		// Grab the tile beneath us:
		test = -1;
		for (x=ex + self->mcrx1; x<=ex + self->mcrx2; x += 1) {
			m  = gettileat(wrap_map_coord_x(x), wrap_map_coord_y(edy + self->mcry2 + 1));
			self->ctx = x; self->cty = edy + self->mcry2 + 1;
			// Call the class-defined maptest function, otherwise check against MAPFLAG_SOLID:
			if (class_properties[self->class]->maptest)
				test &= !(class_properties[self->class]->maptest(map.mapflags[m]));
			else
				test &= !(map.mapflags[m] & MAPFLAG_SOLID);
		}
		// Solid tile?
		if (!test) {
			if ((self->dy > 0L) && !(self->flags & FLAG_ONGROUND) && !(self->flags & FLAG_JUMPING)) {
				self->collide_mapflags = map.mapflags[m];
				self->cmx = self->x;  self->cmy = self->y;
				self->cdx = self->dx; self->cdy = self->dy;

				self->y = ((long)((self->cty - 32) * 0.03125) * 32) + (32 - (self->mcry2 + 1));
				self->dy = 0;
				self->flags |= FLAG_ONGROUND;
				self->collide_flags |= COLLIDE_BELOW;

				// Touched the world:
				call_interact_func(self, world, touched);
			}
		} else if (test) {
			// Not solid.
			if (self->flags & FLAG_ONGROUND) self->flags &= ~FLAG_ONGROUND;
		}
#else
		// Grab the tile beneath us:
		test = 0;
		for (x=ex + self->mcrx1; x<=ex + self->mcrx2; x += 1) {
			m  = gettileat(wrap_map_coord_x(x), wrap_map_coord_y(edy + self->mcry2 + 1));
			self->ctx = x; self->cty = edy + self->mcry2 + 1;
			// Call the class-defined maptest function, otherwise check against MAPFLAG_SOLID:
			if (class_properties[self->class]->maptest)
				test |= (class_properties[self->class]->maptest(map.mapflags[m]));
			else
				test |= (map.mapflags[m] & MAPFLAG_SOLID);
		}
		// Solid tile?
		if (test) {
			if ((self->dy > 0L) && !(self->flags & FLAG_ONGROUND) && !(self->flags & FLAG_JUMPING)) {
				self->collide_mapflags = map.mapflags[m];
				self->cmx = self->x;  self->cmy = self->y;
				self->cdx = self->dx; self->cdy = self->dy;

				self->y = ((long)((self->cty - 32) * 0.03125) * 32) + (32 - (self->mcry2 + 1));
				self->dy = 0;
				self->flags |= FLAG_ONGROUND;
				self->collide_flags = COLLIDE_BELOW;

				// Touched the world:
				call_interact_func(self, world, touched);
			}
		} else if (!test) {
			// Not solid.
			if (self->flags & FLAG_ONGROUND) self->flags &= ~FLAG_ONGROUND;
		}
#endif

		// Grab the tile above us:
		if ((self->dy < 0) && !(self->collide_flags & COLLIDE_BELOW)) {
			test = 0;
			for (x=ex + self->mcrx1; x<=ex + self->mcrx2; x += 1) {
				m  = gettileat(wrap_map_coord_x(x), wrap_map_coord_y(edy + self->mcry1));
				self->ctx = x; self->cty = edy + self->mcry1;
				if (class_properties[self->class]->maptest)
					test |= (class_properties[self->class]->maptest(map.mapflags[m]));
				else
					test |= (map.mapflags[m] & MAPFLAG_SOLID);
				if (test) break;
			}
			// Any solid tile?
			if (test) {
				self->collide_mapflags = map.mapflags[m];
				self->cmx = self->x;  self->cmy = self->y;
				self->cdx = self->dx; self->cdy = self->dy;

				self->y = ((long)((self->cty + 32) * 0.03125) * 32) - self->mcry1 + 1;
				self->dy = 0;
				self->collide_flags = COLLIDE_ABOVE;

				// Touched the world:
				call_interact_func(self, world, touched);
			}
		}

		// Grab the tile to the left:
		test = 0;
		for (y=ey + self->mcry1; y<=ey + self->mcry2; y += 1) {
			m  = gettileat(wrap_map_coord_x(edx + self->mcrx1), wrap_map_coord_y(y));
			self->ctx = edx + self->mcrx1; self->cty = y;
			if (class_properties[self->class]->maptest)
				test |= (class_properties[self->class]->maptest(map.mapflags[m]));
			else
				test |= (map.mapflags[m] & MAPFLAG_SOLID);
			if (test) break;
		}
		// Any solid tile?
		if (test) {
			// Store current state before collision occured:
			self->collide_mapflags = map.mapflags[m];
			self->cmx = self->x; self->cmy = self->y;
			self->cdx = self->dx; self->cdy = self->dy;

			self->dx = 0;
			self->collide_flags = COLLIDE_LEFT;

			// Touched the world:
			call_interact_func(self, world, touched);
		}

		// Grab the tile to the right:
		test = 0;
		for (y=ey + self->mcry1; y<=ey + self->mcry2; y += 1) {
			m  = gettileat(wrap_map_coord_x(edx + self->mcrx2), wrap_map_coord_y(y));
			self->ctx = edx + self->mcrx2; self->cty = y;
			if (class_properties[self->class]->maptest)
				test |= (class_properties[self->class]->maptest(map.mapflags[m]));
			else
				test |= (map.mapflags[m] & MAPFLAG_SOLID);
			if (test) break;
		}
		// Any solid tile?
		if (test) {
			self->collide_mapflags = map.mapflags[m];
			self->cmx = self->x; self->cmy = self->y;
			self->cdx = self->dx; self->cdy = self->dy;

			self->dx = 0;
			self->collide_flags = COLLIDE_RIGHT;

			// Touched the world:
			call_interact_func(self, world, touched);
		}
	}

	m = gettileat(ex, ey);
	if (map.mapflags[m] & MAPFLAG_WATER) {
		if (self->dy > 4) self->dy = 4;
		if (self->dy < -8) self->dy = -8;
	} else {
		if (self->dy > 15) self->dy = 15;
		if (self->dy < -15) self->dy = -15;
	}
}

void draw_vline_on_screen(int x, int y1, int y2, int offx, int offy) {
	int	vx, vy1, vy2;

	x *= 32;
	if ((x < screen_mx) || (x > (screen_mx + screen_w))) return;

	vx = x - screen_mx + offx;
	vy1 = (y1 * 32) - screen_my;
	vy2 = (y2 * 32) - screen_my + offy;

	if (vy1 < 0) vy1 = 0;
	if (vy2 > screen_h) vy2 = screen_h;

	glBindTexture(GL_TEXTURE_2D, 0);
	glColor4f(0.7, 0.12, 0.5, 0.6);
	glLineWidth(3.0);
	glBegin(GL_LINES);
		glVertex2i(vx, vy1);
		glVertex2i(vx, vy2);
	glEnd();
}

void draw_hline_on_screen(int y, int x1, int x2, int offx, int offy) {
	int	vy, vx1, vx2;

	y *= 32;
	if ((y < screen_my) || (y > (screen_my + screen_h))) return;

	vy = y - screen_my + offy;
	vx1 = (x1 * 32) - screen_mx;
	vx2 = (x2 * 32) - screen_mx + offx;

	if (vx1 < 0) vx1 = 0;
	if (vx2 > screen_w) vx2 = screen_w;

	glBindTexture(GL_TEXTURE_2D, 0);
	glColor4f(0.7, 0.12, 0.5, 0.6);
	glLineWidth(3.0);
	glBegin(GL_LINES);
		glVertex2i(vx1, vy);
		glVertex2i(vx2, vy);
	glEnd();
}

void process_entities() {
	int		i, j, m, goodent;
	fixed	x, y, x1, y1, x2, y2;
	entity	e;

	old_screen_mx = screen_mx;
	old_screen_my = screen_my;

	// Go through each entity up to the last known one:
	for (j=0;j<=last_entity;++j) if ((e = entities[j]) != NULL) {
		// Determine if the entity is to be processed this frame:
		if (e->flags & FLAG_ONSCREEN_ONLY) {
			x = e->x; y = e->y;
			x1 = screen_mx - e->ecrx2;
			y1 = screen_my - e->ecry2;
			x2 = screen_mx + e->ecrx1 + screen_w;
			y2 = screen_my + e->ecry1 + screen_h;
			// Right side of screen spills over:
			if (x2 >= (map.width * 32.0)) {
				if (((x < x1) || (x > map.width * 32.0)) && ((x < 0) || (x > x2 - (map.width * 32.0)))) continue;
			} else {
				if ((x < x1) || (x >= x2)) continue;
			}
			if (y2 >= (map.height * 32.0)) {
				if (((y < y1) || (y > map.height * 32.0)) && ((y < 0) || (y > y2 - (map.height * 32.0)))) continue;
			} else {
				if ((y < y1) || (y >= y2)) continue;
			}
		}

		if (e != (entity)player) {
			// If the game is paused, don't do any thinking or moving:
			if (!game_paused) {
				// Think before physics:
				call_process_func(e, prethink);
				if (e->killed) continue;

				// Check bounds against system:
				bounds_check(e);
				if (e->killed) continue;

				// Apply momentum:
				m = map.mapflags[gettileat(e->x, e->y)];
				if ((m & MAPFLAG_WATER) && !(m & MAPFLAG_SOLID)) {
					e->x = wrap_map_coord_x(e->x + (e->dx * 0.5) * time_dt);
					e->y = wrap_map_coord_y(e->y + (e->dy * 0.5) * time_dt);
				} else {
					e->x = wrap_map_coord_x(e->x + e->dx * time_dt);
					e->y = wrap_map_coord_y(e->y + e->dy * time_dt);
				}

				// Apply friction:
				if (!(e->flags & FLAG_NOFRICTION) && (e->flags & FLAG_ONGROUND))
					e->dx -= e->dx * map.friction;

				// Think after physics: (additional bounds checking...)
				call_process_func(e, postthink);
				if (e->killed) continue;
			}

			// Draw this entity if it wants to be drawn:
			call_process_func(e, draw);
			if (level_changed) {
				level_changed = 0;
				break;
			}
		}
	}

	if (player) {
		int	lx, rx;
		int	ty, by;
		int	reg = -1;

		e = (entity)player;

		if (!game_paused) {
			// Think before physics:
			call_process_func(e, prethink);

			// Check bounds against system:
			bounds_check(e);

			// Apply momentum:
			m = map.mapflags[gettileat(e->x, e->y)];
			if ((m & MAPFLAG_WATER) && !(m & MAPFLAG_SOLID)) {
				e->x = wrap_map_coord_x(e->x + (e->dx * 0.5));
				e->y = wrap_map_coord_y(e->y + (e->dy * 0.5));
			} else {
				e->x = wrap_map_coord_x(e->x + e->dx);
				e->y = wrap_map_coord_y(e->y + e->dy);
			}

			// Apply friction:
			if (!(e->flags & FLAG_NOFRICTION) && (e->flags & FLAG_ONGROUND))
				e->dx -= e->dx * map.friction;

			// Think after physics: (additional bounds checking...)
			call_process_func(e, postthink);
		}

		if ((player->flags & FLAG_THRUDOOR) == 0) {
			// Scroll:
			if ( test_map_x_gt(player->x, screen_mx + scroll_right) )
				screen_mx = (player->x - scroll_right);
			if ( test_map_x_lt(player->x, screen_mx + scroll_left) )
				screen_mx = (player->x - scroll_left);
			if ( test_map_y_gt(player->y, screen_my + scroll_bottom) )
				screen_my = (player->y - scroll_bottom);
			if ( test_map_y_lt(player->y, screen_my + scroll_top) )
				screen_my = (player->y - scroll_top);
		}

		for (i = 0; i < map.num_regions; ++i) {
			lx = (map.regions[i]->lx << 5);
			rx = (map.regions[i]->rx << 5) + 31;
			ty = (map.regions[i]->ty << 5);
			by = (map.regions[i]->by << 5) + 31;

			int	test = 0;
			if (lx > rx) {
				if (player->x >= lx && player->x <= (map.width * 32)) test = -1;
				else if (player->x >= 0 && player->x <= rx) test = -1;
				else test = 0;
			} else {
				if (player->x >= lx && player->x <= rx) test = -1;
				else test = 0;
			}
			if (test == 0) continue;
			if (ty > by) {
				if (player->y >= ty && player->y <= (map.height * 32)) test = -1;
				else if (player->y >= 0 && player->y <= by) test = -1;
				else test = 0;
			} else {
				if (player->y >= ty && player->y <= by) test = -1;
				else test = 0;
			}
			if (test == -1) {
				reg = i;
				break;
			}
		}

		// We're inside a scroll-region:
		if (reg != -1) {
			if (lx > rx) {
				if ((screen_mx < lx) && (screen_mx > (rx - screen_w))) {
					if (abs(screen_mx - lx) < abs(screen_mx - (rx - screen_w)))
						screen_mx = lx;
					else
						screen_mx = (rx - screen_w);
				}
			} else {
				if (screen_mx < lx)
					screen_mx = lx;
				if (screen_mx > (rx - screen_w))
					screen_mx = (rx - screen_w);
			}
			if (ty > by) {
				if ((screen_my < ty) && (screen_my > (by - screen_h))) {
					if (abs(screen_my - ty) < abs(screen_my - (by - screen_h)))
						screen_my = ty;
					else
						screen_my = (by - screen_h);
				}
			} else {
				if (screen_my < ty)
					screen_my = ty;
				if (screen_my > (by - screen_h))
					screen_my = (by - screen_h);
			}
		}

		screen_mx = wrap_map_coord_x(screen_mx);
		screen_my = wrap_map_coord_y(screen_my);

		// Draw this entity if it wants to be drawn:
		call_process_func(e, draw);
		if (level_changed) {
			level_changed = 0;
		}
	}

	// Free all the killed entities:
	free_killed();
}

void e_damage(entity target, entity attacker, short damage) {
	// Does the target take damage?
	if (!(target->flags & FLAG_TAKEDAMAGE)) return;
	// It's invincible?
	if (target->flags & FLAG_INVINCIBLE) return;

	// Enough time passed from the last damage?
	if (target->dmg_timer <= 0) {
		target->dmg_timer = target->dmg_time;

		// Take away health:
		if (damage > 0) {
			// Call the attacked function:
			if (class_properties[target->class]->attacked) {
				// We have one, so ask the target permission to deduct health:
				if (class_properties[target->class]->attacked(target, attacker, damage)) {
					target->health -= damage;
				}
			} else {
				// No attacked function, assume it is accepting the damage:
				target->health -= damage;
			}
		} else {
			// No problem with giving health:
			target->health -= damage;
			if (target->health > target->maxhealth) target->health = target->maxhealth;
		}

		if (target->health <= 0) {
			// Call the death function:
			call_process_func(target, death);
			target->health = 0;
		}
	}
}

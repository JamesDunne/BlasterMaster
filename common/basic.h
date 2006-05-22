// These fields are included in every entity structure and are the basis
// for an entity.  Each entity derived from this structure should #include
// this file FIRST, before ANY OTHER declarations for the derived entity.
// All general entity handling functions will be passed an (entity_t *) and
// then cast the pointer into its respective entity-class's structure.

	int				index;							// This entity's index in the entities[] array
	class_type		class;							// What kind of object this is
	unsigned long	uniqueID;						// This entity's unique ID
	char			killed;							// Set if the entity is to be killed this frame
	teams			team;							// Basically 2 teams, Jason+Tank  vs.  Everyone else

	struct entity_t	*owner;							// This entity belongs to this entity.

	// Physics:	(all values are fixed point 16.15 (last bit is +/-))
	fixed			x, y;
	fixed			dx, dy, max_dx, accel;
	fixed			savex, savey;

	// Collision rectangle:	(in pixels, not fixed-point)
	short			ecrx1, ecry1, ecrx2, ecry2;		// For entity-entity collision
	short			mcrx1, mcry1, mcrx2, mcry2;		// For entity-map collision

    unsigned char	collide_mapflags, collide_flags;
	long			ctx, cty;		// Exact position (not fixed) where the collision occured
	fixed			cmx, cmy;		// Previous location before collision adjustment
	fixed			cdx, cdy;		// Previous deltas before collision adjustment

	// Appearance:
	unsigned int	frame, fr_time;
	signed char		face_dir;

	short			dmg_timer, dmg_time;

	// Status:
	short			health, maxhealth;
	unsigned long	flags;							// Misc flags

	short			flicker_time;
	unsigned char	flicker_count;


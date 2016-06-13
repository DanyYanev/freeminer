/*
cavegen.h
Copyright (C) 2010-2013 kwolekr, Ryan Kwolek <kwolekr@minetest.net>
*/

/*
This file is part of Freeminer.

Freeminer is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Freeminer  is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Freeminer.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CAVEGEN_HEADER
#define CAVEGEN_HEADER

#define VMANIP_FLAG_CAVE VOXELFLAG_CHECKED1
#define DEFAULT_LAVA_DEPTH (-256)

class GenerateNotifier;

/*
	CavesRandomWalk is an implementation of a cave-digging algorithm that
	operates on the principle of a "random walk" to approximate the stochiastic
	activity of cavern development.

	In summary, this algorithm works by carving a randomly sized tunnel in a
	random direction a random amount of times, randomly varying in width.
	All randomness here is uniformly distributed; alternative distributions have
	not yet been implemented.

	This algorithm is very fast, executing in less than 1ms on average for an
	80x80x80 chunk of map on a modern processor.
*/
class CavesRandomWalk {
public:
	MMVManip *vm;
	INodeDefManager *ndef;
	GenerateNotifier *gennotify;
	s16 *heightmap;

	// configurable parameters
	int seed;
	int water_level;
	int lava_depth;
	NoiseParams *np_caveliquids;

	// intermediate state variables
	u16 ystride;

	s16 min_tunnel_diameter;
	s16 max_tunnel_diameter;
	u16 tunnel_routepoints;
	int part_max_length_rs;

	bool large_cave;
	bool large_cave_is_flat;
	bool flooded;

	s16 max_stone_y;
	v3s16 node_min;
	v3s16 node_max;

	v3f orp;  // starting point, relative to caved space
	v3s16 of; // absolute coordinates of caved space
	v3s16 ar; // allowed route area
	s16 rs;   // tunnel radius size
	v3f main_direction;

	s16 route_y_min;
	s16 route_y_max;

	PseudoRandom *ps;

	content_t c_water_source;
	content_t c_lava_source;
	content_t c_ice;

	// ndef is a mandatory parameter.
	// If gennotify is NULL, generation events are not logged.
	CavesRandomWalk(INodeDefManager *ndef,
		GenerateNotifier *gennotify = NULL,
		int seed = 0,
		int water_level = 1,
		content_t water_source = CONTENT_IGNORE,
		content_t lava_source = CONTENT_IGNORE);

	// vm and ps are mandatory parameters.
	// If heightmap is NULL, the surface level at all points is assumed to
	// be water_level.
	void makeCave(MMVManip *vm, v3s16 nmin, v3s16 nmax, PseudoRandom *ps,
		bool is_large_cave, int max_stone_height, s16 *heightmap);

private:
	void makeTunnel(bool dirswitch);
	void carveRoute(v3f vec, float f, bool randomize_xz);

	inline bool isPosAboveSurface(v3s16 p);
};

/*
	CavesV6 is the original version of caves used with Mapgen V6.

	Though it uses the same fundamental algorithm as CavesRandomWalk, it is made
	separate to preserve the exact sequence of PseudoRandom calls - any change
	to this ordering results in the output being radically different.
	Because caves in Mapgen V6 are responsible for a large portion of the basic
	terrain shape, modifying this will break our contract of reverse
	compatibility for a 'stable' mapgen such as V6.

	tl;dr,
	*** DO NOT TOUCH THIS CLASS UNLESS YOU KNOW WHAT YOU ARE DOING ***
*/
class CavesV6 {
public:
	MMVManip *vm;
	INodeDefManager *ndef;
	GenerateNotifier *gennotify;
	PseudoRandom *ps;
	PseudoRandom *ps2;

	// configurable parameters
	s16 *heightmap;
	content_t c_water_source;
	content_t c_lava_source;
	content_t c_ice;
	int water_level;

	// intermediate state variables
	u16 ystride;

	s16 min_tunnel_diameter;
	s16 max_tunnel_diameter;
	u16 tunnel_routepoints;
	int part_max_length_rs;

	bool large_cave;
	bool large_cave_is_flat;

	v3s16 node_min;
	v3s16 node_max;

	v3f orp;  // starting point, relative to caved space
	v3s16 of; // absolute coordinates of caved space
	v3s16 ar; // allowed route area
	s16 rs;   // tunnel radius size
	v3f main_direction;

	s16 route_y_min;
	s16 route_y_max;

	// ndef is a mandatory parameter.
	// If gennotify is NULL, generation events are not logged.
	CavesV6(INodeDefManager *ndef,
		GenerateNotifier *gennotify = NULL,
		int water_level = 1,
		content_t water_source = CONTENT_IGNORE,
		content_t lava_source = CONTENT_IGNORE);

	// vm, ps, and ps2 are mandatory parameters.
	// If heightmap is NULL, the surface level at all points is assumed to
	// be water_level.
	void makeCave(MMVManip *vm, v3s16 nmin, v3s16 nmax,
		PseudoRandom *ps, PseudoRandom *ps2,
		bool is_large_cave, int max_stone_height, s16 *heightmap = NULL);

private:
	void makeTunnel(bool dirswitch);
	void carveRoute(v3f vec, float f, bool randomize_xz, bool tunnel_above_ground);

	inline s16 getSurfaceFromHeightmap(v3s16 p);
};

#endif

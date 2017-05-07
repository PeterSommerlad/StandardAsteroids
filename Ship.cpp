#include "Ship.h"
#include "Input.h"
#include "Constants.h"

#include <algorithm>

namespace
{
	using asteroids::path_vertices;
	using asteroids::path_buffer;
	using asteroids::vector_2d;

	path_vertices ship_vb{ vector_2d{ 8, 0 },{ -15, -5 },{ 2, 3 },{ 0, 5 },{ -2, 3 },{ 15, -5 } };
	const asteroids::path_buffer ship_shape{ 6, ship_vb };
}

asteroids::ship::ship(const controllable_physics& cp)
	: m_physics(cp)
	, m_path(ship_shape)
{}

asteroids::ship_update asteroids::ship::update()
{
	if (anti_clockwise())
	{
		m_physics.spin(spin_left);
	}
	if (clockwise())
	{
		m_physics.spin(spin_right);
	}
	if (thrust())
	{
		m_physics.thrust(thrust_force);
	}
	m_physics.update();

	std::transform(&ship_vb[0], &ship_vb[ship_shape.m_count], &m_path.m_vertices[0], [&](const auto & v_in)
	{
		return rotate(v_in, m_physics.orientation(), { 0.0, 0.0 });
	});
	return{ fire(), m_physics.position(), m_physics.orientation() };
}

void asteroids::ship::draw(display_surface& ds)
{
	using namespace std::experimental::io2d;

	auto path = path_builder<>{};
	path.clear();
	auto v = m_physics.position() + (m_path.m_vertices[0]);
	path.move_to(screen_space(v));
	std::for_each(&m_path.m_vertices[1], &m_path.m_vertices[m_path.m_count], [&](const auto& vert)
	{
		v += vert;
		path.line_to(screen_space(v));
	});

	ds.stroke(brush{ bgra_color::white() }, path);
}

asteroids::missile::missile(const vector_2d& position, double orientation, bool active)
	: m_physics(position, pol_to_car({ missile_travel_distance_per_tick, orientation }))
	, m_age(active ? 0.0 : max_missile_age)
{}

bool asteroids::missile::update()
{
	if (!active()) return true;

	m_physics.update();
	m_age += 0.1;
	return active();
}

void asteroids::missile::destroy()
{
	m_age = max_missile_age;
}

bool asteroids::missile::active() const
{
	return (m_age < max_missile_age);
}

void asteroids::missile::draw(display_surface& ds)
{
	using namespace std::experimental::io2d;

	if (!active()) return;

	auto path = path_builder<>{};
	path.move_to(screen_space(m_physics.position()));
	path.line_to(screen_space(m_physics.position() - m_physics.velocity()));

	ds.stroke(brush{ bgra_color::white() }, path);
}
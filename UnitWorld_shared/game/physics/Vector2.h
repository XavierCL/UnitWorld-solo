#pragma once

namespace uw
{
	class Vector2
	{
	public:
		Vector2(const int& x = 0, const int& y = 0);

		const int x() const;
		void x(const int& newX);
		const int y() const;
		void y(const int& newY);
	private:
		int _x;
		int _y;
	};
}
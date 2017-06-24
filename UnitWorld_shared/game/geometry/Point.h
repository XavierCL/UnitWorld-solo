#pragma once

namespace uw
{
	class Point
	{
	public:
		Point(const int& x = 0, const int& y = 0);

		const int x() const;
		void x(const int& newX);
		const int y() const;
		void y(const int& newY);
	private:
		int _x;
		int _y;
	};
}
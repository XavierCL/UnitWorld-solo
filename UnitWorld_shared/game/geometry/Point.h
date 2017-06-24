#pragma once

namespace uw
{
	class Point
	{
	public:
		const int x() const;
		void x(const int& newX);
		const int y() const;
		void y(const int& newY);
	private:
		int _x;
		int _y;
	};
}
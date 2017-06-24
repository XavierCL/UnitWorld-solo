#pragma once

namespace uw
{
	class DisplayCanvas
	{
	public:
		virtual void display() = 0;
		virtual void close() = 0;
	};
}
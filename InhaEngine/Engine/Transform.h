#pragma once

struct Transform
{
	Transform() = default;
	Transform(const Vec3& position, const Quaternion& rotation, const Vec3& scale)
		: position(position), rotation(rotation), scale(scale)
	{
	}

	Vec3 position = Vec3::Zero;
	Quaternion rotation = Quaternion::Identity; // NOTE: This quaternion rotation follows directX coordinate system. (Not game coords)
	Vec3 scale = Vec3::One;
};


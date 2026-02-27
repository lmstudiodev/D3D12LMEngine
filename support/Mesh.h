#pragma once

struct Vec2
{
	float x;
	float y;
};

struct  Vec3
{
	float x; 
	float y;
	float z;
};

struct Vertex
{
	Vec3 pos;
	Vec3 color;
	Vec2 textCoord;
};

struct Correction
{
	float aspectRatio;
	float zoom;
	float sinAngle;
	float cosAngle;
};
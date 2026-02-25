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
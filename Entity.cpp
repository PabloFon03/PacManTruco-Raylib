#include "Entity.h";

Entity::Entity()
{
}

void Entity::Update()
{

	frameCounter++;

}

void Entity::Draw()
{

	DrawRectangle(x * 16, y * 16, 16, 16, YELLOW);

}
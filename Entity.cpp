#include "Entity.h";

Entity::Entity()
{

	animIndex = 0;

}

Entity::~Entity()
{

	UnloadTexture(animAtlas);

}

void Entity::LoadTextureAtlas()
{

	animAtlas = LoadTexture("img/wakawaka.png");
	SetTextureFilter(animAtlas, TEXTURE_FILTER_POINT);

}

void Entity::Update()
{

	float deltaTime = GetFrameTime();

	animDelay--;

	if (animDelay <= 0) {
		
		animIndex++;

		animIndex %= 4;

	}

}

void Entity::Draw()
{

	DrawTextureRec(animAtlas, Rectangle{ (float)animIndex * 16, (float)dir * 16, 16, 16 }, Vector2{ (float)x * 16, (float)y * 16 }, WHITE);

}
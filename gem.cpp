#include <iostream>
#include <SFML/Graphics.hpp>
#include "gem.hpp"


const sf::Vector2f Gem::size {(float)gemSize, (float)gemSize};

Gem::Gem(int col, int row, Color color, sf::Texture& texture, Status status)
	: col(col), row(row), pos(float((Gem::size.x+padding)*col)+gemSize/2.0f, float((Gem::size.y+padding)*row)+gemSize/2.0f),
	target(pos), color(color), status(status), alpha(255), sprite(texture), possibleMatch(false)
{
	sprite.setTextureRect(sf::IntRect(static_cast<int>(color)*gemSize, 0, gemSize, gemSize));
	sprite.setOrigin((float)gemSize/2.0f, (float)gemSize/2.0f);
	sprite.setColor(sf::Color(255, 255, 255, alpha));
	sprite.setPosition(pos);
	sprite.setRotation(0.0f);
	sprite.setScale(1.0f, 1.0f);
}

Gem::~Gem()
{

}

void Gem::draw(sf::RenderWindow& window)
{
	if (status != Gem::Status::NEW) {
		//sprite.setPosition(pos);
		if (possibleMatch)
			sprite.setTextureRect(sf::IntRect(static_cast<int>(color)*gemSize, 2*gemSize, gemSize, gemSize));
		else
			sprite.setTextureRect(sf::IntRect(static_cast<int>(color)*gemSize, (status==Status::SELECTED)?gemSize:0, gemSize, gemSize));
		sprite.setColor(sf::Color(255, 255, 255, alpha));
		window.draw(sprite);
	}
}

bool Gem::checkHit(const sf::Vector2f& spos)
{
	if (status==Status::DELETED) return false;
	return sf::Rect<float>(pos, size).contains(spos);
}

void Gem::swapTargets(Gem& other)
{
	std::swap(col, other.col);
	std::swap(row, other.row);
	
	target = sf::Vector2f(float((Gem::size.x+padding)*col)+gemSize/2.0f, float((Gem::size.y+padding)*row)+gemSize/2.0f);
	other.target = sf::Vector2f(float((Gem::size.x+padding)*other.col)+gemSize/2.0f, float((Gem::size.y+padding)*other.row)+gemSize/2.0f);
	setStatus(Status::MOVING);
	other.setStatus(Status::MOVING);
}

Gem::Status Gem::update()
{
	switch(status)
	{
		case Status::MATCH:
		{
			setStatus(Status::DELETING);
			cyclesToDie = 0;
			break;
		}
		case Status::DELETING:
		{
			sprite.setRotation(sprite.getRotation() + 12.0f);

			alpha -= 10;
			if (alpha <= 50) {
				setStatus(Status::DELETED);
			}
			break;
		}
		case Status::MOVING:
		case Status::DELETED:
		{
			auto move = [=](float& p, float t) -> bool
			{
				if (p == t) return false;
				if (p < t) p += std::min(5.0f, t-p);
				else       p -= std::min(5.0f, p-t);
				return true;
			};
			if (move(pos.x, target.x) || move(pos.y, target.y))
				sprite.setPosition(pos.x + gemSize/2.0f, pos.y + gemSize/2.0f);
			else if(status == Status::MOVING)
				setStatus(Status::NONE);
			break;
		}
		default:
			break;
	}
	return status;
}

void Gem::setStatus(Status status)
{
	if (this->status == Status::DELETED) return;
	this->status = status;
}

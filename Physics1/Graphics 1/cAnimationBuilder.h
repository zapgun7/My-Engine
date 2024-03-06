#pragma once


struct sAnimInfo;

enum AnimType
{
	BOUNCEBALL,
	FLYSHIP
};


class cAnimationBuilder
{
public:
	cAnimationBuilder();
	~cAnimationBuilder();

	sAnimInfo* MakeAnimation(AnimType type);



private:

};
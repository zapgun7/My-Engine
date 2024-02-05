#pragma once


class sAnimInfo;

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
#pragma once

#include "SuperBase.h"

class Level : public SuperBase
{
private:
	Level();
	virtual ~Level() = default;


public:
	virtual	void Free() override;
};


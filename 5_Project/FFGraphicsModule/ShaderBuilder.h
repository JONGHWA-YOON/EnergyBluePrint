#pragma once
#include "IBuilder.h"

class ShaderBuilder : public IBuilder
{
public:
	virtual void Build(IResource** p_resource) override;

public:
	ShaderInfo m_info;

};


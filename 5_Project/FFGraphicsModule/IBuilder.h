#pragma once

class IResource;

class IBuilder
{
public:
	virtual void Build(IResource** p_resource) abstract;
};


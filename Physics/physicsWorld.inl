#pragma once

const std::vector<BodyId>& physicsWorld::getActiveBodyIds() const
{
	return m_activeBodyIds;
}
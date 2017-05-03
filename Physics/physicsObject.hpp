#pragma once

static int ctr = 0;

class physicsObject
{
  public:
	physicsObject();

	inline const int getID() const { return m_id; }

  protected:
	int m_id;
	
};

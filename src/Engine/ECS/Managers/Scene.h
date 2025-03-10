#pragma once

class Scene
{
public:
	virtual ~Scene() = default;
	void virtual OnEnter() = 0;
	void virtual OnFixedUpdate() = 0;
	void virtual OnUpdate() = 0;
	void virtual OnExit() = 0;
	void virtual Render() = 0;
};

